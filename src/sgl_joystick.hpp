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
	
	/**
	 * @brief Check if this joystick is connected.
	 *
	 * @return true if connected.
	 */
	bool is_connected();
	
	/* axes */
	float get_axis(int index);
	glm::vec2 get_axis(int index_axis_x, int index_axis_y);
	int get_axes_count();
	
	/* buttons */
	/**
	 * @brief Check if button is pressed.
	 * 
	 * @param index Index of the button.
	 * @return truthy value if button at `index` is pressed
	 * 
	 * @see sgl::joystick::get_button_count()
	 */
	unsigned char get_button(int index);

	/**
	 * @brief Get the amount of buttons.
	 *
	 * Buttons on the available joystick are enumerated
	 * in the range [0, ::get_button_count()).
	 * 
	 * @return Amount of buttons.
	 *
	 * @see sgl::joystick::get_button()
	 */
	int get_button_count();
};

}

#endif

