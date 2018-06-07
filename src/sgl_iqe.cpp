#include "sgl_iqe.hpp"

void sgl::iqe_loader::parse(std::string filename)
{
	std::ifstream iqe_file(filename);
	if (!iqe_file.is_open()) {
		std::cout << "could not open file '" << filename << "'!" << std::endl;
		return;
	}
	
	std::string line;
	while (std::getline(iqe_file, line)) {
		std::istringstream iss(line);
		
		if (line.size() == 0)
			continue;

		bool is_child = (line.at(0) == '\t' || line.at(0) == ' ');

		std::string tag_name;
		iss >> tag_name;
		
		if (tag_name == "#")
			continue;

		sgl::iqe_tag tag(tag_name);


		std::string param;
		while (!iss.eof()) {
			iss >> param;
			tag.push_param(param);
		}
		
		if (is_child)
			this->tags.back().push_child(tag);
		else
			this->tags.push_back(tag);
	}
	
	iqe_file.close();
}

sgl::iqe_loader::iqe_loader()
{
}

sgl::iqe_loader::iqe_loader(std::string filename)
{
	this->parse(filename);
}

void sgl::iqe_loader::print()
{
	for (auto &tag : this->tags) {
		tag.print();
	}
}

