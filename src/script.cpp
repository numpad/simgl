#include <vector>
#include <string>

#include <stdio.h>

#include "sgl_window.hpp"
#include "sgl_window_context.hpp"
#include "sgl_mesh.hpp"
#include "sgl_texture.hpp"
#include "sgl_shader.hpp"

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptarray/scriptarray.h>
#include <scripthelper/scripthelper.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::string ASErrorMessage;

class ConsoleLog {
	std::string log_msg;
	ImVec4 color;
public:
	enum Level {
		ERR, WARN, INFO, LOG
	} log_level;

	ConsoleLog(std::string text, ConsoleLog::Level level = ConsoleLog::LOG) : log_msg(text), log_level(level) {
		this->color = LogLevel_to_Color(level);
	}
	
	ImVec4 LogLevel_to_Color(ConsoleLog::Level level) {
		ImVec4 c;
		switch (level) {
		case ConsoleLog::Level::ERR:
			c = ImVec4(0.7f, 0.0f, 0.0f, 1.0f);
			break;
		case ConsoleLog::Level::WARN:
			c = ImVec4(0.7f, 0.7f, 0.0f, 1.0f);
			break;
		case ConsoleLog::Level::INFO:
			c = ImVec4(0.2f, 0.4f, 0.7f, 1.0f);
			break;
		case ConsoleLog::Level::LOG:
		default:
			c = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
			break;
		};

		return c;
	}

	std::string &get_message() {
		return this->log_msg;
	}

	int get_level() {
		return (int)this->log_level;
	}

	ImVec4 &get_color() {
		return this->color;
	}
};

class ImConsole {
	std::vector<ConsoleLog> logs;
	
	asIScriptEngine *script_engine;
	asIScriptModule *script_module;
	asIScriptContext *script_ctx;

	std::string title, childname;

	static constexpr size_t CMD_INPUT_SIZE = 512;
	bool message_added = false,
	     command_entered = false;
	char command_input[CMD_INPUT_SIZE];
	
	int _refcount;
public:
	
	ImConsole(std::string title, asIScriptEngine *engine, asIScriptContext *ctx, std::string modname) {
		this->_refcount = 1;
		this->title = title;
		this->childname = title + "_child";

		script_engine = engine;
		script_module = engine->GetModule(modname.c_str());
		script_ctx = ctx;
	}
	
	void AddRef() {
		this->_refcount++;
		std::cout << "Added Ref #" << _refcount << std::endl;
	}
	void Release() {
		std::cout << "Removed Ref #" << _refcount << std::endl;
		if (--(this->_refcount) == 0) {
			delete this;
		}
	}

	void print(ConsoleLog l) {
		this->message_added = true;
		logs.push_back(l);
	}

	void print(std::string msg, ConsoleLog::Level level = ConsoleLog::Level::LOG) {
		this->print(ConsoleLog(msg, level));
	}
	
	void log(std::string msg) {
		this->print(msg);
	}

