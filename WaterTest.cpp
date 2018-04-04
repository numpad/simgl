#include <iostream>

#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "src/sgl_window.hpp"
#include "src/sgl_camera.hpp"
#include "src/sgl_texture.hpp"
#include "src/sgl_mesh.hpp"
#include "src/sgl_shader.hpp"

#define VERTEX(MESH,pos,color) MESH.add_vertex({pos.x, pos.y, pos.z, color.x, color.y, color.z});

int main(int argc, char *argv[]) {
	sgl::window_context wctx;
	sgl::window window(wctx);

	window.on_resize([](sgl::window &, int w, int h) { glViewport(0, 0, w, h); });

	sgl::mesh mesh(std::vector<GLuint>{3, 3});
	VERTEX(mesh, glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	VERTEX(mesh, glm::vec3( 0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	VERTEX(mesh, glm::vec3( 0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mesh.load();

	sgl::shader shader;
	shader.load_from_memory(R"(
		#version 330 core
		
		layout (location = 0) in vec3 vPos;
		layout (location = 1) in vec3 vCol;

		out vec4 Color;

		void main() {
			Color = vec4(vCol, 1.0);
			gl_Position = vec4(vPos, 1.0);
		}
	)", sgl::shader::VERTEX);

	shader.load_from_memory(R"(
		#version 330 core
		
		in vec4 Color;
		out vec4 FragColor;
		
		void main() {
			FragColor = Color;
		}
	)", sgl::shader::FRAGMENT);

	shader.compile(sgl::shader::VERTEX);
	shader.compile(sgl::shader::FRAGMENT);
	shader.link();

	glEnable(GL_DEPTH_TEST);
	window.on_update([&](sgl::window &) {
		glClearColor(0.6f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shader.use();
		mesh.render();
	});

	return 0;
}
