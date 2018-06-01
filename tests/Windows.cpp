#include <vector>
#include <iostream>

#include <string.h>
#include <stdio.h>

#include <GL/gl3w.h>
#include "simgl.hpp"

#include <glm/glm.hpp>

#include <nuklear/nuklear.h>
#include <imgui/imgui.h>

ImVec2 ImVec2_Sub(const ImVec2 &o, float v) {
	return ImVec2(o.x - v, o.y - v);
}

ImVec2 ImVec2_Sub(const ImVec2 &o, const ImVec2 &v) {
	return ImVec2(o.x - v.x, o.y - v.y);
}

void load_file(const char *path, char *str) {
	FILE *fp = fopen(path, "r");
	if (!fp) return;

	fseek(fp, 0, SEEK_END);
	const long len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	char v[len + 1];
	fread(v, sizeof(char), len, fp);

	strncpy(str, v, len);

	fclose(fp);
}

struct model {
	sgl::mesh *mesh;
	sgl::texture *texture, *normals, *specular;
	sgl::shader *shader;
	
	model(sgl::mesh *m, sgl::texture *t, sgl::shader *s) {
		this->mesh = m;
		this->texture = t;
		this->shader = s;
		this->normals = nullptr;
		this->specular = nullptr;
	}

	void render(glm::mat4 &m, glm::mat4 &v, glm::mat4 &p) {
		shader->use();
		texture->bind(0);
		if (this->normals) normals->bind(1);
		if (this->specular) specular->bind(2);
		(*shader)["uDiffuse"] = 0;
		(*shader)["uNormal"] = 1;
		(*shader)["uSpecular"] = 2;
		(*shader)["uProjection"] = p;
		(*shader)["uView"] = v;
		(*shader)["uModel"] = m;
		mesh->render();
	}
};

struct model_window {
	model md;
	sgl::framebuffer *fb;
	static int c;
	int i;
	ImVec2 size;
	float scale;
	glm::vec3 rot, trans;
	sgl::shader *s;

	model_window(model m, sgl::window &w)
		: md{m}
	{
		this->fb = new sgl::framebuffer(w.width, w.height);
		this->i = model_window::c++;
		this->size = ImVec2(1.0f, 1.0f);
		this->scale = 1.0f;
		this->rot = glm::vec3(20.0f, 0.0f, 0.0f);
		this->trans = glm::vec3(0.0f, -1.0f, 0.0f);
		this->s = nullptr;
	}
	~model_window() {
		delete this->fb;
		delete this->s;
	}

	void render(glm::mat4 &v) {
		glm::mat4 p = glm::perspective(glm::radians(45.0f), size.x / size.y, 0.1f, 100.0f);

		glm::mat4 m(1.0f);
		m = glm::scale(m, glm::vec3(this->scale));
		m = glm::rotate(m, glm::radians(this->rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m = glm::rotate(m, glm::radians(this->rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m = glm::translate(m, this->trans);
			
		char n[128];
		static char shader_text_v[4096 * 2],
		            shader_text_f[4096 * 2];
		sprintf(n, "Model##%u", this->i);
		ImGui::Begin(n);
			this->size = ImVec2_Sub(ImGui::GetWindowSize(), ImVec2(20.0f, 64.0f));
			ImGui::Image((void *)((GLuint)(*fb)), size);
			
			if (ImGui::TreeNode("Transform")) {
				ImGui::SliderFloat("Scale", &this->scale, 0.2f, 5.0f);
				ImGui::SliderFloat2("Rotation", &this->rot[0], -90.0f, 90.0f);
				ImGui::SliderFloat3("Translation", &this->trans[0], -2.0f, 2.0f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Shader")) {
				ImGui::InputTextMultiline("Vertex", shader_text_v, sizeof(shader_text_v) / sizeof(shader_text_v[0]));
				ImGui::InputTextMultiline("Fragment", shader_text_f, sizeof(shader_text_f) / sizeof(shader_text_f[0]));
				if (ImGui::Button("Compile")) {
					if (this->s != nullptr)
						delete this->s;
					this->s = new sgl::shader;
					s->load_from_memory(shader_text_v, sgl::shader::VERTEX);
					s->load_from_memory(shader_text_f, sgl::shader::FRAGMENT);
					s->compile();
					s->link();
					this->md.shader = s;
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset to Default")) {
					load_file("assets/default_vert.glsl", shader_text_v);
					load_file("assets/default_frag.glsl", shader_text_f);
				}

				ImGui::TreePop();
			}

		ImGui::End();
		
		fb->bind();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			md.render(m, v, p);
		fb->unbind();
	}
};
int model_window::c = 0;

int main(int argc, char *argv[]) {
	
	sgl::window_context wctx;
	wctx.dear_imgui = true;
	wctx.resizable = false;
	wctx.width = 740;
	wctx.height = 610;
	sgl::window window(wctx);

	window.on_resize([](sgl::window &window, int w, int h) { glViewport(0, 0, w, h); });
	
	sgl::framebuffer fb(window);
	
	sgl::shader s("assets/default_vert.glsl", "assets/default_frag.glsl");
	
	std::vector<sgl::mesh *> meshes;
	std::vector<sgl::texture *> textures;
	std::vector<model_window *> windows;

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	window.on_update([&](sgl::window &) {
		glClearColor(0.33f, 0.66f, 0.99f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		static glm::mat4 uView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
		
		static char in_model[256],
		            in_textr[256],
					in_norml[256],
					in_specl[256];
		static bool _t = true;
		if (_t) {
			strcpy(in_model, "assets/well.obj");
			strcpy(in_textr, "assets/well.jpg");
			strcpy(in_norml, "assets/well_normal.jpg");
			strcpy(in_specl, "assets/well_specular.jpg");
			_t = false;
		}

		ImGui::Begin("Add Model");
			ImGui::InputText("Model", in_model, sizeof(in_model) / sizeof(*in_model));
			ImGui::InputText("Texture", in_textr, sizeof(in_textr) / sizeof(*in_textr));
			ImGui::InputText("Normal", in_norml, sizeof(in_norml) / sizeof(*in_norml));
			ImGui::InputText("Specular", in_specl, sizeof(in_specl) / sizeof(*in_specl));

			if (ImGui::Button("Load")) {
				meshes.push_back(new sgl::mesh(in_model));
				size_t begin = textures.size();
				textures.push_back(new sgl::texture(in_textr));
				textures.push_back(new sgl::texture(in_norml));
				textures.push_back(new sgl::texture(in_specl));

				model md = model(meshes.back(), textures.at(begin + 0), &s);
				md.normals = textures.at(begin + 1);
				md.specular = textures.at(begin + 2);

				model_window *mwd = new model_window(md, window);
				windows.push_back(mwd);
			}
		ImGui::End();

		for (auto &w : windows) {
			w->render(uView);
		}
		
	});
		
	for (model_window *w : windows) {
		delete w;
	}
	for (sgl::texture *tex : textures) {
		delete tex;
	}
	for (sgl::mesh *me : meshes) {
		delete me;
	}

	return 0;
}

