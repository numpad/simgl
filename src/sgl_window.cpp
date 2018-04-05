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

void sgl::window::window_close_callback(GLFWwindow *window)
{
	sgl::window *curr_win = (sgl::window *)glfwGetWindowUserPointer(window);
	if (curr_win->on_closed != nullptr)
		curr_win->on_closed(*curr_win);
}

bool sgl::window::init_context(sgl::window_context &wctx)
{
	/* initialize glfw, check for errors */
	if (!glfwInit()) {
		return false;
	}
	
	/* apply default window hints */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, wctx.gl_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, wctx.gl_minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); /* use core opengl without extensions */
	glfwWindowHint(GLFW_RESIZABLE, wctx.resizable);
	glfwWindowHint(GLFW_VISIBLE, wctx.visible);
	glfwWindowHint(GLFW_DECORATED, wctx.decorated);
	glfwWindowHint(GLFW_FOCUSED, wctx.focused);
	glfwWindowHint(GLFW_DOUBLEBUFFER, wctx.double_buffer);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, wctx.forward_compat);
	glfwWindowHint(GLFW_SAMPLES, wctx.samples);

	
	/* use primary monitor for fullscreen */
	GLFWmonitor *monitor = (wctx.fullscreen ? glfwGetPrimaryMonitor() : NULL);
	
	int set_width = sgl::window_context::DEFAULT_WIDTH;
	int set_height = sgl::window_context::DEFAULT_HEIGHT;
	
	if (wctx.fullscreen && monitor) {
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
		set_width = vidmode->width;
		set_height = vidmode->height;
	}

	if (wctx.width == GLFW_DONT_CARE)
		wctx.width = set_width;

	if (wctx.height == GLFW_DONT_CARE)
		wctx.height = set_height;
	
	this->_title = wctx.title;
	this->glfw_window = glfwCreateWindow(wctx.width, wctx.height, wctx.title.c_str(), monitor, NULL);
	if (!this->glfw_window) {
		return false;
	}
	glfwGetWindowSize(this->glfw_window, &this->_width, &this->_height);
	
	glfwMakeContextCurrent(this->glfw_window);
	glfwSwapInterval(wctx.swap_interval);
	
	/* register callbacks */
	glfwSetFramebufferSizeCallback(this->glfw_window, this->window_fb_resize_callback);
	
	/* set userpointer to glfw window */
	glfwSetWindowUserPointer(this->glfw_window, (void *)this);
	
	glfwSetWindowSizeLimits(this->glfw_window, wctx.min_width, wctx.min_height, wctx.max_width, wctx.max_height);

	/* init gl3w */
	if (gl3wInit()) {
		return false;
	}
	
	if (!gl3wIsSupported(wctx.gl_major, wctx.gl_minor)) {
		return false;
	}
	
	/* initialize dear imgui */
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(this->glfw_window, true);
	
	return true;
}


sgl::window::window(sgl::window_context wctx)
	: _width(wctx.width), _height(wctx.height),
	width(_width), height(_height),
	focused(_focused)
{
	this->init_context(wctx);
}

/* free resources */
sgl::window::~window()
{
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
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

void sgl::window::set_size(int width, int height)
{
	this->_width = width;
	this->_height = height;
	glfwSetWindowSize(this->glfw_window, width, height);
}

void sgl::window::capture_cursor(bool capture_enabled)
{
	glfwSetInputMode(this->glfw_window, GLFW_CURSOR, (capture_enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
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

void sgl::window::on_close(sgl::window::close_callback close_callback)
{
	this->on_closed = close_callback;
	glfwSetWindowCloseCallback(this->glfw_window, this->window_close_callback);
}

void sgl::window::on_update(std::function<void (sgl::window &)> update_func)
{
	/* measure fps */
	double last_time = glfwGetTime();
	int nb_frames = 0;
	/* update window title with fps */
	const char title_format[] = "%s (%gms)";
	char win_title[128];
	const std::string last_title = this->get_title();

	while (!glfwWindowShouldClose(this->glfw_window)) {
		/* calculate average fps over x seconds */
		double curr_time = glfwGetTime();
		nb_frames++;
		if (curr_time - last_time >= this->_ms_per_frame_update_interval) {
			this->_ms_per_frame = (1000.0 * this->_ms_per_frame_update_interval) / double(nb_frames);
			nb_frames = 0;
			last_time += this->_ms_per_frame_update_interval;
			/* update window title */
			sprintf(win_title, title_format, last_title.c_str(), this->_ms_per_frame);
			this->set_title(win_title);
		}
		
		
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		
		update_func(*this);
		
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(this->glfw_window);
	}
	
}

void sgl::window::render_wireframe(bool wf_enabled)
{
	glPolygonMode(GL_FRONT_AND_BACK, (wf_enabled ? GL_LINE : GL_FILL));
}

