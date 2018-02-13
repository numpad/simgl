#include "sgl_model.hpp"

sgl::model::model()
{
	glGenVertexArrays(1, &this->vertex_array);
	glGenBuffers(1, &this->vertex_buffer);
}

sgl::model::model(std::string obj_path) : model()
{
	this->load(obj_path);
}


sgl::model::~model()
{
	glDeleteVertexArrays(1, &this->vertex_array);
	glDeleteBuffers(1, &this->vertex_buffer);
}

void sgl::model::init_vertex_data()
{
	sgl::model::use_vertex_array();
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(GLfloat), &(this->vertices[0]), GL_STATIC_DRAW);
	sgl::model::restore_vertex_array();
}

void sgl::model::add_vertex_attrib(GLuint count, GLuint max_count)
{
	sgl::model::use_vertex_array();
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	
	glVertexAttribPointer(this->attrib_index, count, GL_FLOAT, GL_FALSE, max_count * sizeof(GLfloat), (void *)(this->attrib_offset * sizeof(GLfloat)));
	glEnableVertexAttribArray(this->attrib_index);
	sgl::model::restore_vertex_array();
	
	this->attrib_index++;
	this->attrib_offset += count;
}

void sgl::model::add_vertex_attribs(std::vector<GLuint> sizes)
{
	GLuint stride = 0;
	for (size_t i = 0; i < sizes.size(); ++i) {
		stride += sizes.at(i);
	}
	
	for (size_t i = 0; i < sizes.size(); ++i)
		this->add_vertex_attrib(sizes.at(i), stride);
}

void sgl::model::use_vertex_array()
{
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &this->prev_vertex_array);
	glBindVertexArray(this->vertex_array);
}

void sgl::model::restore_vertex_array()
{
	glBindVertexArray(this->prev_vertex_array);
}

void sgl::model::load(std::string obj_path)
{
	/* load object file and fill sgl_model data */
	sgl::modelloader loaded(obj_path);
	this->vertices = loaded.pack({sgl::modelloader::PACK_VERTEX, sgl::modelloader::PACK_NORMAL, sgl::modelloader::PACK_UV});;
	
	this->init_vertex_data();
	this->add_vertex_attribs({3, 3, 2}); /* vec3 vertex, vec3 normal, vec2 texcoord */
}

void sgl::model::render()
{
	/* store current vertex array object */
	sgl::model::use_vertex_array();
	
	glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
	
	/* restore previous vao */
	sgl::model::restore_vertex_array();
}

