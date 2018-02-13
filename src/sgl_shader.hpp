#ifndef SGL_SHADER_HPP
#define SGL_SHADER_HPP

#include <string.h>

#include <iostream>
#include <string>
#include <fstream>

#include <GL/gl3w.h>

#include "sgl_shader_uniform.hpp"

namespace sgl {

class shader {
public:
	enum type {
		VERTEX,
		FRAGMENT,
		MAX_TYPES
	};
	
	/* translate sgl::shader::type to OpenGL shader type */
	GLenum type_to_gl_shader[sgl::shader::MAX_TYPES] = {
		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER
	};
	
	std::string type_to_name[sgl::shader::MAX_TYPES] = {
		"vertex",
		"fragment"
	};
	
private:
	char *shaders_src[sgl::shader::MAX_TYPES];
	GLuint shaders[sgl::shader::MAX_TYPES];
	GLuint program;

	static std::string load_file(std::string fname);
	
public:
	
	shader(std::string fname_vert, std::string fname_frag);
	~shader();

	/* return shader program */
	GLuint operator()(void) const;
	operator GLuint() const;
	
	/* return location of uniform */
	GLint uniform(std::string uniform_name) const;
	GLint uniform(const char *uniform_name) const;
	
	/* set uniform */
	sgl::shader_uniform operator[](const char *uniform_name);
	sgl::shader_uniform operator[](GLint location);
	
	/* load shader from source */
	bool load(std::string fname, sgl::shader::type type);
	
	/* compile shader */
	bool compile(sgl::shader::type type);
	
	/* link shader program */
	bool link();
};

}

#endif

