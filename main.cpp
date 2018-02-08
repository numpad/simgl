#include <stdio.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sgl_window.hpp"

int main(int argc, char *argv[]) {
	
	sgl::window window(800, 600);
	window.on_resize([](sgl::window &, int w, int h){ glViewport(0, 0, w, h); });
	
	GLfloat vertices[] = {
		
	};
	
	window.update([=](sgl::window &){
		glClearColor(0.5f, 0.2f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	});
	
	return 0;
}

