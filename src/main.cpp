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
	wctx.samples = 8;
	wctx.width = 740;
	wctx.height = 610;
	sgl::window window(wctx);

	window.on_resize([](sgl::window &window, int w, int h) { glViewport(0, 0, w, h); });
	
	sgl::framebuffer fb(window);
	
	sgl::mesh m("assets/well.obj", {3, 3, 2});
	sgl::shader s("assets/default_vert.glsl", "assets/default_frag.glsl");
	sgl::texture t("assets/well.jpg");
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	window.on_update([&](sgl::window &) {
		glClearColor(0.33f, 0.66f, 0.99f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 uProjection = glm::perspective(glm::radians(45.0f), window.aspect(), 0.1f, 1000.0f);
		static glm::mat4 uView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
		static glm::mat4 uModel = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(0.75f)), glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.0f, -1.0f, 0.0f));
		static float bg[] = {0.3f, 0.3f, 0.3f, 1.0f};

		uModel = glm::rotate(uModel, glm::radians(2.75f), glm::vec3(0.0f, 1.0f, 0.0f));

		fb.bind();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glClearColor(bg[0], bg[1], bg[2], bg[3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
		fb.unbind();


		s.use();
		t.bind(0);
		s["uProjection"] = uProjection;
		s["uView"] = uView;
		s["uModel"] = uModel;
		m.render();

		ImGui::Begin("Colors");
			ImGui::ColorEdit4("glClearColor", bg);
			ImGui::Image((void *)((GLuint)fb), ImVec2_Sub(ImGui::GetWindowSize(), ImVec2(20.0f, 64.0f)));
		ImGui::End();


	});

	return 0;
}

