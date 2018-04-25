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

void generate_grid(sgl::mesh &mesh, GLuint count, GLfloat radius = 1.0f) {
	std::vector<GLuint> indices;

	for (GLuint z = 0; z < count; ++z) {
		for (GLuint x = 0; x < count; ++x) {
			GLuint vID = x + z * count;
			mesh.add_vertex({(x / (float)count) * radius - radius * 0.5f, 0.0f, (z / (float)count) * radius - radius * 0.5f, 0.0f, 1.0f, 0.0f});
			if (z < count - 1 && x < count - 1) {
				indices.insert(indices.end(), {vID, vID + 1, vID + count, vID + 1, vID + 1 + count, vID + count});
			}
		}
	}
	mesh.set_indices(indices);
	mesh.load();
}

int main(int argc, char *argv[]) {
	sgl::window_context wctx;
	wctx.samples = 16;
	sgl::window window(wctx);
	
	window.on_resize([](sgl::window &, int w, int h) { glViewport(0, 0, w, h); });
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.05f));
	
	sgl::mesh water_mesh(std::vector<GLuint>{3, 3});
	generate_grid(water_mesh, 256, 4.0f);

	sgl::shader water_shader;
	water_shader.load_from_memory(R"(
		#version 330 core
		
		uniform mat4 mModel;
		uniform mat4 mView;
		uniform mat4 mProj;
		uniform vec3 mColor;
		uniform float mTime;
		
		uniform float uWaveHeight;
		uniform float uWaveMult;
		uniform float uTimeMult;

		layout (location = 0) in vec3 vPos;
		layout (location = 1) in vec3 vNorm;
		
		out VS_OUT {
			vec3 Color;
			vec3 Normal;
		} vs_out;

		vec2 grid_coords(int side_vertices, float radius) {
			vec2 p;
			p.x = float(gl_VertexID % side_vertices);
			p.y = float(gl_VertexID / side_vertices);
			return (p / float(side_vertices - 1.0)) * radius - radius * 0.5;
		}
		
		void main() {
			vec3 normal = normalize(transpose(inverse(mat3(mModel))) * vNorm);
			float light = max(dot(normal, vec3(0.0, 1.0, 0.0)), 0.1);

			vec3 pos = vec3(vPos.x, vPos.y, vPos.z);
			vec2 coords = grid_coords(256, 4.0);
			
			float time = mTime * uTimeMult;
			pos.y = sin(time + coords.y * uWaveMult) * uWaveHeight
			      + cos(time + coords.x * uWaveMult) * uWaveHeight;

			vs_out.Color = vec3(0.35, 0.375, 0.75) * light;
			vs_out.Normal = normal;
			gl_Position = mProj * mView * mModel * vec4(pos, 1.0);
		}
	)", sgl::shader::VERTEX);
	
	water_shader.load_from_memory(R"(
		#version 330 core
		
		uniform mat4 mModel;

		layout (triangles) in;
		layout (triangle_strip, max_vertices = 3) out;

		in VS_OUT {
			vec3 Color;
			vec3 Normal;
		} gs_in[];

		out vec3 fColor;
		
		vec3 combined_color() {
			return mix(mix(gs_in[0].Color, gs_in[1].Color, 0.5), gs_in[2].Color, 0.5);
		}
		
		vec3 normal() {
			vec3 A = gl_in[0].gl_Position.xyz,
			     B = gl_in[1].gl_Position.xyz,
				 C = gl_in[2].gl_Position.xyz;

			vec3 ba = A - B;
			vec3 bc = C - B;

			return normalize(cross(ba, bc));
		}

		void main() {
			vec3 n = normal();
			float light = max(1.0 - dot(-n, vec3(0.0, 1.0, 0.0)), 0.25);
			fColor = combined_color() * light;
			
			for (int i = 0; i < 3; ++i) {
				gl_Position = gl_in[i].gl_Position;
				EmitVertex();
			}

			EndPrimitive();
		}

	)", sgl::shader::GEOMETRY);

	water_shader.load_from_memory(R"(
		#version 330 core
		
		in vec3 fColor;
		out vec4 FragColor;

		void main() {
			FragColor = vec4(fColor, 1.0);
		}

	)", sgl::shader::FRAGMENT);
	water_shader.compile(sgl::shader::VERTEX);
	water_shader.compile(sgl::shader::GEOMETRY);
	water_shader.compile(sgl::shader::FRAGMENT);
	water_shader.link();
	

	glm::mat4 mView(1.0f);
	mView = glm::rotate(mView, glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mView = glm::translate(mView, glm::vec3(0.0f, -2.5f, -5.0f));
	
	ImGui::StyleColorsLight();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	window.on_update([&](sgl::window &) {
		glClearColor(0.6f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		/* imgui */
		static float uWaveHeight = 0.05f,
		             uWaveMult   = 8.5f,
					 uTimeMult   = 1.5f;
		ImGui::Begin("Water");
			ImGui::DragFloat("Wave Height", &uWaveHeight, 0.0025f);
			ImGui::DragFloat("Wave Mult", &uWaveMult, 0.25f);
			ImGui::DragFloat("Time Mult", &uTimeMult, 0.01f);
		ImGui::End();

		/* input */
		static float lx = 0.0f, ly = 0.0f;
		float mx, my;
		sgl::input::get_mouse(mx, my);
		float dx = lx - mx, dy = ly - my;
		if (sgl::input::get_button(GLFW_MOUSE_BUTTON_LEFT))
			mView = glm::rotate(mView, glm::radians(dx * -0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
		lx = mx;
		ly = my;
		/* update */
		float aspect = (float)window.width / (float)window.height;
		glm::mat4 mProj = glm::ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, -20.0f, 20.0f);
		//glm::mat4 mProj = glm::perspective(window.width / (float)window.height, glm::radians(45.0f), 0.1f, 100.0f);
		

		/* water */
		static glm::mat4 mModel(1.0f);
		
		water_shader["mView"] = mView;
		water_shader["mProj"] = mProj;
		water_shader["mModel"] = mModel;
		water_shader["mColor"] = glm::vec3(0.0f, 0.0f, 1.0f);
		water_shader["mTime"] = (float)glfwGetTime();
		water_shader["uWaveHeight"] = uWaveHeight;
		water_shader["uWaveMult"] = uWaveMult;
		water_shader["uTimeMult"] = uTimeMult;

		water_shader.use();
		glLineWidth(2.0f);
		water_mesh.render_indexed();


	});

	return 0;
}
