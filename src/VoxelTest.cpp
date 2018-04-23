#include <iostream>
#include <vector>

#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sgl_window.hpp"
#include "sgl_camera.hpp"
#include "sgl_texture.hpp"
#include "sgl_mesh.hpp"
#include "sgl_shader.hpp"
#include "sgl_input.hpp"

#define CONCATE_(X,Y) X##Y
#define CONCATE(X,Y) CONCATE_(X,Y)
#define UNIQUE(NAME) CONCATE(NAME, __LINE__)

struct Static_ 
{
	template<typename T> Static_ (T only_once) { only_once(); }
	~Static_ () {}  // to counter "warning: unused variable"
};
// `UNIQUE` macro required if we expect multiple `static` blocks in function
#define STATIC static Static_ UNIQUE(block) = [&]() -> void

class Cube {

public:
	glm::vec3 pos;
	glm::vec3 color;

	Cube(int x, int y, int z) {
		this->pos = glm::vec3((float)x, (float)y, (float)z);
		this->color = glm::vec3(1.0f);
	}

	Cube *set_color(float r, float g, float b) {
		this->color.r = r;
		this->color.g = g;
		this->color.b = b;

		return this;
	}
};


int main(int argc, char *argv[]) {
	sgl::window_context wctx;
	wctx.samples = 16;
	sgl::window window(wctx);
	
	window.on_resize([](sgl::window &, int w, int h) { glViewport(0, 0, w, h); });
	
	sgl::mesh mesh("assets/cube.obj", {3, 3, 2});
	
	sgl::shader shader;
	shader.load_from_memory(R"(
		#version 330 core
		
		uniform vec3 LightDir;
		uniform vec3 mColor;
		uniform mat4 mModel;
		uniform mat4 mView;
		uniform mat4 mProjection;

		layout (location = 0) in vec3 vPos;
		layout (location = 1) in vec3 vNorm;
		layout (location = 2) in vec2 vTexCoord;

		out vec4 Color;

		void main() {
			vec3 normal = normalize(transpose(inverse(mat3(mModel))) * vNorm);
			float light = dot(normal, normalize(LightDir));

			Color = vec4(vec3(max(light, 0.1125) * mColor), 1.0);
			gl_Position = mProjection * mView * mModel * vec4(vPos, 1.0);
		}
	)", sgl::shader::VERTEX);

	shader.load_from_memory(R"(
		#version 330 core
		
		in vec4 Color;
		out vec4 FragColor;
		
		void main() {
			FragColor = Color;
		}
	)", sgl::shader::FRAGMENT);

	shader.compile(sgl::shader::VERTEX);
	shader.compile(sgl::shader::FRAGMENT);
	shader.link();
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.05f));
	
	shader["LightDir"] = glm::vec3(0.2f, -1.0f, -0.2f);
	
	std::vector<Cube> cubes;
	
	float r = 8.0f;
	for (float x = -r; x <= r; ++x) {
		for (float y = -r; y <= r; ++y) {
			for (float z = -r; z <= r; ++z) {
				float sq = sqrt(x*x + y*y + z*z);
				if (sq >= r - 0.75f && sq < r)
					cubes.push_back(Cube(x, y, z));
			}
		}
	}
	
	sgl::mesh water_mesh(std::vector<GLuint>{3, 3});
	water_mesh.add_vertex({-1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f});
	water_mesh.add_vertex({ 0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f});
	water_mesh.add_vertex({ 0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f});
	water_mesh.add_vertex({ 1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f});
	water_mesh.load();
	
	sgl::shader water_shader;
	water_shader.load_from_memory(R"(
		#version 330 core
		
		uniform mat4 mModel;
		uniform mat4 mView;
		uniform mat4 mProj;
		uniform vec3 mColor;
		
		layout (location = 0) in vec3 vPos;
		layout (location = 1) in vec3 vNorm;
		
		out vec3 fColor;
		
		void main() {
			//vec3 normal = normalize(transpose(inverse(mat3(mModel))) * vNorm);
			fColor = vec3(0.375, 0.35, 0.75);
			gl_Position = mProj * mView * mModel * vec4(vPos, 1.0);
		}
	)", sgl::shader::VERTEX);
	
	water_shader.load_from_memory(R"(
		#version 330 core
		
		in vec3 fColor;
		out vec4 FragColor;

		void main() {
			FragColor = vec4(fColor, 1.0);
		}

	)", sgl::shader::FRAGMENT);
	water_shader.compile(sgl::shader::VERTEX);
	water_shader.compile(sgl::shader::FRAGMENT);
	water_shader.link();

	glm::mat4 mView(1.0f);
	mView = glm::rotate(mView, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mView = glm::translate(mView, glm::vec3(0.0f, -1.5f, -5.0f));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	window.on_update([&](sgl::window &) {
		glClearColor(0.6f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if (sgl::input::get_key(GLFW_KEY_SPACE))
			mView = glm::rotate(mView, glm::radians(3.25f), glm::vec3(0.0f, 1.0f, 0.0f));

		float aspect = (float)window.width / (float)window.height;
		glm::mat4 mProj = glm::ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, -20.0f, 20.0f);
		//glm::mat4 mProj = glm::perspective(window.width / (float)window.height, glm::radians(45.0f), 0.1f, 100.0f);
		shader["mProjection"] = mProj;
		shader["mView"] = mView;
		
		/* cubes */
		/*
		shader.use();
		for (auto &cube : cubes) {
			glm::mat4 cmodel = glm::translate(model, cube.pos * 2.0f);
			shader["mModel"] = cmodel;
			shader["mColor"] = cube.color;
			mesh.render();
		}
		*/

		/* water */
		static glm::mat4 mModel(1.0f);

		water_shader["mView"] = mView;
		water_shader["mProj"] = mProj;
		water_shader["mModel"] = mModel;
		water_shader["mColor"] = glm::vec3(0.0f, 0.0f, 1.0f);

		water_shader.use();
		water_mesh.render(GL_TRIANGLE_STRIP);
	});

	return 0;
}
