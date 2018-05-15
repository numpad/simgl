#include "sgl_framebuffer.hpp"

sgl::framebuffer::framebuffer(GLuint width, GLuint height) {
	glGenFramebuffers(1, &this->fbo);
	glGenTextures(1, &this->texture_color_att);
	glGenRenderbuffers(1, &this->rbo);
	
	this->width = width;
	this->height = height;

	this->bind();
	
	/* create texture */
	glBindTexture(GL_TEXTURE_2D, this->texture_color_att);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* create renderbuffer */
	glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* attach */
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture_color_att, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("framebuffer not complete\n");
	}

	this->unbind();

}

sgl::framebuffer::operator GLuint() const {
	return this->texture_color_att;
}

void sgl::framebuffer::bind() {
	glGetIntegerv(GL_VIEWPORT, this->prev_viewport);
	glViewport(0, 0, this->width, this->height);
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}
void sgl::framebuffer::unbind() {
	glViewport(prev_viewport[0], prev_viewport[1], prev_viewport[2], prev_viewport[3]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void sgl::framebuffer::bind_texture(GLuint index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, this->texture_color_att);
}

sgl::framebuffer::~framebuffer() {
	glDeleteFramebuffers(1, &this->fbo);
	glDeleteTextures(1, &this->texture_color_att);
	glDeleteRenderbuffers(1, &this->rbo);
}
