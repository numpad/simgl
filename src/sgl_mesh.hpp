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
	std::vector<GLuint> vertex_layout;

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
	
	GLuint get_vertex_array();

	mesh(std::vector<GLuint> layout = {3, 3, 2});
	mesh(std::string obj_path, std::vector<GLuint> layout = {3, 3, 2});
	~mesh();
	
	/* load from file */
	void load(std::string obj_path);

	/* load from memory */
	void add_vertex(std::vector<GLfloat> data);
	void load();
	

	/* bind required data (vao, textures, ...) and call glDraw* */
	void render();
	void render_instanced(GLuint count);

};

}

#endif
