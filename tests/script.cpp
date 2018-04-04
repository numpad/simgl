#include <vector>
#include <string>

#include <stdio.h>

#include "sgl_window.hpp"
#include "sgl_window_context.hpp"
#include "sgl_camera.hpp"
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
#include <glm/gtc/type_ptr.hpp>

std::string ASErrorMessage;

asIScriptEngine *engine = NULL;
asIScriptContext *ctx = NULL;


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
	
	void set_color(float r, float g, float b) {
		this->color = ImVec4(r, g, b, 1.0f);
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

	void handle_command(std::string cmd, int &line_num) {
		int r;
		if (cmd[0] == '.') {
			r = this->script_module->CompileGlobalVar("DynamicVar", cmd.c_str() + 1, line_num);
			this->print(ConsoleLog(cmd.substr(1, cmd.size()), ConsoleLog::Level::WARN));
		} else {
			this->print(ConsoleLog(cmd, ConsoleLog::Level::WARN));
			r = ExecuteString(this->script_engine, cmd.c_str(), script_module, script_ctx);
		}
		
		if (r < 0) {
			this->print(ASErrorMessage, ConsoleLog::Level::ERR);
		}
		
		command_input[0] = '\0';
	}
public:

	ImConsole(std::string title, asIScriptEngine *engine, std::string modname, asIScriptContext *ctx = nullptr) {
		this->_refcount = 1;
		this->title = title;
		this->childname = title + "_child";

		script_engine = engine;
		script_module = engine->GetModule(modname.c_str());
		
		if (!ctx) {
			ctx = engine->CreateContext();
		}
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
				static int line_num = 1;
				this->handle_command(std::string(command_input), line_num);
				++line_num;
			}
		ImGui::End();
	}
};


ImConsole *ImConsole_CreateConsole() {
	static std::string name = "Console #";
	static int count = 0;
	++count;
	return new ImConsole(name + std::to_string(count), engine, "Engine", ctx);
}
	

