#ifndef SGL_JOYSTICK_HPP
#define SGL_JOYSTICK_HPP

/* 
 * remove the following line to increase performance
 * if SGL_JOYSTICK_FAILSAFE is not defined, working
 * with a disconnected joystick may result in a crash
 * if this scenario is not handled correctly.
 * however, using SGL_JOYSTICK_FAILSAFE is discouraged
 * because it encourages laziness and decreases performance
 */
#define SGL_JOYSTICK_FAILSAFE
#ifdef SGL_JOYSTICK_FAILSAFE
	#define SGL_FAILSAFE_RETURN(R) do { if (!this->is_connected()) return (R);  } while (0);
#else
	#define SGL_FAILSAFE_RETURN(R)
#endif

#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace sgl {

class joystick {
	int joy_id;

public:
	
	joystick(int id);
	
	/* return human-readable name of joystick */
	std::string get_name();
	
	/* check if joystick is connected */
	bool is_connected();
	
	/* axes */
	float get_axis(int index);
	glm::vec2 get_axis(int index_axis_x, int index_axis_y);
	int get_axes_count();
	
	/* buttons */
	unsigned char get_button(int index);
	int get_button_count();
};

}

#endif

