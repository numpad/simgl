#ifndef SGL_MODEL_HPP
#define SGL_MODEL_HPP

#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "sgl_modelloader.hpp"

namespace sgl {

class model {
	/* model data */
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<GLuint> indices;

	/* opengl data */
	std::vector<GLuint> vertex_buffers;
	GLuint vertex_arrays,
		   element_buffers;

public:



};

}

#endif
