#ifndef SGL_TEXTURE_HPP
#define SGL_TEXTURE_HPP

#include <string>
#include <GL/gl3w.h>
#include <stb_image.h>

namespace sgl {

class texture {
	GLuint texture_id;
	GLint _width, _height;
	
public:
	
	texture();
	texture(std::string fname);
	~texture();
	
	bool load(std::string fname);
	
	operator GLuint() const;
};

}

#endif

