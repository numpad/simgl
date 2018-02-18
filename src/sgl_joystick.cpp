#include "sgl_joystick.hpp"

sgl::joystick::joystick(int id)
	: joy_id(id)
{
}

std::string sgl::joystick::get_name()
{
	SGL_FAILSAFE_RETURN(std::string("<none>"));
	
	return std::string(glfwGetJoystickName(this->joy_id));
}

bool sgl::joystick::is_connected()
{
	return glfwJoystickPresent(this->joy_id);
}

float sgl::joystick::get_axis(int index)
{
	SGL_FAILSAFE_RETURN(0.0f);
	
	int count;
	const float *axes = glfwGetJoystickAxes(this->joy_id, &count);
	return axes[index];
}

glm::vec2 sgl::joystick::get_axis(int index_axis_x, int index_axis_y)
{
	SGL_FAILSAFE_RETURN(glm::vec2(0.0f, 0.0f));
	
	int count;
	const float *axes = glfwGetJoystickAxes(this->joy_id, &count);
	return glm::vec2(axes[index_axis_x], axes[index_axis_y]);
}

int sgl::joystick::get_axes_count()
{
	SGL_FAILSAFE_RETURN(-1);
	
	int count;
	(void)glfwGetJoystickAxes(this->joy_id, &count);
	return count;
}

unsigned char sgl::joystick::get_button(int index)
{
	SGL_FAILSAFE_RETURN(0);
	
	int count;
	const unsigned char *buttons = glfwGetJoystickButtons(this->joy_id, &count);
	return buttons[index];
}

int sgl::joystick::get_button_count()
{
	SGL_FAILSAFE_RETURN(-1);
	
	int count;
	(void)glfwGetJoystickButtons(this->joy_id, &count);
	return count;
}

