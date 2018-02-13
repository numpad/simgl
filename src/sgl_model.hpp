#ifndef SGL_MODEL_HPP
#define SGL_MODEL_HPP

#include <string>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "sgl_modelloader.hpp"

namespace sgl {

class model {
	/* model data */
	std::vector<GLfloat> vertices;

	/* opengl data */
	GLuint vertex_buffer;
	GLuint vertex_array;
	
	GLuint attrib_index = 0;
	GLuint attrib_offset = 0;
	void init_vertex_data();
	void add_vertex_attrib(GLuint count, GLuint max_count);
	void add_vertex_attribs(std::vector<GLuint> sizes);
	
	/* use & restore vertex array */
	GLint prev_vertex_array;
	void use_vertex_array();
	void restore_vertex_array();
public:
	
	model();
	model(std::string obj_path);
	~model();
	
	void load(std::string obj_path);
	
	/* bind required data (vao, textures, ...) and call glDraw* */
	void render();

};

}

#endif
