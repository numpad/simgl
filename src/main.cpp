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
#include "sgl_joystick.hpp"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char *argv[]) {
#endif

	bool rotate_model = false,
		 wireframe = false;
	
	sgl::window window(800, 600);
	window.on_resize([](sgl::window &, int w, int h){ glViewport(0, 0, w, h); });
	window.on_key([&](sgl::window &, int key, int, int action, int) {
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			rotate_model = !rotate_model;
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			wireframe = !wireframe;
			window.render_wireframe(wireframe);
		}
	});

	sgl::joystick controller(0);
	
	/* matrices */
	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

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
	sgl::model cube("assets/world.obj");
	sgl::texture donut_tex("assets/world.png");
	tshader["teximage"] = 0;
	tshader["viewport"] = glm::vec2(window.width, window.height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	glEnable(GL_CULL_FACE);
	
	window.update([&](sgl::window &) {
		/* inputs */
#ifdef SGL_DEBUG
		printf("\x1b[2J\x1b[0;0H");
		for (int i = 0; i < controller.get_axes_count(); ++i) {
			std::cout << "Axis #" << i << ": " << controller.get_axis(i) << std::endl;
		}
#endif
		
		/* recalculate matrices */
		projection = glm::perspective(45.0f, (float)window.width / (float)window.height, 0.1f, 100.0f);
		if (rotate_model)
			model = glm::rotate(model, glm::radians(1.0f + controller.get_axis(0) * 4.5f), glm::vec3(0.0f, 1.0f, 0.0f));
		MVP = projection * view * model;
		
		/* update shader uniform mvp */
		tshader[uMVP] = MVP;
		
		/* render code */
		glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, donut_tex);
		
		glUseProgram(tshader);
		cube.render();
		glUseProgram(0);
		
	});
	
	return 0;
}

