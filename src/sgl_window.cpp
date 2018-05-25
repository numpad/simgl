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

bool sgl::window::init_context()
{
	/* initialize glfw, check for errors */
	if (!glfwInit()) {
		return false;
	}
	
	/* apply default window hints */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->window_ctx.gl_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->window_ctx.gl_minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); /* use core opengl without extensions */
	glfwWindowHint(GLFW_RESIZABLE, this->window_ctx.resizable);
	glfwWindowHint(GLFW_VISIBLE, this->window_ctx.visible);
	glfwWindowHint(GLFW_DECORATED, this->window_ctx.decorated);
	glfwWindowHint(GLFW_FOCUSED, this->window_ctx.focused);
	glfwWindowHint(GLFW_DOUBLEBUFFER, this->window_ctx.double_buffer);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, this->window_ctx.forward_compat);
	glfwWindowHint(GLFW_SAMPLES, this->window_ctx.samples);

	/* use primary monitor for fullscreen */
	GLFWmonitor *monitor = (this->window_ctx.fullscreen ? glfwGetPrimaryMonitor() : NULL);
	
	int set_width = sgl::window_context::DEFAULT_WIDTH;
	int set_height = sgl::window_context::DEFAULT_HEIGHT;
	
	if (this->window_ctx.fullscreen && monitor) {
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
		set_width = vidmode->width;
		set_height = vidmode->height;
	}

	if (this->window_ctx.width == GLFW_DONT_CARE)
		this->window_ctx.width = set_width;

	if (this->window_ctx.height == GLFW_DONT_CARE)
		this->window_ctx.height = set_height;
	
	this->_title = this->window_ctx.title;
	this->glfw_window = glfwCreateWindow(this->window_ctx.width, this->window_ctx.height, this->window_ctx.title.c_str(), monitor, NULL);
	if (!this->glfw_window) {
		return false;
	}
	glfwGetWindowSize(this->glfw_window, &this->_width, &this->_height);
	
	glfwMakeContextCurrent(this->glfw_window);
	glfwSwapInterval(this->window_ctx.swap_interval);
	
	/* register callbacks */
	glfwSetFramebufferSizeCallback(this->glfw_window, this->window_fb_resize_callback);
	
	/* set userpointer to glfw window */
	glfwSetWindowUserPointer(this->glfw_window, (void *)this);
	
	glfwSetWindowSizeLimits(this->glfw_window, this->window_ctx.min_width, this->window_ctx.min_height, this->window_ctx.max_width, this->window_ctx.max_height);

	/* init gl3w */
	if (gl3wInit()) {
		return false;
	}
	
	if (!gl3wIsSupported(this->window_ctx.gl_major, this->window_ctx.gl_minor)) {
		return false;
	}
	
	/* default opengl config */
	if (this->window_ctx.samples != GLFW_DONT_CARE) {
		glEnable(GL_MULTISAMPLE);
	}

	/* initialize input */
	sgl::input::attach(this->glfw_window);
	
	/* initialize nuklear */
	if (this->window_ctx.nuklear) {
		this->nk_ctx = nk_glfw3_init(this->glfw_window, NK_GLFW3_INSTALL_CALLBACKS);
		nk_glfw3_font_stash_begin(&this->nk_fontatlas);
		nk_glfw3_font_stash_end();
	}

	/* initialize imgui */
	if (this->window_ctx.dear_imgui) {
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui_ImplGlfwGL3_Init(this->glfw_window, true);
		ImGui::StyleColorsLight();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	}

	return true;
}


sgl::window::window(sgl::window_context wctx)
	: _width(wctx.width), _height(wctx.height), window_ctx(wctx),
	width(_width), height(_height),
	focused(_focused)
{
	this->init_context();
}

/* free resources */
sgl::window::~window()
{	
	if (this->window_ctx.nuklear) {
		nk_glfw3_shutdown();
	}

	if (this->window_ctx.dear_imgui) {
		ImGui_ImplGlfwGL3_Shutdown();
		ImGui::DestroyContext();
	}

	/* close window */
	glfwTerminate();
}

sgl::window::operator GLFWwindow*() const
{
	return glfw_window;
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

float sgl::window::aspect()
{
	return (float)this->width / (float)this->height;
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

void sgl::window::on_update(sgl::window::update_callback update_func)
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
		/* 
		 * call imgui/nuklear wrappers for newframe, may also call
		 * a do-nothing function if the library was not initialized
		 * in the window_context
		 */
		if (this->window_ctx.dear_imgui) {
			ImGui_ImplGlfwGL3_NewFrame();
		}
		if (this->window_ctx.nuklear) {
			nk_glfw3_new_frame();
		}

		update_func(*this, this->nk_ctx);
		

		if (this->window_ctx.nuklear) {
			nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
		}

		if (this->window_ctx.dear_imgui) {
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(this->glfw_window);
	}
	
}

void sgl::window::on_update(sgl::window::update_callback_nogui update_func)
{
	this->on_update([&](sgl::window &win, nk_context *nctx) {
		update_func(win);
	});
}

void sgl::window::render_wireframe(bool wf_enabled)
{
	glPolygonMode(GL_FRONT_AND_BACK, (wf_enabled ? GL_LINE : GL_FILL));
}

