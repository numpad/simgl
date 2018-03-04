#include "sgl_mesh.hpp"

sgl::mesh::mesh(std::vector<GLuint> layout)
{
	glGenVertexArrays(1, &this->vertex_array);
	glGenBuffers(1, &this->vertex_buffer);

	this->vertex_layout = layout;
}

sgl::mesh::mesh(std::string obj_path, std::vector<GLuint> layout) : mesh(layout)
{
	this->load(obj_path);
}


sgl::mesh::~mesh()
{
	glDeleteVertexArrays(1, &this->vertex_array);
	glDeleteBuffers(1, &this->vertex_buffer);
}

GLuint sgl::mesh::get_vertex_array()
{
	return this->vertex_array;
}

void sgl::mesh::init_vertex_data()
{
	this->use_vertex_array();
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(GLfloat), &(this->vertices[0]), GL_STATIC_DRAW);
	this->restore_vertex_array();
}

void sgl::mesh::add_vertex_attrib(GLuint count, GLuint max_count)
{
	this->use_vertex_array();
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	
	glVertexAttribPointer(this->attrib_index, count, GL_FLOAT, GL_FALSE, max_count * sizeof(GLfloat), (void *)(this->attrib_offset * sizeof(GLfloat)));
	glEnableVertexAttribArray(this->attrib_index);
	this->restore_vertex_array();
	
	this->attrib_index++;
	this->attrib_offset += count;
}

void sgl::mesh::add_vertex_attribs(std::vector<GLuint> sizes)
{
	GLuint stride = 0;
	for (size_t i = 0; i < sizes.size(); ++i) {
		stride += sizes.at(i);
	}
	
	for (size_t i = 0; i < sizes.size(); ++i)
		this->add_vertex_attrib(sizes.at(i), stride);
}

void sgl::mesh::use_vertex_array()
{
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &this->prev_vertex_array);
	glBindVertexArray(this->vertex_array);
}

void sgl::mesh::restore_vertex_array()
{
	glBindVertexArray(this->prev_vertex_array);
}

void sgl::mesh::load(std::string obj_path)
{
	/* load object file and fill sgl_mesh data */
	sgl::modelloader loaded(obj_path);
	this->vertices = loaded.pack({sgl::modelloader::PACK_VERTEX, sgl::modelloader::PACK_NORMAL, sgl::modelloader::PACK_UV});;
	
	this->init_vertex_data();
	this->add_vertex_attribs(this->vertex_layout);
}
	
void sgl::mesh::add_vertex(std::vector<GLfloat> data)
{
	this->vertices.insert(this->vertices.end(), data.begin(), data.end());
}

void sgl::mesh::load()
{
	this->init_vertex_data();
	this->add_vertex_attribs(this->vertex_layout);
}

void sgl::mesh::render()
{
	/* store current vertex array object */
	this->use_vertex_array();
	
	glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
	
	/* restore previous vao */
	this->restore_vertex_array();
}

void sgl::mesh::render_instanced(GLuint count)
{
	/* store current vertex array object */
	this->use_vertex_array();
	
	glDrawArraysInstanced(GL_TRIANGLES, 0, this->vertices.size(), count);
	
	/* restore previous vao */
	this->restore_vertex_array();
}

