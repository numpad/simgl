#include "sgl_input.hpp"

GLFWwindow *sgl::input::window;

void sgl::input::attach(GLFWwindow *input_from_window)
{
	sgl::input::window = input_from_window;
}

void sgl::input::get_mouse(float &x, float &y)
{
	double tx, ty;
	glfwGetCursorPos(sgl::input::window, &tx, &ty);
	x = tx;
	y = ty;
}

void sgl::input::set_mouse(float x, float y)
{
	glfwSetCursorPos(sgl::input::window, x, y);
}

bool sgl::input::get_button(int btn)
{
	return glfwGetMouseButton(sgl::input::window, btn) == GLFW_PRESS;
}

bool sgl::input::get_key(int key)
{
	return glfwGetKey(sgl::input::window, key) == GLFW_PRESS;
}

