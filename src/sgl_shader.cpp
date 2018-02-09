#include "sgl_shader.hpp"

sgl::shader::shader(std::string fname_vert, std::string fname_frag)
{
	this->load(fname_vert, sgl::shader::VERTEX);
	this->load(fname_frag, sgl::shader::FRAGMENT);
	this->compile(sgl::shader::VERTEX);
	this->compile(sgl::shader::FRAGMENT);
	this->link();
}

sgl::shader::~shader()
{
	glDeleteProgram(this->program);
}

std::string sgl::shader::load_file(std::string fname)
{
	std::ifstream infile(fname);
	std::string content{std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
	return content;
}

bool sgl::shader::load(std::string fname, sgl::shader::type type)
{
	std::string content = sgl::shader::load_file(fname);
	const char *src = content.c_str();
	
	const size_t src_len = strlen(src);
	this->shaders_src[type] = new char[src_len + 1];
	strncpy(this->shaders_src[type], src, src_len);
	this->shaders_src[type][src_len] = 0;
	
	return true;
}

bool sgl::shader::compile(sgl::shader::type type)
{
	this->shaders[type] = glCreateShader(sgl::shader::type_to_gl_shader[type]);
	glShaderSource(this->shaders[type], 1, (const GLchar* const*)&(this->shaders_src[type]), NULL);
	glCompileShader(this->shaders[type]);
	
	/* error message */
	const size_t info_log_len = 1024;
	int success;
	char info_log[info_log_len];
	
	glGetShaderiv(this->shaders[type], GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(this->shaders[type], info_log_len, NULL, info_log);
		
		std::cout << "Failed compiling " << sgl::shader::type_to_name[type] << " shader:" << std::endl;
		std::cout << info_log << std::endl;
	}
	
	return success;
}

bool sgl::shader::link()
{
	this->program = glCreateProgram();
	for (size_t i = 0; i < sgl::shader::MAX_TYPES; ++i) {
		glAttachShader(this->program, this->shaders[i]);
	}
	glLinkProgram(this->program);
	
	/* error message */
	const size_t info_log_len = 1024;
	int success;
	char info_log[info_log_len];
	
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->program, info_log_len, NULL, info_log);
		
		std::cout << "Failed linking shaders:" << std::endl;
		std::cout << info_log << std::endl;
		
		return false;
	}
	
	/* free resources */
	for (size_t i = 0; i < sgl::shader::MAX_TYPES; ++i) {
		glDeleteShader(this->shaders[i]);
		delete[] this->shaders_src[i];
	}
	
	return true;
}

GLuint sgl::shader::operator()(void) const
{
	return this->program;
}

GLint sgl::shader::operator[](const char *uniform_name)
{
	return this->get_uniform(uniform_name);
}

GLint sgl::shader::get_uniform(std::string uniform_name)
{
	return this->get_uniform(uniform_name.c_str());
}

GLint sgl::shader::get_uniform(const char *uniform_name)
{
	return glGetUniformLocation(this->program, uniform_name);
}


