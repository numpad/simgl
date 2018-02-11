#ifndef SGL_MODELLOADER_HPP
#define SGL_MODELLOADER_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <string.h>

#define sgl_model_str_startswith(str, startswith) (strncmp(str, startswith, strlen(startswith)) == 0)

namespace sgl {
	class modelloader {

	public:
		enum pack_type {
			PACK_VERTEX, PACK_NORMAL, PACK_UV
		};

		std::vector<unsigned int> vertex_indices, normal_indices, uv_indices;
		std::vector<glm::vec3> vertices,
			normals;
		std::vector<glm::vec2> uvs;

		modelloader();
		modelloader(std::string obj_path);

		bool load_file(std::string obj_path);

		std::vector<float> pack(std::vector<sgl::modelloader::pack_type> order);

	private:
		bool parse_line_vertex(std::string &);
		bool parse_line_normal(std::string &);
		bool parse_line_uv(std::string &);
		bool parse_line_face(std::string &);

		void pack_element(std::vector<float> &into, size_t index, sgl::modelloader::pack_type packtype);

	};
}

#endif
