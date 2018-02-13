#include <stdio.h>

#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sgl_window.hpp"
#include "sgl_shader.hpp"
#include "sgl_model.hpp"
#include "sgl_texture.hpp"

int main(int argc, char *argv[]) {
	
	sgl::window window(800, 600);
	window.on_resize([](sgl::window &, int w, int h){ glViewport(0, 0, w, h); });
	
	/* matrices */
	glm::mat4 model(1.0f);
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 2.0f, 5.0f),
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
	
	/* model */
	sgl::model cube("assets/donut2.obj");
	sgl::texture donut_tex("assets/donut.png");
	
	tshader["teximage"] = 0;
	
	glEnable(GL_DEPTH_TEST);

	window.update([&](sgl::window &){
		model = glm::rotate(model, glm::radians(3.5f), glm::vec3(0.1f, 1.0f, -0.2f));
		
		MVP = projection * view * model;
		tshader[uMVP] = MVP;
		
		glClearColor(0.5f, 0.2f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, donut_tex);
		
		glUseProgram(tshader);
		cube.render();
		glUseProgram(0);
		
	});
	
	return 0;
}

