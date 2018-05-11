#ifndef SGL_FRAMEBUFFER_HPP
#define SGL_FRAMEBUFFER_HPP

#include <stdio.h>
#include <GL/gl3w.h>

namespace sgl {

	class framebuffer {
		GLuint fbo, texture_color_att, rbo;
		
		GLsizei prev_viewport[4];

	public:
		GLuint width, height;

		framebuffer(GLuint width, GLuint height);
		~framebuffer();
		
		operator GLuint() const;

		void bind();
		void unbind();

	};

}

#endif
