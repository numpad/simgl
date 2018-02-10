#ifndef SGL_SHADER_UNIFORM_HPP
#define SGL_SHADER_UNIFORM_HPP

#include <GL/gl3w.h>

#include <glm/glm.hpp>

namespace sgl {

class shader_uniform {
	const GLuint shader_program;
	const GLint uniform_location;

	/* save active program before switching to this->shader_program and restore later */
	GLint saved_program;
	void use_program();
	void restore_previous_program();

public:
	shader_uniform(const GLuint shader_program, const GLint location);

	/* scalar */
	void operator=(const GLfloat &v);

	/* vector */
	void operator=(const glm::vec2 &vec2);
	void operator=(const glm::vec3 &vec3);
	void operator=(const glm::vec4 &vec4);

	/* matrix */
	void operator=(const glm::mat2 &mat2);
	void operator=(const glm::mat3 &mat3);
	void operator=(const glm::mat4 &mat4);
	
};

}

#endif
