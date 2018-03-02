#include "sgl_window_context.hpp"

bool sgl::window_context::set_version(int major, int minor)
{
	/* check if version is supported */
	if (!gl3wIsSupported(major, minor)) {
		return false;
	}

	this->gl_major = major;
	this->gl_minor = minor;

	return true;
}

