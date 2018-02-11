#include "sgl_modelloader.hpp"

bool sgl::modelloader::parse_line_vertex(std::string &line) {
	std::istringstream iss(line);

	std::string identifier;
	float x, y, z;
	iss >> identifier >> x >> y >> z;

	this->vertices.push_back(glm::vec3(x, y, z));

	return true; /* TODO: Error check */
}

bool sgl::modelloader::parse_line_normal(std::string &line) {
	std::istringstream iss(line);

	std::string identifier;
	float x, y, z;
	iss >> identifier >> x >> y >> z;

	this->normals.push_back(glm::vec3(x, y, z));

	return true; /* TODO: Error check */
}

bool sgl::modelloader::parse_line_uv(std::string &line) {
	std::istringstream iss(line);

	std::string identifier;
	float u, v;
	iss >> identifier >> u >> v;

	this->uvs.push_back(glm::vec2(u, v));

	return true; /* TODO: Error check */
}

bool sgl::modelloader::parse_line_face(std::string &line) {
	std::istringstream iss(line);

	char _; /* dummy, used for '/' */
	std::string identifier;
	int f[3][3];

	/* vertex index, uv index, normal index */
	iss >> identifier >> f[0][0] >> _ >> f[0][1] >> _ >> f[0][2]
		>> f[1][0] >> _ >> f[1][1] >> _ >> f[1][2]
		>> f[2][0] >> _ >> f[2][1] >> _ >> f[2][2];

	for (size_t i = 0; i < 3; ++i) {
		/* wavefront .obj indices start at 1, opengl uses 0 */
		this->vertex_indices.push_back(f[i][0] - 1);
		this->uv_indices.push_back(f[i][1] - 1);
		this->normal_indices.push_back(f[i][2] - 1);
	}

	return true; /* TODO: Error check */
}

sgl::modelloader::modelloader() {

}

sgl::modelloader::modelloader(std::string obj_path) {
	this->load_file(obj_path);
}

bool sgl::modelloader::load_file(std::string obj_path) {
	std::ifstream obj_file(obj_path);

	const char identifier_face[]	= "f ",
		identifier_vertex[]	= "v ",
		identifier_normal[]	= "vn ",
		identifier_uv[]		= "vt ";

	/* for every line in .obj-file */
	for (std::string line; std::getline(obj_file, line); ) {
		if (sgl_model_str_startswith(line.c_str(), identifier_face)) {
			this->parse_line_face(line);
		} else if (sgl_model_str_startswith(line.c_str(), identifier_vertex)) {
			this->parse_line_vertex(line);
		} else if (sgl_model_str_startswith(line.c_str(), identifier_normal)) {
			this->parse_line_normal(line);
		} else if (sgl_model_str_startswith(line.c_str(), identifier_uv)) {
			this->parse_line_uv(line);
		}

	}

	return true;
}

std::vector<float> sgl::modelloader::pack(std::vector<sgl::modelloader::pack_type> order) {
	std::vector<float> data;

	for (size_t i = 0; i < this->vertex_indices.size(); ++i) {
		for (sgl::modelloader::pack_type ptype : order) {
			this->pack_element(data, i, ptype);
		}
	}

	return data;
}

void sgl::modelloader::pack_element(std::vector<float>& into, size_t index, sgl::modelloader::pack_type packtype) {
	switch (packtype) {
	case sgl::modelloader::pack_type::PACK_NORMAL: {
		glm::vec3 n = this->normals.at(this->normal_indices.at(index));
		into.push_back(n.x);
		into.push_back(n.y);
		into.push_back(n.z);
		break;
	}
	case sgl::modelloader::pack_type::PACK_VERTEX: {
		glm::vec3 v = this->vertices.at(this->vertex_indices.at(index));
		into.push_back(v.x);
		into.push_back(v.y);
		into.push_back(v.z);
		break;
	}
	case sgl::modelloader::pack_type::PACK_UV: {
		glm::vec2 t = this->uvs.at(this->uv_indices.at(index));
		into.push_back(t.x);
		into.push_back(t.y);
		break;
	}
	};
}
