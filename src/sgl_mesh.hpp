#ifndef SGL_MESH_HPP
#define SGL_MESH_HPP

#include <string>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "sgl_modelloader.hpp"

namespace sgl {

class mesh {
	/* mesh data */
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
	
	mesh();
	mesh(std::string obj_path);
	~mesh();
	
	void load(std::string obj_path);
	
	/* bind required data (vao, textures, ...) and call glDraw* */
	void render();

};

}

#endif
