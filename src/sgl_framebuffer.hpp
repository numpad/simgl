#ifndef SGL_FRAMEBUFFER_HPP
#define SGL_FRAMEBUFFER_HPP

#include <stdio.h>
#include <string>
#include <GL/gl3w.h>
#include <stb_image.h>
#include "sgl_window.hpp"

namespace sgl {

	class framebuffer {
		GLuint fbo, texture_color_att, rbo;
		
		GLsizei prev_viewport[4];
		
		void load(GLuint width, GLuint height, void *pixels,
				GLuint channels_src = GL_RGBA, GLuint channels_dst = GL_RGBA,
				GLuint type = GL_UNSIGNED_BYTE);

	public:
		GLuint width, height;
		
		framebuffer(std::string filename);
		framebuffer(GLuint width, GLuint height);
		framebuffer(sgl::window &window);
		~framebuffer();
		
		/* return color attachment */
		operator GLuint() const;
		
		/* bind/unbind framebuffer as render target */
		void bind();
		void unbind();

		/* activate as texture */
		void bind_texture(GLuint index);
		void unbind_texture();

	};

}

#endif
