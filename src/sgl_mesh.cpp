#include "sgl_mesh.hpp"

sgl::mesh::mesh(std::vector<GLuint> layout)
{
	glGenVertexArrays(1, &this->vertex_array);
	glGenBuffers(1, &this->vertex_buffer);
	
	this->vertex_layout = layout;
}

sgl::mesh::mesh(std::string obj_path, std::vector<GLuint> layout)
	: mesh(layout)
{
	this->load(obj_path);
}


sgl::mesh::~mesh()
{
	glDeleteVertexArrays(1, &this->vertex_array);
	glDeleteBuffers(1, &this->vertex_buffer);
	glDeleteBuffers(1, &this->index_buffer);
	
	for (GLuint &i : this->instance_data_buffers) {
		glDeleteBuffers(1, &i);
	}
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

void sgl::mesh::add_vertex_attrib(GLuint count, GLuint max_count, size_t sizeof_data)
{
	this->use_vertex_array();
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	
	glVertexAttribPointer(this->attrib_index, count, GL_FLOAT, GL_FALSE, max_count * sizeof_data, (void *)(this->attrib_stride));
	glEnableVertexAttribArray(this->attrib_index);
	this->restore_vertex_array();
	
	this->attrib_index++;
	this->attrib_stride += count * sizeof_data;
}

void sgl::mesh::add_vertex_attribs(std::vector<GLuint> sizes, size_t sizeof_data)
{
	GLuint stride = 0;
	for (size_t i = 0; i < sizes.size(); ++i) {
		stride += sizes.at(i);
	}
	
	for (size_t i = 0; i < sizes.size(); ++i)
		this->add_vertex_attrib(sizes.at(i), stride, sizeof_data);
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
void sgl::mesh::set_indices(std::vector<GLuint> indices)
{
	glGenBuffers(1, &this->index_buffer);
	this->indices = indices.size();

	this->use_vertex_array();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	this->restore_vertex_array();
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

size_t sgl::mesh::add_instance_buffer(std::vector<GLuint> layout, bool combine_data)
{
	size_t buffer_i = this->instance_data_buffers.size();
	this->instance_data_buffers.push_back(0);
	GLuint &buffer = this->instance_data_buffers.at(buffer_i);
	
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	
	this->use_vertex_array();
	
	GLuint layout_sum = 0;
	for (GLuint v : layout)
		layout_sum += v;
	
	this->instance_data_block_sizes.push_back(layout_sum);


	for (size_t i = 0; i < layout.size(); ++i) {
		GLuint block_size = (combine_data ? layout_sum : layout[i]);
		glEnableVertexAttribArray(this->attrib_index);
		glVertexAttribDivisor(this->attrib_index, 1);
		glVertexAttribPointer(this->attrib_index, layout[i], GL_FLOAT, GL_FALSE, block_size * sizeof(GLfloat), (void *)(i * (layout[i] * sizeof(GLfloat))));

		this->attrib_index++;
	}

	this->restore_vertex_array();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return buffer_i;
}

void sgl::mesh::update_instance_buffer(size_t instance_buffer_index, GLvoid *data, size_t count)
{
	glBindBuffer(GL_ARRAY_BUFFER, this->instance_data_buffers.at(instance_buffer_index));
	glBufferData(GL_ARRAY_BUFFER, count * this->instance_data_block_sizes.at(instance_buffer_index) * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void sgl::mesh::render(GLenum draw_mode)
{
	/* store current vertex array object */
	this->use_vertex_array();
	
	glDrawArrays(draw_mode, 0, this->vertices.size());
	
	/* restore previous vao */
	this->restore_vertex_array();
}
void sgl::mesh::render_indexed(GLenum draw_mode, GLuint count)
{
	if (count == 0) count = this->indices;
	this->use_vertex_array();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_buffer);
	glDrawElements(draw_mode, count, GL_UNSIGNED_INT, (void *)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	this->restore_vertex_array();
}

void sgl::mesh::render_instanced(GLuint count, GLenum draw_mode)
{
	/* store current vertex array object */
	this->use_vertex_array();
	
	glDrawArraysInstanced(draw_mode, 0, this->vertices.size(), count);
	
	/* restore previous vao */
	this->restore_vertex_array();
}

