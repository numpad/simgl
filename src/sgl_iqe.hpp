#ifndef SGL_INTERQUAKE_EXPORT_HPP
#define SGL_INTERQUAKE_EXPORT_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>

#include <GL/gl3w.h>


namespace sgl {

	struct iqe_tag {
		std::string tag;
		std::vector<std::string> params;

		std::vector<sgl::iqe_tag> children;

		iqe_tag(std::string tag_name)
			: tag(tag_name)
		{
		}
		iqe_tag(const sgl::iqe_tag &o)
			: tag(o.tag),
			params(o.params),
			children(o.children)
		{
		}
		iqe_tag(sgl::iqe_tag &&o)
			: tag(o.tag),
			params(o.params),
			children(o.children)
		{
		}

		void push_param(std::string param)
		{
			this->params.push_back(param);
		}

		void push_child(sgl::iqe_tag iqetag)
		{
			this->children.push_back(iqetag);
		}

		sgl::iqe_tag *get_child(std::string tagname)
		{
			for (auto &child : this->children) {
				if (child.tag == tagname)
					return &child;
			}

			return nullptr;
		}

		void print()
		{
			std::cout << this->tag << ": ";
			for (auto &param : this->params) {
				std::cout << param << " ";
			}
			std::cout << std::endl;

			for (auto &child : this->children) {
				std::cout << "  -> ";
				child.print();
			}
		}
	};


	class iqe_loader {
		std::vector<sgl::iqe_tag> tags;

	public:
		
		iqe_loader();
		iqe_loader(std::string filename);
		void parse(std::string filename);
		
		void print();
	};

}

#endif

