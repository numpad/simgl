#ifndef SGL_WINDOW_CONTEXT_HPP
#define SGL_WINDOW_CONTEXT_HPP

#include <string>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace sgl {
	
	/**
	 * @brief Provides a basic configuration for sgl::window.
	 * 
	 * A window context can be created without any configuration.
	 * sgl::window#window() will accept either a sgl::window_context or create
	 * a default one.
	 * Most context settings that have no clear invalid value are configured by
	 * setting variables, for example sgl::window_context#fullscreen:
	 * 
	 * @code{.cpp}
	 *     #include "sgl_window.hpp"
	 *     #include "sgl_window_context.hpp" // Already available trought sgl_window.hpp
	 *
	 *     // Open our window in fullscreen, width and height will be automatically
	 *     // determined based on the primary monitors dimensions.
	 *     sgl::window_context wctx;
	 *     wctx.fullscreen = true;
	 *     
	 *     // Create a window configured by 'wctx'
	 *     sgl::window window(wctx);
	 * @endcode
	 * 
	 * @warning It is @em not guaranteed that configuration variables such as
	 *          sgl::window_context#width are initialized to their default
	 *          values. Most will hold a value indicating an 'uninitialized
	 *          state' (such as -1) to request automatic initialization on
	 *          window creation.
	 * 
	 * @see sgl::window#window()
	 */
	class window_context {
		friend class window;
		
		static constexpr int DEFAULT_WIDTH  = 800,
		                     DEFAULT_HEIGHT = 600;

		/* OpenGL major & minor version */
		int gl_major = 3,
			gl_minor = 2;
	public:
		/**
		 * @brief Width of the window in pixels.
		 */
		int width  = GLFW_DONT_CARE;
		/**
		 * @brief Height of the window in pixels.
		 */
		int height = GLFW_DONT_CARE;
		
		/**
		 * @brief Minimum width of the window in pixels. Defaults to no limit.
		 * 
		 * Because this requests the window manager to set a minimum width,
		 * this isn't guaranteed to work depending on the users environment.
		 * 
		 * @see sgl::window::width
		 * @see sgl::window::on_resize()
		 */
		int min_width  = GLFW_DONT_CARE;
		/**
		 * @brief Minimum height of the window in pixels. Defaults to no limit.
		 * 
		 * Because this requests the window manager to set a minimum height,
		 * this isn't guaranteed to work depending on the users environment.
		 * 
		 * @see sgl::window::height
		 * @see sgl::window::on_resize()
		 */
		int min_height = GLFW_DONT_CARE;
		
		/**
		 * @brief Maximum width of the window in pixels. Defaults to no limit.
		 * 
		 * Because this requests the window manager to set a maximum width,
		 * this isn't guaranteed to work depending on the users environment.
		 * 
		 * @see sgl::window::width
		 * @see sgl::window::on_resize()
		 */
		int max_width  = GLFW_DONT_CARE;
		/**
		 * @brief Maximum height of the window in pixels. Defaults to no limit.
		 * 
		 * Because this requests the window manager to set a maximum height,
		 * this isn't guaranteed to work depending on the users environment.
		 * 
		 * @see sgl::window::height
		 * @see sgl::window::on_resize()
		 */
		int max_height = GLFW_DONT_CARE;
		
		/**
		 * @brief Set the window to be resizable.
		 * 
		 * Requests the window to be (not) resizable.
		 *
		 * @warning This isn't guaranteed to work depending on the users
		 *          environment.
		 * 
		 * @see sgl::window::on_resize()
		 */
		bool resizable      = true;
		/**
		 * @brief Set the window to be visible.
		 *
		 * Requests the window to be (not) visible.
		 */
		bool visible        = true;
		/**
		 * @brief Request the window manager to (not) draw window decorations.
		 */
		bool decorated      = true;
		/**
		 * @brief Requests the window to be (not) focused.
		 */
		bool focused        = true;
		/**
		 * @brief Requests (no) double buffering.
		 */
		bool double_buffer  = true;
		/**
		 * @brief Request a forward compatible context.
		 */
		bool forward_compat = false;
		/**
		 * @brief Set the swap interval.
		 */
		int swap_interval = 1;
		/**
		 * @brief Number of samples for multisampling.
		 */
		int samples = GLFW_DONT_CARE;

		/**
		 * @brief Enable/Disable fullscreen.
		 * 
		 * Without specifying any other (related) options (e.g.
		 * sgl::window_context#width, sgl::window_context#height)
		 * a fullscreen window will default to the dimensions of the
		 * primary monitor.
		 * 
		 * @see sgl::window_context#width
		 * @see sgl::window_context#height
		 */
		bool fullscreen = false;
		
		/**
		 * @brief Set the title of the window.
		 */
		std::string title = "sgl::window application";

		/**
		 * @brief Specify the OpenGL client version API.
		 * 
		 * simgl will default to OpenGL core profile 3.2 if
		 * sgl::window_context#set_version() is not called.
		 * 
		 * @param major OpenGL context version major.
		 * @param minor OpenGL context version minor.
		 * @return true if the specified context is supported and was set.
		 *         false otherwise.
		 */
		bool set_version(int major, int minor);
	};

}

#endif

