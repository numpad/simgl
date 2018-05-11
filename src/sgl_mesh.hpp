#ifndef SGL_MESH_HPP
#define SGL_MESH_HPP

#include <string>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "sgl_modelloader.hpp"

namespace sgl {

/**
 * @brief Collection of vertices and their 
 */
class mesh {
	/* mesh data */
	std::vector<GLfloat> vertices;
	std::vector<GLuint> vertex_layout;
	
	/* instance data */
	std::vector<GLuint> instance_data_buffers;
	std::vector<size_t> instance_data_block_sizes;

	/* opengl data */
	GLuint vertex_buffer;
	GLuint vertex_array;
	GLuint index_buffer;
	
	GLuint indices;
	
	GLuint attrib_index = 0;
	size_t attrib_stride = 0;
	void init_vertex_data();
	void add_vertex_attrib(GLuint count, GLuint max_count, size_t sizeof_data = sizeof(GLfloat));
	void add_vertex_attribs(std::vector<GLuint> sizes, size_t sizeof_data = sizeof(GLfloat));
	
	/* use & restore vertex array */
	GLint prev_vertex_array;
	void use_vertex_array();
	void restore_vertex_array();
public:
	
	GLuint get_vertex_array();
	
	/**
	 * @brief Create an empty mesh.
	 * 
	 * Creates an empty mesh object, which still needs to be loaded before it
	 * can be rendered. The layout defines the vertex shader inputs, the
	 * default of {3, 3, 2} is mostly used as vec3 position, vec3 normal and
	 * vec2 texcoord.
	 *
	 * @code{.cpp}
	 *     #include <sgl_mesh.hpp>
	 *     
	 *     // Create a mesh, and define two vec3 inputs, position and color.
	 *     sgl::mesh cube_mesh(std::vector<GLuint>({3, 3}));
	 *     
	 * @endcode
	 */
	mesh(std::vector<GLuint> layout = {3, 3, 2});
	/**
	 * @brief Load a mesh from a wavefront object file.
	 * 
	 * Creates and loads a mesh from a simplified wavefront .obj file
	 * 
	 * @see sgl::mesh::load
	 */
	mesh(std::string obj_path, std::vector<GLuint> layout = {3, 3, 2});
	~mesh();
	
	/* load from file */
	void load(std::string obj_path);

	/* load from memory */
	void add_vertex(std::vector<GLfloat> data);
	void load();
	
	/* indexing */
	void set_indices(std::vector<GLuint> indices);

	/* instancing */
	size_t add_instance_buffer(std::vector<GLuint> layout, bool combine_data = false);
	void update_instance_buffer(size_t instance_buffer_index, GLvoid *data, size_t count);

	/* bind required data (vao, textures, ...) and call glDraw* */
	void render(GLenum draw_mode = GL_TRIANGLES);
	void render_indexed(GLenum draw_mode = GL_TRIANGLES, GLuint count = 0);
	void render_instanced(GLuint count, GLenum draw_mode = GL_TRIANGLES);

};

}

#endif
