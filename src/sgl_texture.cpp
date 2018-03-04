#include "sgl_texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

sgl::texture::texture(GLenum texture_type)
	: texture_id(0),
	type(texture_type)
{
	glGenTextures(1, &this->texture_id);
}

sgl::texture::texture(std::string fname, GLenum texture_type)
	: texture(texture_type)
{
	this->load(fname);
}

sgl::texture::texture(const sgl::texture &copy)
	: texture_id(copy.texture_id),
	type(copy.type)
{
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
	glBindTexture(this->type, this->texture_id);
	
	/* texture parameters */
	glTexParameteri(this->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(this->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(this->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	/* load texture data */
	stbi_set_flip_vertically_on_load(1);
	int channels_in_file;
	unsigned char *tex_data = stbi_load(fname.c_str(), &this->_width, &this->_height, &channels_in_file, 0);
	constexpr GLuint gl_channels[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};

	/* upload texture data to gpu */
	glTexImage2D(this->type, 0, GL_RGB, this->_width, this->_height, 0, gl_channels[channels_in_file - 1], GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(this->type);
	
	/* free resources */
	stbi_image_free(tex_data);
	glBindTexture(this->type, 0);

	return this->texture_id != 0;
}

bool sgl::texture::load_cubemap(std::vector<std::string> fnames)
{
	glBindTexture(this->type, this->texture_id);
	
	/* texture parameters */
	glTexParameteri(this->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(this->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(this->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(this->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	/*
	 * load texture data
	 * posx, negx, posy, negy, posz, negz
	 */
	stbi_set_flip_vertically_on_load(0);
	GLint width, height, channels_count;
	for (GLuint i = 0; i < fnames.size(); ++i) {
		unsigned char *data = stbi_load(fnames[i].c_str(), &width, &height, &channels_count, 0);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		);

		stbi_image_free(data);
	}

	glBindTexture(this->type, 0);

	return this->texture_id != 0;
}

void sgl::texture::bind(GLint value)
{
	glActiveTexture(GL_TEXTURE0 + value);
	glBindTexture(this->type, this->texture_id);
}

void sgl::texture::unbind()
{
	glBindTexture(this->type, 0);
}

