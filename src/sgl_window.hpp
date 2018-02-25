#ifndef SGL_WINDOW_HPP
#define SGL_WINDOW_HPP

#include <stdio.h>
#include <string>

#include <functional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>

#define SGL_DEFAULT_MAJOR 3
#define SGL_DEFAULT_MINOR 3

namespace sgl {
	
	/**
	 * @brief Initializes an OpenGL context and provides a simple interface to quickly prototype multimedia applications.
	 *
	 * 
	 */
	class window {
	public:
		/* callbacks */
		typedef std::function<void (sgl::window &, int, int)>			resize_callback;
		typedef std::function<void (sgl::window &, bool)>				focus_callback;
		typedef std::function<void (sgl::window &, unsigned int)>		character_callback;
		typedef std::function<void (sgl::window &, int, int, int, int)>	key_callback;
		typedef std::function<void (sgl::window &)>						update_callback;
		typedef std::function<void (sgl::window &)>						close_callback;
	private:
		
		/* callback functions */
		sgl::window::resize_callback on_framebuffer_resize = nullptr;
		sgl::window::focus_callback on_focus_changed = nullptr;
		sgl::window::character_callback on_character_input = nullptr;
		sgl::window::key_callback on_key_input = nullptr;
		sgl::window::close_callback on_closed = nullptr;
		
		/* callback wrappers */
		static void window_fb_resize_callback(GLFWwindow *window, int w, int h);
		static void window_focus_changed_callback(GLFWwindow *window, int focused);
		static void window_character_input_callback(GLFWwindow *window, unsigned int codepoint);
		static void window_key_input_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
		static void window_close_callback(GLFWwindow *window);
		
		
		GLFWwindow *glfw_window;
		int _width, _height; /* read-write values */
		bool _focused;
		
		std::string _title;

		/* ms per frame */
		double _ms_per_frame_update_interval = 1.0;
		double _ms_per_frame;

		/* initializer methods */
		bool init_glfw_window(int win_width, int win_height, std::string win_title, bool win_fullscreen, int gl_major = SGL_DEFAULT_MINOR, int gl_minor = SGL_DEFAULT_MINOR);
	
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
		/**
		 * @brief Get the title of this window.
		 * 
		 * Returns a copy of the last window title that has been
		 * set by sgl::window#set_title().
		 * 
		 * @return A copy of the title.
		 * @see sgl::window#set_title()
		 */
		std::string get_title();
		/**
		 * @brief Set the title of this window.
		 *
		 * @param title The new title of the window.
		 * @see sgl::window#get_title()
		 */
		void set_title(std::string title);
		/** @copydoc sgl::window::set_title(std::string title) */
		void set_title(const char *title);
		
		/**
		 * @brief Requests the window to be (not) resizable.
		 * 
		 * 
		 * @warning This function @em kindly @em requests the window manager
		 *          to disable resizing. Some platforms may ignore this and
		 *          still allow changing the window size.
		 * 
		 * @param window_allow_resize Set to true to enable resizing.
		 * @see sgl::window#on_resize()
		 */
		void set_resizable(bool window_allow_resize = true);
		
		/* opengl debug */
		void render_wireframe(bool wf_enabled = true);

		/**
		 * @brief Request closing the window.
		 *
		 * After the window has been requested to close, calling the
		 * function with a falsy parameter will undo the close request.
		 * This needs to happen before the next call to sgl::window#on_update().
		 *
		 * @param close_window Sets the window flag @em 'window_should_close'.
		 * @see sgl::window#on_update()
		 */
		void close(bool close_window = true);
		
		/* callbacks */
		
		/** @{ */
		/**
		 * @brief Register a callback for when the window framebuffer is resized.
		 *
		 * The registered function of type sgl::window#resize_callback will get called
		 * with a reference to the current sgl::window and two ints @em width and @em height.
		 * They represent the framebuffers new size in pixels.
		 * 
		 * @code{.cpp}
		 *     #include <sgl_window.hpp>
		 *     
		 *     // Create a window and register a resize callback
		 *     sgl::window window(800, 600);
		 *     window.on_resize([](sgl::window &, int new_width, int new_height) {
		 *         glViewport(0, 0, new_width, new_height);
		 *     });
		 * @endcode
		 * 
		 */
		void on_resize(sgl::window::resize_callback resize_callback);
		/**
		 * @brief Register a callback for when the window focus has changed.
		 *
		 * The registered function of type sgl::window#focus_callback will get called
		 * with a reference to the current sgl::window and a bool @em focus_gained.
		 * This indicates if the window just @em gained or @em lost focus.
		 *
		 * @code{.cpp}
		 *     #include <sgl_window.hpp>
		 *
		 *     // Create a window and register a focus change callback
		 *     sgl::window window(800, 600);
		 *     window.on_focus([&game_engine](sgl::window &, bool focus_gained) {
		 *         // Pause the game when losing focus
		 *         if (!focus_gained)
		 *             game_engine.show_pause_screen();
		 *     });
		 * @endcode
		 * 
		 */
		void on_focus(sgl::window::focus_callback focus_callback);
		void on_character(sgl::window::character_callback character_callback);
		void on_key(sgl::window::key_callback key_callback);
		
		/**
		 * @brief Register a callback for when the window is requested to be closed.
		 * 
		 * The registered function of type sgl::window#close_callback will get called
		 * with a reference to the current sgl::window.
		 * This can be used to suppress a window close request, for example to
		 * ask for confirmation or wait until some operations are finished.
		 *
		 * @code{.cpp}
		 *     #include <sgl_window.hpp>
		 *
		 *     // Only close after having tried once.
		 *     bool tried_to_close = false;
		 *     
		 *     // Create a window and register a sgl::window::on_close() callback
		 *     sgl::window window(800, 600);
		 *     window.on_close([&](sgl::window &) {
		 *         // First time trying to close...
		 *         if (!tried_to_close) {
		 *             // ...cancel close request.
		 *             window.close(false);
		 *	           tried_to_close = true;
		 *             return;
		 *         }
		 *     });
		 * @endcode
		 */
		void on_close(sgl::window::close_callback close_callback);
		
		/**
		 * @brief Register an update function.
		 * 
		 * This will enter the @em game @em loop, which usually combines handling input,
		 * updating values and rendering to the window. Change the framerate of the
		 * sgl::window to control in which interval this function is called.
		 *
		 * @code{.cpp}
		 *     #include <sgl_window.hpp>
		 *     
		 *     // Define a red color
		 *     float color[] = {1.0f, 0.0f, 0.0f};
		 *
		 *     // Create a window and fill it with a color.
		 *     sgl::window window(800, 600);
		 *     window.on_update([&](sgl::window &) {
		 *	       glClearColor(color[0], color[1], color[2], 1.0f);
		 *         glClear(GL_COLOR_BUFFER_BIT);
		 *     });
		 *     
		 * @endcode
		 * 
		 * As shown, sgl::window#on_update() implicitly handles most of the work,
		 * such as polling for input, timing and presenting the framebuffer.
		 * 
		 * @warning This function blocks the current process until the window is closed.
		 */
		void on_update(sgl::window::update_callback update_func);
		/** @} */
	};
	
}

#endif

