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
		
		/* return color attachment */
		operator GLuint() const;

		/* bind/unbind framebuffer as render target */
		void bind();
		void unbind();

		/* activate as texture */
		void active(GLuint index);

	};

}

#endif
