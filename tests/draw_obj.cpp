#include <stdlib.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>

#include "sgl_window.hpp"
#include "sgl_mesh.hpp"
#include "sgl_texture.hpp"
#include "sgl_shader.hpp"

void usage_and_exit(const char *arg0) {
	std::cout << "usage:" << std::endl << " " << arg0 << " <obj-file> <texture-file>" << std::endl;
	exit(0);
}

int main(int argc, char *argv[]) {
	if (argc < 3) usage_and_exit(argv[0]);
	
	/* create window */
	sgl::window_context wctx;
	wctx.resizable = true;
	wctx.dear_imgui = true;
	sgl::window window(wctx);
	
	glm::mat4 matrix_projection = glm::perspective(glm::radians(45.0f), (float)window.width / (float)window.height, 0.1f, 500.0f);
	window.on_resize([&matrix_projection](sgl::window &, int width, int height) {
		glViewport(0, 0, width, height);
		matrix_projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 500.0f);
	});
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	std::string model_mesh_path = argv[1];
	std::string model_texture_path = argv[2];

	sgl::mesh model_mesh(model_mesh_path);
	sgl::texture model_texture(model_texture_path);
	sgl::shader model_shader;
	
	model_shader.load_from_memory(
		"#version 330 core\n"
		
		"uniform mat4 mModel;\n"
		"uniform mat4 mView;\n"
		"uniform mat4 mProjection;\n"
		"uniform vec3 pLightPos;\n"

		"layout (location = 0) in vec3 Position;\n"
		"layout (location = 1) in vec3 Normal;\n"
		"layout (location = 2) in vec2 TexCoord;\n"

		"out vec2 tex_coord;\n"
		"out float illumination;\n"

		"void main() {\n"
		"  vec3 LightDir = vec3(0.0) - pLightPos;\n"
		"  vec3 ModelNormal = normalize(transpose(inverse(mat3(mModel))) * Normal);\n"
		"  illumination = clamp(dot(ModelNormal, LightDir), 0.165, 0.765);\n"
		"  tex_coord = TexCoord;\n"
		"  gl_Position = mProjection * mView * mModel * vec4(Position, 1.0);\n"
		"}\n"
	, sgl::shader::VERTEX);

	model_shader.load_from_memory(
		"#version 330 core\n"

		"uniform sampler2D tex_image;\n"

		"in vec2 tex_coord;\n"
		"in float illumination;\n"

		"out vec4 frag_color;\n"

		"void main() {\n"
		"  vec3 pixel_color = texture(tex_image, tex_coord).rgb;\n"
		"  frag_color = vec4(pixel_color * illumination, 1.0);\n"
		"}\n"
	, sgl::shader::FRAGMENT);
	
	model_shader.compile(sgl::shader::VERTEX);
	model_shader.compile(sgl::shader::FRAGMENT);
	model_shader.link();
	
	model_shader["tex_image"] = 0;
	model_shader["pLightPos"] = glm::vec3(0.0f, -1.0f, -2.0f);
	ImGui::StyleColorsLight();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	window.on_update([&](sgl::window &) {
		/* settings */
		static glm::vec3 clear_color = glm::vec3(0.1f, 0.2f, 0.3f);
		static glm::vec3 camera_pos = glm::vec3(0.0f, 1.0f, -2.0f);
		static glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
		static glm::vec3 scale = glm::vec3(1.0f);
		static glm::mat4 matrix_model(1.0f);
		static bool wireframe_enabled = false;

		/* update matrices */
		model_shader["mProjection"] = matrix_projection;
		model_shader["mView"] = glm::lookAt(
			camera_pos,
			camera_target,
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		model_shader["mModel"] = glm::scale(matrix_model, scale);
		
		/* ImGui */
		ImGui::Begin("Camera");
			ImGui::SliderFloat3("Color", &clear_color[0], 0.0f, 1.0f);
			ImGui::Separator();
			ImGui::SliderFloat3("Position", &camera_pos[0], -10.0f, 10.0f);
			ImGui::SliderFloat3("Target", &camera_target[0], -10.0f, 10.0f);
			ImGui::SliderFloat3("Scale", &scale[0], 0.01f, 10.0f);
		ImGui::End();

		ImGui::Begin("Render Settings");
			if (ImGui::Checkbox("Wireframe", &wireframe_enabled)) {
				window.render_wireframe(wireframe_enabled);
			}
		ImGui::End();

		/* rendering */
		glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		model_shader.use();
		model_texture.bind(0);
		model_mesh.render();
	});
}

