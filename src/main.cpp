#include <stdio.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sgl_window.hpp"
#include "sgl_shader.hpp"

int main(int argc, char *argv[]) {
	
	sgl::window window(800, 600);
	window.on_resize([](sgl::window &, int w, int h){ glViewport(0, 0, w, h); });
	
	/* vertices */
	GLfloat vertices[] = {
		-0.9f,  0.9f,  0.0f,
		 0.9f,  0.9f,  0.0f,
		 0.0f, -0.9f,  0.0f
	};
	
	/* matrices */
	glm::mat4 model(1.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
	
	glm::mat4 MVP = projection * view * model;
	
	/* shader */
	sgl::shader tshader("vert.glsl", "frag.glsl");
	
	tshader["MVP"] = glm::mat4(1.0);
	
	window.update([=](sgl::window &){
		glClearColor(0.5f, 0.2f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(tshader());
		glUseProgram(0);
	});
	
	return 0;
}

