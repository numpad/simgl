#ifndef SGL_INPUT_HPP
#define SGL_INPUT_HPP

#include <GLFW/glfw3.h>

namespace sgl {
	
	class input {
		static GLFWwindow *window;

	public:
		static void attach(GLFWwindow *);

		/**
		 * @brief Get the cursor position.
		 *
		 * @param [out] x Mouse position in screen coordinates from the left border.
		 * @param [out] y Mouse position in screen coordinates from the top border.
		 */
		static void get_mouse(float &x, float &y);

		/**
		 * @brief Set the cursor position.
		 * 
		 * @param [in] x Mouse position in screen coordinates from the left border.
		 * @param [in] y Mouse position in screen coordinates from the top border.
		 */
		static void set_mouse(float x, float y);
		
		/**
		 * @brief Check if the mouse button is pressed.
		 * 
		 * @param btn GLFW Mouse button.
		 */
		static bool get_button(int btn);

		/**
		 * @brief Check if the key is pressed.
		 * 
		 * @param key GLFW Keycode.
		 * @return true, if the key is pressed.
		 */
		static bool get_key(int key);
	};

}

#endif

