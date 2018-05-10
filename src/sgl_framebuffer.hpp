#ifndef SGL_FRAMEBUFFER_HPP
#define SGL_FRAMEBUFFER_HPP

#include <stdio.h>
#include <GL/gl3w.h>

namespace sgl {

	class framebuffer {
		GLuint fbo, texture_color_att, rbo;

	public:

		framebuffer(GLuint width, GLuint height);
		~framebuffer();

		void attach();

		void bind();
		void unbind();

	};

}

#endif
