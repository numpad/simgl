#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "sgl_window.hpp"
#include "sgl_mesh.hpp"
#include "sgl_shader.hpp"

sgl::shader load_raymarch() {
	sgl::shader raym;
	
	raym.load("assets/raym.glsl", sgl::shader::FRAGMENT);
	raym.load_from_memory(R"(
		#version 330 core
		
		uniform vec2 uResolution;
		layout(location = 0) in vec3 Position;
		
		void main() {
			gl_Position = vec4(Position, 1.0f);
		}
	)", sgl::shader::VERTEX);
	
	raym.compile();
	raym.link();

	return raym;
}

sgl::mesh load_rect() {
	sgl::mesh rect(std::vector<GLuint>{2});

	rect.add_vertex({-1.0f, -1.0f});
	rect.add_vertex({ 1.0f, -1.0f});
	rect.add_vertex({-1.0f,  1.0f});
	rect.add_vertex({ 1.0f,  1.0f});
	rect.set_indices({0, 1, 2, 2, 1, 3});
	rect.load();

	return rect;
}

int main() {
	
	sgl::window_context wctx;
	wctx.width = 800;
	wctx.height = 600;
	wctx.dear_imgui = true;
	sgl::window window(wctx);
	
	sgl::mesh screen_rect = load_rect();
	sgl::shader shader = load_raymarch();
	sgl::shader shader2;
	
	window.on_resize([](sgl::window &, int w, int h) { glViewport(0, 0, w, h); });
	
	window.on_update([&](sgl::window &) {
		glClear(GL_COLOR_BUFFER_BIT);
		
		static glm::vec3 camera(0, 5, 0);
		static float speed = 0.1;
		if (sgl::input::get_key(GLFW_KEY_W))
			camera.z += speed;
		if (sgl::input::get_key(GLFW_KEY_S))
			camera.z -= speed;
		if (sgl::input::get_key(GLFW_KEY_A))
			camera.x -= speed;
		if (sgl::input::get_key(GLFW_KEY_D))
			camera.x += speed;
		if (sgl::input::get_key(GLFW_KEY_SPACE))
			camera.y += speed;
		if (sgl::input::get_key(GLFW_KEY_LEFT_SHIFT))
			camera.y -= speed;
		
		/* uniforms */
		static float time = 0, timestep = 0.1f;
		shader["uCamera"] = camera;
		shader["uTime"] = (time += timestep);
		shader["uResolution"] = glm::vec2(window.width, window.height);
		
		if (ImGui::Begin("Uniforms")) {
			ImGui::SliderFloat("delta t", &timestep, 0.0f, 3.0f);
			ImGui::SameLine();
			if (ImGui::Button("Stop")) {
				timestep = 0;
			}
			ImGui::SliderFloat3("eye", &camera[0], -10, 10);
			if (ImGui::Button("Reload Shader")) {
				
			}
			ImGui::Text("%d rays/frame.", window.width * window.height);
			
		}
		ImGui::End();
		
		shader.use();
		screen_rect.render_indexed();
	});

	return 0;
}

