#include <iostream>

#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <imgui/imgui.h>

#include "sgl_window.hpp"
#include "sgl_shader.hpp"
#include "sgl_mesh.hpp"
#include "sgl_texture.hpp"
#include "sgl_joystick.hpp"
#include "sgl_camera.hpp"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char *argv[]) {
#endif
	
	sgl::window window(800, 600);
	window.on_resize([](sgl::window &, int w, int h){ glViewport(0, 0, w, h); });

	sgl::joystick controller(0);
	
	/* matrices */
	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

	sgl::camera camera(glm::vec3(0.0f, 1.5f, 0.0f));
	glm::mat4 projection;
	glm::mat4 MVP;
	
	/* shader */
	sgl::shader tshader("assets/vert.glsl", "assets/frag.glsl");
	GLint uMVP = tshader["MVP"];
	
	/* model */
	sgl::mesh world_mesh("assets/world.obj");
	sgl::texture world_tex("assets/world.png");
	tshader["teximage"] = 0;
	
	/* opengl */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	ImGui::StyleColorsLight();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	
	window.update([&](sgl::window &) {
		/* inputs */
		ImGui::Begin("Controller #0");
			ImGui::Columns(2);
			ImGui::Text("Axes:");
			for (int i = 0; i < controller.get_axes_count(); ++i) {
				float v = controller.get_axis(i);
				ImGui::TextColored(ImVec4(1.0 - (v * 0.5 + 0.5), 0.0, v * 0.5 + 0.5, 1.0), "%d: %g\n", i, v);
			}
			ImGui::NextColumn();
			ImGui::Text("Buttons:");
			for (int i = 0; i < controller.get_button_count(); ++i) {
				ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "%d: %d\n", i, controller.get_button(i));
			}
		ImGui::End();
		
		ImGui::Begin("Render Settings");
			static bool _wireframe_enabled = false;
			static bool _render_depthbuffer = false;
			if (ImGui::Checkbox("Wireframe", &_wireframe_enabled)) {
				window.render_wireframe(_wireframe_enabled);
			}
			if (ImGui::Checkbox("Depthbuffer", &_render_depthbuffer)) {
				tshader["render_depthbuffer"] = _render_depthbuffer;
			}
		ImGui::End();
		
		/* recalculate matrices */
		float c_l2 = (controller.get_axis(2) * 0.5 + 0.5) * -1.0f;
		float c_r2 = (controller.get_axis(5) * 0.5 + 0.5) *  1.0f;
		camera.move(glm::vec3(controller.get_axis(0), c_l2 + c_r2, controller.get_axis(1) * -1.0f));
		camera.rotate(controller.get_axis(3) * 1.25f, controller.get_axis(4) * -1.0f, 1.75f);
		projection = glm::perspective(45.0f, (float)window.width / (float)window.height, 0.1f, 350.0f);
		MVP = projection * camera.get_view() * model;
		
		/* update shader uniform mvp */
		tshader["viewport"] = glm::vec2(window.width, window.height);
		tshader["projection"] = projection;
		tshader["model"] = model;
		tshader[uMVP] = MVP;
		
		/* render code */
		glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(tshader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, world_tex);
		world_mesh.render();
		glUseProgram(0);
		
	});
	
	return 0;
}

