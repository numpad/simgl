#include "sgl_texture.hpp"

sgl::texture::texture() : texture_id(0)
{
	glGenTextures(1, &this->texture_id);
}

sgl::texture::texture(std::string fname) : texture()
{
	this->load(fname);
}

sgl::texture::~texture()
{
	glDeleteTextures(1, &this->texture_id);
}

sgl::texture::operator GLuint() const
{
	return this->texture_id;
}

bool sgl::texture::load(std::string fname)
{
	glBindTexture(GL_TEXTURE_2D, this->texture_id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	GLubyte *tex_data = SOIL_load_image(fname.c_str(), &this->_width, &this->_height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->_width, this->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	SOIL_free_image_data(tex_data);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return this->texture_id != 0;
}