void ImConsole_register_context(asIScriptEngine *script_engine) {
	script_engine->RegisterObjectType("Console", 0, asOBJ_REF);
	script_engine->RegisterObjectBehaviour("Console", asBEHAVE_FACTORY, "Console@ f()", asFUNCTION(ImConsole_CreateConsole), asCALL_CDECL);
	script_engine->RegisterObjectBehaviour("Console", asBEHAVE_ADDREF, "void f()", asMETHOD(ImConsole, AddRef), asCALL_THISCALL);
	script_engine->RegisterObjectBehaviour("Console", asBEHAVE_RELEASE, "void f()", asMETHOD(ImConsole, Release), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Console", "void log(const string &in)", asMETHOD(ImConsole, log), asCALL_THISCALL);
	script_engine->RegisterObjectMethod("Console", "void render()", asMETHOD(ImConsole, render), asCALL_THISCALL);
}

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

class CVec3 : public glm::vec3 {
	int _refcount;

public:
	
	CVec3(glm::vec3 v) : glm::vec3(v.x, v.y, v.z) {
		_refcount = 1;
	}

	CVec3(float x, float y, float z) : glm::vec3(x, y, z) {
		_refcount = 1;
	}

	float length() {
		return glm::length((glm::vec3)(*this));
	}
	
	CVec3 *normal() {
		return new CVec3(glm::normalize((glm::vec3)(*this)));
	}
	
	void set(CVec3 *other) {
		this->x = other->x;
		this->y = other->y;
		this->z = other->z;
	}

	CVec3 *add(CVec3 *other) {
		return new CVec3(
			this->x + other->x,
			this->y + other->y,
			this->z + other->z
		);
	}

	void AddRef() {
		++_refcount;
	}
	void Release() {
		if (--_refcount == 0)
			delete this;
	}
};

class CCamera : public sgl::camera {
	int _refcount;

public:
	
	CCamera(CVec3 pos) : sgl::camera(glm::vec3(pos.x, pos.y, pos.z))
	{
		_refcount = 1;
	}
	
	void lookAt(CVec3 *target) {
		std::cout << "x: " << target->x << ", y: " << target->y << ", z: " << target->z << std::endl;
	}
	
	void move_relative(CVec3 *v) {
		this->move(glm::vec3(v->x, v->y, v->z), 1.0f);
	}

	void AddRef() {
		++_refcount;
	}
	void Release() {
		if (--_refcount == 0)
			delete this;
	}
};

CVec3 *CVec3_Factory(float x, float y, float z) {
	return new CVec3(x, y, z);
}
CVec3 *CVec3_Factory(float a) {
	return CVec3_Factory(a, a, a);
}
CVec3 *CVec3_Factory() {
	return CVec3_Factory(0.0f);
}

CCamera *CCamera_Factory(float x, float y, float z) {
	return new CCamera(glm::vec3(x, y, z));
}

void RegisterASVec3() {
	engine->RegisterObjectType("Vec3", 0, asOBJ_REF);
	engine->RegisterObjectBehaviour(
		"Vec3",
		asBEHAVE_FACTORY,
		"Vec3@ f(float x, float y, float z)",
		asFUNCTIONPR(CVec3_Factory, (float, float, float), CVec3 *),
		asCALL_CDECL
	);
	engine->RegisterObjectBehaviour(
		"Vec3",
		asBEHAVE_FACTORY,
		"Vec3@ f(float a)",
		asFUNCTIONPR(CVec3_Factory, (float), CVec3 *),
		asCALL_CDECL
	);
	engine->RegisterObjectBehaviour(
		"Vec3",
		asBEHAVE_FACTORY,
		"Vec3@ f()",
		asFUNCTIONPR(CVec3_Factory, (void), CVec3 *),
		asCALL_CDECL
	);
	engine->RegisterObjectBehaviour(
		"Vec3",
		asBEHAVE_ADDREF,
		"void f()",
		asMETHOD(CVec3, AddRef),
		asCALL_THISCALL
	);
	engine->RegisterObjectBehaviour(
		"Vec3",
		asBEHAVE_RELEASE,
		"void f()",
		asMETHOD(CVec3, Release),
		asCALL_THISCALL
	);
	engine->RegisterObjectMethod(
		"Vec3",
		"void opAssign(Vec3@ other)",
		asMETHOD(CVec3, set),
		asCALL_THISCALL
	);
	engine->RegisterObjectMethod(
		"Vec3",
		"float length()",
		asMETHOD(CVec3, length),
		asCALL_THISCALL
	);
	engine->RegisterObjectMethod(
		"Vec3",
		"Vec3@ normal()",
		asMETHOD(CVec3, normal),
		asCALL_THISCALL
	);

	engine->RegisterObjectProperty(
		"Vec3", "float x", asOFFSET(CVec3, x)
	);
	engine->RegisterObjectProperty(
		"Vec3", "float y", asOFFSET(CVec3, y)
	);
	engine->RegisterObjectProperty(
		"Vec3", "float z", asOFFSET(CVec3, z)
	);
}

void RegisterASCamera() {
	engine->RegisterObjectType("Camera", 0, asOBJ_REF);
	engine->RegisterObjectBehaviour("Camera", asBEHAVE_FACTORY, "Camera@ f(float, float, float)", asFUNCTION(CCamera_Factory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Camera", asBEHAVE_ADDREF, "void f()", asMETHOD(CCamera, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("Camera", asBEHAVE_RELEASE, "void f()", asMETHOD(CCamera, Release), asCALL_THISCALL);
	engine->RegisterObjectProperty(
		"Camera",
		"Vec3 pos",
		asOFFSET(CCamera, pos)
	);
	engine->RegisterObjectMethod(
		"Camera",
		"float get_pitch()",
		asMETHOD(CCamera, get_pitch),
		asCALL_THISCALL
	);
	engine->RegisterObjectMethod(
		"Camera",
		"float get_yaw()",
		asMETHOD(CCamera, get_yaw),
		asCALL_THISCALL
	);
	engine->RegisterObjectMethod(
		"Camera",
		"void rotate(float, float, float sensitivity = 1.0f)",
		asMETHOD(CCamera, rotate),
		asCALL_THISCALL
	);
	engine->RegisterObjectMethod(
		"Camera",
		"void set_rotation(float, float)",
		asMETHOD(CCamera, rotate),
		asCALL_THISCALL
	);
	engine->RegisterObjectMethod(
		"Camera",
		"void move(Vec3@)",
		asMETHOD(CCamera, move_relative),
		asCALL_THISCALL
	);
}

void imgui_showmat4(const char *name, glm::mat4 m) {
	ImGui::Begin(name);
		for (int i = 0; i < 4; ++i) {
			char label[32] = {0};
			sprintf(label, "Row %d##%s", i, name);
			ImGui::InputFloat4(label, (float *)glm::value_ptr(glm::transpose(m)) + (i * 4), 2);
		}
	ImGui::End();
}

std::vector<char *> camera_names;
std::vector<CCamera *> cameras;

void RegisterCamera(std::string name, CCamera *cam) {
	char *cname = new char[name.size() + 1]();
	strcpy(cname, name.c_str());

	camera_names.push_back(cname);
	cameras.push_back(cam);
}

void UnregisterCameras() {
	for (char *name : camera_names) {
		free(name);
	}
}

int main() {
	sgl::window window = window_create();
	
	sgl::shader asteroid_shader("assets/instance_vert.glsl", "assets/instance_frag.glsl");
	sgl::texture asteroid_tex("assets/can_texture.png");
	sgl::mesh asteroid_mesh("assets/can_mesh.obj");
	
	std::vector<glm::mat4> asteroids;
	std::vector<glm::vec3> asteroid_colors;
	for (size_t i = 0; i < 10; ++i) {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f * i));
		model = glm::scale(model, glm::vec3(0.1f));
		asteroids.push_back(model);
		asteroid_colors.push_back(glm::vec3(0.9f, 0.3f, 0.4f));
	}
	
	size_t indexModel = asteroid_mesh.add_instance_buffer({4, 4, 4, 4}, true);
	size_t indexColor = asteroid_mesh.add_instance_buffer({3});
	
	sgl::mesh mesh(std::vector<GLuint>{3, 3});
	mesh.add_vertex({-0.5f,  0.0f,  0.0f, 	1.0f, 0.0f, 0.0f});
	mesh.add_vertex({ 0.0f,  0.5f,  0.0f, 	1.0f, 1.0f, 1.0f});
	mesh.add_vertex({ 0.5f,  0.0f,  0.0f, 	1.0f, 0.0f, 0.0f});
	mesh.load();
	
	sgl::shader shader;
	shader.load_from_memory(
		R"(
			#version 330 core

			uniform mat4 mProjection;
			uniform mat4 mView;
			uniform mat4 mModel;

			uniform float uColor;

			layout (location = 0) in vec3 VertPos;
			layout (location = 1) in vec3 VertColor;
			
			out vec4 FragColor;

			void main() {
				vec3 pixel = VertColor;
				if (uColor > 0.5) {
					pixel.r = 1.0 - VertColor.r;
					pixel.g = 1.0 - VertColor.g;
					pixel.b = 1.0 - VertColor.b;
				}

				gl_Position = mProjection * mView * mModel * vec4(VertPos, 1.0);
				FragColor = vec4(pixel, 1.0);
			}
		)"
	, sgl::shader::VERTEX);
	
	shader.load_from_memory(
		R"(
			#version 330 core
			
			in vec4 FragColor;
			out vec4 Color;

			void main() {
				Color = FragColor;
			}
		)"
	, sgl::shader::FRAGMENT);
	
	shader.compile(sgl::shader::VERTEX);
	shader.compile(sgl::shader::FRAGMENT);
	shader.link();

	/* angelscript */
	engine = asCreateScriptEngine();
	int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	assert(r >= 0);
	
	RegisterStdString(engine);
	RegisterScriptArray(engine, true);
	
	RegisterASVec3();
	RegisterASCamera();
	ImConsole_register_context(engine);

	CCamera *script_camera = new CCamera(CVec3(0.0f, 0.5f, -2.0f));
	script_camera->set_rotation(90.0f, 0.0f);
	
	engine->RegisterGlobalProperty("Camera camera", script_camera);

	engine->RegisterGlobalFunction("void RegisterCamera(string, Camera@)", asFUNCTION(RegisterCamera), asCALL_CDECL);

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
	ExecuteError(ctx, r);
	ctx->Unprepare();
	
	RegisterCamera("Default", script_camera);

	/* ----------- */
	ImGui::StyleColorsLight();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	
	glEnable(GL_DEPTH_TEST);
	window.on_update([&](sgl::window &) {
		/* ImGui */
		static int camstate_current = 0;
		static CCamera *cam_current = cameras.at(camstate_current);
		static float field_of_view = 50.0f;
		
		ImGui::Begin("Camera");
			if (ImGui::Combo("View Matrix", &camstate_current, &camera_names[0], camera_names.size()))
				cam_current = cameras.at(camstate_current);
			ImGui::SliderFloat("FoV", &field_of_view, 1.0f, 175.0f);
			ImGui::DragFloat3("Position", &cam_current->pos[0], 0.01f);
			float rot[2] = {cam_current->get_yaw(), cam_current->get_pitch()};
			ImGui::DragFloat2("Rotation", rot, 0.5f);
			cam_current->set_rotation(rot[0], rot[1]);
		ImGui::End();
		
		/* script */
		ctx->Prepare(on_render_script);	
		ctx->Execute();
		ExecuteError(ctx, r);
		ctx->Unprepare();
		
		/* update */
		const float speed_gain = 1.5f;
		int i = 0;
		for (auto &m : asteroids) {
			m = glm::translate(m, glm::vec3(0.0f, sinf(glfwGetTime() * 1.25f + i * 0.45f) * 0.04f, 0.0f));
			m = glm::rotate(m, glm::radians((i + 1) * 0.25f * speed_gain), glm::vec3(0.0f, 1.0f, 0.0f));
			++i;
		}

		/* Rendering */
		glClearColor(0.3f, 0.6f, 0.75f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 mProjection = glm::infinitePerspective(glm::radians(field_of_view), (float)window.width / (float)window.height, 0.1f);
		glm::mat4 mView = cameras.at(camstate_current)->get_view();
		
		asteroid_shader["mProjection"] = mProjection;
		asteroid_shader["mView"] = mView;
		
		asteroid_mesh.update_instance_buffer(indexModel, &asteroids[0], asteroids.size());
		asteroid_mesh.update_instance_buffer(indexColor, &asteroid_colors[0], asteroid_colors.size());

		asteroid_shader.use();
		asteroid_tex.bind(0);
		asteroid_mesh.render_instanced(asteroids.size());

		static glm::mat4 mod(1.0f);
		mod = glm::rotate(mod, glm::radians(6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		
		glEnable(GL_CULL_FACE);
		shader["mModel"] = mod;
		shader["mView"] = mView;
		shader["mProjection"] = mProjection;
		
		glCullFace(GL_FRONT);
		shader["uColor"] = 1.0f;
		shader.use();
		mesh.render();
		
		glCullFace(GL_BACK);
		shader["uColor"] = 0.0f;
		shader.use();
		mesh.render();
	});
	
	UnregisterCameras();
	delete script_camera;
	ctx->Release();
	engine->ShutDownAndRelease();

	return 0;
}

