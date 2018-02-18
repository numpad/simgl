#include "sgl_window.hpp"

void sgl::window::window_fb_resize_callback(GLFWwindow *window, int w, int h) {
	sgl::window *curr_win = (sgl::window *)glfwGetWindowUserPointer(window);
	curr_win->_width = w;
	curr_win->_height = h;
	if (curr_win->on_framebuffer_resize != nullptr)
		curr_win->on_framebuffer_resize(*curr_win, w, h);
}
void sgl::window::window_focus_changed_callback(GLFWwindow *window, int focused)
{
	sgl::window *curr_win = (sgl::window *)glfwGetWindowUserPointer(window);
	curr_win->_focused = focused;
	if (curr_win->on_focus_changed != nullptr)
		curr_win->on_focus_changed(*curr_win, focused);
}

void sgl::window::window_character_input_callback(GLFWwindow *window, unsigned int codepoint)
{
	sgl::window *curr_win = (sgl::window *)glfwGetWindowUserPointer(window);
	if (curr_win->on_character_input != nullptr)
		curr_win->on_character_input(*curr_win, codepoint);
}

void sgl::window::window_key_input_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	sgl::window *curr_win = (sgl::window *)glfwGetWindowUserPointer(window);
	if (curr_win->on_key_input != nullptr)
		curr_win->on_key_input(*curr_win, key, scancode, action, mode);
}

bool sgl::window::init_glfw_window(int win_width, int win_height, std::string win_title, bool win_fullscreen, int gl_major, int gl_minor)
{
	/* initialize glfw, check for errors */
	if (!glfwInit()) {
		puts("glfw init failed");
		return false;
	}
	
	/* apply default window hints */
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); /* use core opengl without extensions */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	/* use primary monitor for fullscreen */
	GLFWmonitor *monitor = (win_fullscreen ? glfwGetPrimaryMonitor() : NULL);
	
	this->_title = win_title;
	this->glfw_window = glfwCreateWindow(win_width, win_height, this->_title.c_str(), monitor, NULL);
	if (!this->glfw_window) {
		puts("glfw window creation failed");
		return false;
	}
	
	glfwMakeContextCurrent(this->glfw_window);
	glfwSwapInterval(1);
	
	/* register callbacks */
	glfwSetFramebufferSizeCallback(this->glfw_window, this->window_fb_resize_callback);
	
	/* set userpointer to glfw window */
	glfwSetWindowUserPointer(this->glfw_window, (void *)this);
	
	/* init gl3w */
	if (gl3wInit()) {
		puts("gl3w init failed!");
		return false;
	}
	
	if (!gl3wIsSupported(gl_major, gl_minor)) {
		printf("gl version %d.%d not supported!\n", gl_major, gl_minor);
		return false;
	}
	
	return true;
}

sgl::window::window(int win_width, int win_height, std::string win_title, bool win_fullscreen)
	: _width(win_width), _height(win_height),
	  width(_width), height(_height),
	  focused(_focused)
{
	this->init_glfw_window(win_width, win_height, win_title, win_fullscreen);
}

/* free resources */
sgl::window::~window()
{
	/* close window */
	glfwTerminate();
}

std::string sgl::window::get_title()
{
	return this->_title;
}

void sgl::window::set_title(std::string title)
{
	this->set_title(title.c_str());
}

void sgl::window::set_title(const char *title)
{
	this->_title = std::string(title);
	glfwSetWindowTitle(this->glfw_window, title);
}

void sgl::window::close(bool close_window)
{
	glfwSetWindowShouldClose(this->glfw_window, (close_window ? GLFW_TRUE : GLFW_FALSE));
}

void sgl::window::on_resize(sgl::window::resize_callback resize_callback)
{
	this->on_framebuffer_resize = resize_callback;
	glfwSetFramebufferSizeCallback(this->glfw_window, this->window_fb_resize_callback);
}

void sgl::window::on_focus(sgl::window::focus_callback focus_callback)
{
	this->on_focus_changed = focus_callback;
	glfwSetWindowFocusCallback(this->glfw_window, this->window_focus_changed_callback);
}

void sgl::window::on_character(sgl::window::character_callback character_callback)
{
	this->on_character_input = character_callback;
	glfwSetCharCallback(this->glfw_window, this->window_character_input_callback);
}

void sgl::window::on_key(sgl::window::key_callback key_callback)
{
	this->on_key_input = key_callback;
	glfwSetKeyCallback(this->glfw_window, this->window_key_input_callback);
}

void sgl::window::update(std::function<void (sgl::window &)> update_func)
{
	double last_time = glfwGetTime();
	int nb_frames = 0;
	char win_title[128];

	const std::string last_title = this->get_title();

	while (!glfwWindowShouldClose(this->glfw_window)) {
		double curr_time = glfwGetTime();
		nb_frames++;
		if (curr_time - last_time >= this->_ms_per_frame_update_interval) {
			this->_ms_per_frame = (1000.0 * this->_ms_per_frame_update_interval) / double(nb_frames);
			nb_frames = 0;
			last_time += this->_ms_per_frame_update_interval;
			sprintf(win_title, "%s (%gms)", last_title.c_str(), this->_ms_per_frame);
			this->set_title(win_title);
		}

		update_func(*this);
		
		glfwSwapBuffers(this->glfw_window);
		glfwPollEvents();
	}
	
}

void sgl::window::render_wireframe(bool wf_enabled)
{
	glPolygonMode(GL_FRONT_AND_BACK, (wf_enabled ? GL_LINE : GL_FILL));
}

