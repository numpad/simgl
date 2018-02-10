#include <stdio.h>

#include <GL/gl3w.h>

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
		/* position */				/* color */
		-1.41f,  1.41f,  0.00f,		1.0f, 0.0f, 0.0f,
		 1.41f,  1.41f,  0.00f,		0.0f, 1.0f, 0.0f,
		 0.00f, -1.00f,  0.00f,		0.0f, 0.0f, 1.0f
	};
	GLuint indices[] = {
		0, 1, 2
	};

	/* vertex array */
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	/* matrices */
	glm::mat4 model(1.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 5.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
	glm::mat4 MVP = projection * view * model;
	
	/* shader */
	sgl::shader tshader("vert.glsl", "frag.glsl");
	GLint uMVP = tshader["MVP"];
	tshader["color"] = glm::vec3(0.0f, 1.0f, 0.0f);
	tshader[uMVP] = MVP;

	glEnable(GL_DEPTH_TEST);

	window.update([&](sgl::window &){
		model = glm::rotate(model, glm::radians(3.5f), glm::vec3(-0.2f, 0.95f, 0.75f));
		MVP = projection * view * model;
		tshader[uMVP] = MVP;
		
		glClearColor(0.5f, 0.2f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(tshader());
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
		
	});
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	
	return 0;
}

