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

int main(int argc, char *argv[]) {
	
	sgl::window_context wctx;
	wctx.dear_imgui = true;
	wctx.resizable = false;
	wctx.samples = 16;
	wctx.width = wctx.height = 740;
	sgl::window window(wctx);

	window.on_resize([](sgl::window &window, int w, int h) { glViewport(0, 0, w, h); });
	
	sgl::framebuffer fb(window);
	
	sgl::mesh m(std::vector<GLuint>{3});
	m.add_vertex({-1.0f, -1.0f,  1.0f});
	m.add_vertex({ 1.0f, -1.0f,  1.0f});
	m.add_vertex({-1.0f,  1.0f,  1.0f});
	m.add_vertex({ 1.0f,  1.0f,  1.0f});

	m.add_vertex({-1.0f, -1.0f, -1.0f});
	m.add_vertex({ 1.0f, -1.0f, -1.0f});
	m.add_vertex({-1.0f,  1.0f, -1.0f});
	m.add_vertex({ 1.0f,  1.0f, -1.0f});

	m.set_indices({
		0, 1, 2,	2, 1, 3,
		1, 5, 3,	3, 5, 7,
		0, 5, 1,	0, 4, 5,
		2, 7, 6,	2, 3, 7,
		2, 4, 0,	2, 6, 4,
		7, 4, 6,	7, 5, 4
	});
	m.load();
	
	sgl::shader s;
	s.load_from_memory(R"(
		#version 330 core

		uniform mat4 uModel;	

		layout (location = 0) in vec3 vPos;

		out vec3 fPos;

		void main() {
			fPos = vPos;
			gl_Position = uModel * vec4(vPos, 1.0);
		}
	)", sgl::shader::VERTEX);
	
	s.load_from_memory(R"(
		#version 330 core
		
		in vec3 fPos;

		out vec4 FragColor;

		void main() {
			FragColor = vec4(fPos.xyz * 0.5 + 0.5, 1.0);
		}
	)", sgl::shader::FRAGMENT);
	s.compile();
	s.link();

	glEnable(GL_DEPTH_TEST);
	window.on_update([&](sgl::window &) {
		glClearColor(0.33f, 0.66f, 0.99f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		fb.bind();
			static glm::mat4 uModel = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
			uModel = glm::rotate(uModel, glm::radians(2.75f), glm::vec3(0.0f, 0.5f, 1.0f));

			static float bg[] = {0.3f, 0.3f, 0.3f, 0.0f};
			glEnable(GL_BLEND);
			
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glClearColor(bg[0], bg[1], bg[2], bg[3]);
			glClear(GL_COLOR_BUFFER_BIT);

			s.use();
			s["uModel"] = uModel;
			glClear(GL_DEPTH_BUFFER_BIT);
			m.render_indexed();

		fb.unbind();
		
		ImGui::Begin("Colors");
			ImGui::ColorEdit4("glClearColor", bg);
			ImGui::Image((void *)((GLuint)fb), ImVec2_Sub(ImGui::GetWindowSize(), ImVec2(20.0f, 64.0f)));
		ImGui::End();
	});

	return 0;
}

