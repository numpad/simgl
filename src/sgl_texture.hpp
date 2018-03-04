#ifndef SGL_TEXTURE_HPP
#define SGL_TEXTURE_HPP

#include <string>
#include <vector>
#include <GL/gl3w.h>
#include <stb_image.h>

namespace sgl {

	class texture {
		GLuint texture_id;
		GLint _width, _height;

		GLenum type;

	public:
		
		texture(GLenum texture_type = GL_TEXTURE_2D);
		texture(std::string fname, GLenum texture_type = GL_TEXTURE_2D);
		
		texture(const sgl::texture &copy);
		~texture();
		
		bool load(std::string fname);
		bool load_cubemap(std::vector<std::string> fnames);

		operator GLuint() const;

		void bind(GLint value);
		void unbind();
	};

}

#endif

