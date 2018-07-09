#include "sgl_framebuffer.hpp"

sgl::framebuffer::framebuffer()
{

}

sgl::framebuffer::framebuffer(std::string filename)
{
	/* TODO: merge with sgl::texture::load into one static, public method void *sgl::texture::load_pixels(&width, &height); */
	/* load texture data */
	stbi_set_flip_vertically_on_load(1);
	int channels_in_file;
	int w, h;
	GLubyte *tex_data = stbi_load(filename.c_str(), &w, &h, &channels_in_file, 0);
	constexpr GLuint gl_channels[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
	
	this->width = (GLuint)w;
	this->height = (GLuint)h;

	GLuint channels = gl_channels[channels_in_file - 1];

	/* upload texture data to gpu */
	this->load(this->width, this->height, tex_data, channels, channels, GL_UNSIGNED_BYTE);
	
	/* free resources */
	stbi_image_free(tex_data);
}

sgl::framebuffer::framebuffer(GLuint width, GLuint height)
{
	this->load(width, height, NULL);
}

sgl::framebuffer::framebuffer(sgl::window &window)
	: sgl::framebuffer::framebuffer(window.width, window.height)
{
	
}

void sgl::framebuffer::load(GLuint width, GLuint height, void *pixels, GLuint channels_src, GLuint channels_dst, GLuint type)
{
	glGenFramebuffers(1, &this->fbo);
	glGenTextures(1, &this->texture_color_att);
	glGenRenderbuffers(1, &this->rbo);
	
	this->width = width;
	this->height = height;

	this->bind();
	
	/* create texture */
	glBindTexture(GL_TEXTURE_2D, this->texture_color_att);
	glTexImage2D(GL_TEXTURE_2D, 0, channels_dst, width, height, 0, channels_src, type, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* create renderbuffer */
	glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* attach */
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture_color_att, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

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

void sgl::framebuffer::unbind_texture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

sgl::framebuffer::~framebuffer()
{
	glDeleteFramebuffers(1, &this->fbo);
	glDeleteTextures(1, &this->texture_color_att);
	glDeleteRenderbuffers(1, &this->rbo);
}

void sgl::framebuffer::load_depthbuffer(GLuint width, GLuint height)
{
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &this->fbo);
	glGenTextures(1, &this->texture_color_att);
	
	/* shadow map texture */
	glBindTexture(GL_TEXTURE_2D, this->texture_color_att);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
	             0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	float bgcolor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bgcolor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	
	/* framebuffer */
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->texture_color_att, 0);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	/* errors? */
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("framebuffer not complete!\n");
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

