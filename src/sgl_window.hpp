#ifndef SGL_WINDOW_HPP
#define SGL_WINDOW_HPP

#include <stdio.h>
#include <string>

#include <functional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#define SGL_DEFAULT_MAJOR 3
#define SGL_DEFAULT_MINOR 3

namespace sgl {
	
	class window {
	public:
		typedef std::function<void (sgl::window &, int, int)> resize_callback;
		typedef std::function<void (sgl::window &, bool)> focus_callback;
		
	private:
		GLFWwindow *glfw_window;
		int _width, _height; /* read-write values */
		bool _focused;
		
		std::string _title;

		/* ms per frame */
		double _ms_per_frame_update_interval = 1.0;
		double _ms_per_frame;

		/* callbacks */
		sgl::window::resize_callback on_framebuffer_resize = nullptr;
		sgl::window::focus_callback on_focus_changed = nullptr;
		
		bool init_glfw_window(int win_width, int win_height, std::string win_title, bool win_fullscreen, int gl_major = SGL_DEFAULT_MINOR, int gl_minor = SGL_DEFAULT_MINOR);
		
		static void window_fb_resize_callback(GLFWwindow *window, int w, int h);
		static void window_focus_changed_callback(GLFWwindow *window, int focused);
		
	public:
		/* 
		 * 'read-only' references to _width, _height,
		 * cannot change dimensions by setting a variable
		 * -> "wrong" values when writing to it
		 */
		const int &width, &height;
		const bool &focused;
		
		window(int win_width, int win_height, std::string win_title = "sgl::window application", bool win_fullscreen = false);
		~window();
		
		/* window properties */
		std::string get_title();
		void set_title(std::string title);
		void set_title(const char *title);
		
		/* close window */
		void close(bool close_window = true);
		
		/* callback */
		void on_resize(sgl::window::resize_callback resize_callback);
		void on_focus(sgl::window::focus_callback focus_callback);
		void update(std::function<void (sgl::window &)> update_func);
		
	};
	
}

#endif

