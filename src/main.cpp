#include <iostream>

#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "sgl_window.hpp"
#include "sgl_shader.hpp"
#include "sgl_model.hpp"
#include "sgl_texture.hpp"

//#include <locale>

int main(int argc, char *argv[]) {
	//std::wcout.sync_with_stdio(false);
	//std::wcout.imbue(std::locale("de_DE.utf8"));
	
	bool rotate_model = true;
	sgl::window window(800, 600);
	window.on_resize([](sgl::window &, int w, int h){ glViewport(0, 0, w, h); });
	window.on_character([](sgl::window &, unsigned int codepoint) {
		std::wcout << "Character: " << codepoint << " => " << wchar_t(codepoint) << std::endl;
	});
	window.on_key([&rotate_model](sgl::window &, int key, int, int action, int) {
		std::cout << "Key: " << (char)key << " (" << (action == GLFW_PRESS ? "pressed" : "released") << ")" << std::endl;
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			rotate_model = !rotate_model;
	});
	
	/* matrices */
	glm::mat4 model(1.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 2.0f, 5.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
	glm::mat4 MVP = projection * view * model;
	
	/* shader */
	sgl::shader tshader("assets/vert.glsl", "assets/frag.glsl");
	GLint uMVP = tshader["MVP"];
	tshader[uMVP] = MVP;
	
	/* model */
	sgl::model cube("assets/monkey.obj");
	sgl::texture donut_tex("assets/monkey.png");
	sgl::texture donut_tex2("assets/monkey2.png");
	tshader["teximage"] = 0;
	tshader["teximage2"] = 1;
	
	glEnable(GL_DEPTH_TEST);
	
	float ts = glfwGetTime();
	window.update([&](sgl::window &) {
		/* recalculate matrices */
		projection = glm::perspective(45.0f, (float)window.width / (float)window.height, 0.1f, 100.0f);
		if (rotate_model)
			model = glm::rotate(model, glm::radians(3.5f), glm::vec3(sin(ts * 10.0), 1.0f, -0.4f));
		MVP = projection * view * model;
		
		/* update shader uniform mvp */
		tshader[uMVP] = MVP;
		
		/* render code */
		glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, donut_tex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, donut_tex2);
		
		glUseProgram(tshader);
		cube.render();
		glUseProgram(0);
		
	});
	
	return 0;
}