	void render() {
		ImGui::Begin(this->title.c_str(), NULL, ImGuiWindowFlags_NoScrollbar);
			ImGui::BeginChild(this->childname.c_str(), ImVec2(0, -28.0f));
				for (size_t i = 0; i < this->logs.size(); ++i) {
					ImGui::TextColored(this->logs.at(i).get_color(), this->logs.at(i).get_message().c_str());
				}

				if (this->message_added) {
					this->message_added = false;
					ImGui::SetScrollHere();
				}
			ImGui::EndChild();

			ImGui::Separator();
			bool cmd_entered = ImGui::InputText("##command", command_input, this->CMD_INPUT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			if (ImGui::Button("Enter") || cmd_entered) {
				this->print(ConsoleLog(std::string(command_input), ConsoleLog::Level::WARN));
				int r = ExecuteString(this->script_engine, command_input, script_module, script_ctx);
				if (r < 0) {
					this->print(ASErrorMessage, ConsoleLog::Level::ERR);
				}

				command_input[0] = '\0';
			}
		ImGui::End();
	}
};

sgl::window window_create() {
	sgl::window_context wctx;
	wctx.resizable = false;
	sgl::window window(wctx);
	window.on_resize([](sgl::window &, int w, int h) { glViewport(0, 0, w, h); });
	
	return window;
}

void MessageCallback(const asSMessageInfo *msg, void *param) {
	const char *type;
	
	switch (msg->type) {
	case asMSGTYPE_ERROR:
		type = "ERR";
		break;
	case asMSGTYPE_WARNING:
		type = "WARN";
		break;
	case asMSGTYPE_INFORMATION:
		type = "INFO";
		break;
	};
	
	ASErrorMessage = std::string(msg->message);
	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void ExecuteError(asIScriptContext *ctx, int r) {
	if(r != asEXECUTION_FINISHED) {
		// The execution didn't complete as expected. Determine what happened.
		if(r == asEXECUTION_EXCEPTION) {
			// An exception occurred, let the script writer know what happened so it can be corrected.
			printf("An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
		}
	}
}

asIScriptEngine *engine = NULL;
asIScriptContext *ctx = NULL;

ImConsole *ImConsole_Factory() {
	static std::string name = "Console #";
	static int count = 0;
	++count;
	return new ImConsole(name + std::to_string(count), engine, ctx, "Engine");
}

int main() {
	sgl::window window = window_create();

	sgl::shader asteroid_shader("assets/instance_vert.glsl", "assets/instance_frag.glsl");
	sgl::texture asteroid_tex("assets/asteroid.png");
	sgl::mesh asteroid_mesh("assets/asteroid.obj");
	
	std::vector<glm::mat4> asteroids;
	std::vector<glm::vec3> asteroid_colors;
	for (size_t i = 0; i < 10; ++i) {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f * i));
		asteroids.push_back(model);
		asteroid_colors.push_back(glm::vec3(0.9f, 0.3f, 0.4f));
	}
	
	size_t indexModel = asteroid_mesh.add_instance_buffer({4, 4, 4, 4}, true);
	size_t indexColor = asteroid_mesh.add_instance_buffer({3});
	
	/* angelscript */
	
	engine = asCreateScriptEngine();
	int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	assert(r >= 0);
	
	RegisterStdString(engine);
	RegisterScriptArray(engine, true);
	
	r = engine->RegisterObjectType("Console", 0, asOBJ_REF);
	assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Console", asBEHAVE_ADDREF, "void f()", asMETHOD(ImConsole, AddRef), asCALL_THISCALL);
	assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Console", asBEHAVE_RELEASE, "void f()", asMETHOD(ImConsole, Release), asCALL_THISCALL);
	assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Console", asBEHAVE_FACTORY, "Console@ f()", asFUNCTION(ImConsole_Factory), asCALL_CDECL);
	assert(r >= 0);
	r = engine->RegisterObjectMethod("Console", "void log(const string &in)", asMETHOD(ImConsole, log), asCALL_THISCALL);
	assert(r >= 0);
	r = engine->RegisterObjectMethod("Console", "void render()", asMETHOD(ImConsole, render), asCALL_THISCALL);
	assert(r >= 0);
	
	CScriptBuilder builder;
	r = builder.StartNewModule(engine, "Engine");
	assert(r >= 0);

	r = builder.AddSectionFromFile("assets/scripts/test.as");
	assert(r >= 0);

	r = builder.BuildModule();
	assert(r >= 0);

	
	asIScriptModule *mod = engine->GetModule("Engine");
	asIScriptFunction *on_load_script = mod->GetFunctionByDecl("void on_load()");
	asIScriptFunction *on_render_script = mod->GetFunctionByDecl("void on_render()");
	assert(on_load_script != NULL);
	assert(on_render_script != NULL);


	ctx = engine->CreateContext();
	ctx->Prepare(on_load_script);
	r = ctx->Execute();
	ctx->Unprepare();
	ExecuteError(ctx, r);

	/* ----------- */
	ImGui::StyleColorsLight();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	
	glEnable(GL_DEPTH_TEST);
	window.on_update([&](sgl::window &) {
		/* ImGui */

		ctx->Prepare(on_render_script);	
		ctx->Execute();
		ExecuteError(ctx, r);
		ctx->Unprepare();

		/* Rendering */
		glClearColor(0.3f, 0.6f, 0.75f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		asteroid_shader["mProjection"] = glm::perspective(glm::radians(45.0f), (float)window.width / (float)window.height, 0.1f, 1000.0f);
		asteroid_shader["mView"] = glm::lookAt(
			glm::vec3(-3.5f, 4.5f, -8.0f),
			glm::vec3(1.0f, 0.5f, 4.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		
		asteroid_mesh.update_instance_buffer(indexModel, &asteroids[0], asteroids.size());
		asteroid_mesh.update_instance_buffer(indexColor, &asteroid_colors[0], asteroid_colors.size());

		asteroid_shader.use();
		asteroid_tex.bind(0);
		asteroid_mesh.render_instanced(asteroids.size());
	});
	
	ctx->Release();
	engine->ShutDownAndRelease();

	return 0;
}

