#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sgl_window.hpp"
#include "sgl_window_context.hpp"
#include "sgl_shader.hpp"
#include "sgl_texture.hpp"
#include "sgl_mesh.hpp"

#define POINT(x,y) (x),(y)
#define COLOR(x,y,z) (x),(y),(z)

int main(int argc, char *argv[]) {
	sgl::window_context wctx;
	wctx.resizable = false;
	
	sgl::window window(wctx);
	window.on_resize([](sgl::window &, int w, int h) { glViewport(0, 0, w, h); });
	
	sgl::mesh data(std::vector<GLuint>{2, 3, 1});
	data.add_vertex({
		POINT(-0.5f, 0.0f),
		COLOR(1.0f, 0.0f, 0.0f),
		glm::radians(20.0f)
	});
	
	data.add_vertex({
		POINT(0.5f, 0.0f),
		COLOR(0.0f, 1.0f, 0.0f),
		glm::radians(130.0f)
	});
	
	data.add_vertex({
		POINT(0.0f, 0.5f),
		COLOR(0.0f, 0.0f, 1.0f),
		glm::radians(-45.0f)
	});
	data.load();
	
	sgl::shader shader;
	shader.load_from_memory(
		"#version 330 core\n"
		
		"layout (location = 0) in vec2 Position;\n"
		"layout (location = 1) in vec3 Color;\n"
		"layout (location = 2) in float Angle;\n"
		
		"out VS_OUT {\n"
		"  vec3 VertexColor;\n"
		"  float Angle;\n"
		"} vs_out;\n"

		"void main() {\n"
		"  vs_out.VertexColor = Color;\n"
		"  vs_out.Angle = Angle;\n"
		"  gl_Position = vec4(Position, 0.0, 1.0);\n"
		"}\n"
	, sgl::shader::VERTEX);

	shader.load_from_memory(
		"#version 330 core\n"

		"uniform float uTime;\n"

		"layout (points) in;\n"
		"layout (triangle_strip, max_vertices = 3) out;\n"
		
		"in VS_OUT {\n"
		"  vec3 VertexColor;\n"
		"  float Angle;\n"
		"} gs_in[];\n"

		"out vec3 Color;\n"
		
		"vec4 rotate_by(vec4 v, float a) {\n"
		"  float cs = cos(a),\n"
		"        sn = sin(a);\n"
		"  float px = v.x * cs - v.y * sn;\n"
		"  float py = v.x * sn + v.y * cs;\n"
		"  return vec4(\n"
		"    px,\n"
		"    py,\n"
		"    v.z,\n"
		"    v.w\n"
		"  );"
		"}\n"

		"void main() {\n"
		"  Color = gs_in[0].VertexColor;\n"
		"  gl_Position = gl_in[0].gl_Position + rotate_by(vec4(-0.1, 0.1, 0.0, 0.0), gs_in[0].Angle + uTime);\n"
		"  EmitVertex();\n"
		"  gl_Position = gl_in[0].gl_Position + rotate_by(vec4( 0.1, 0.1, 0.0, 0.0), gs_in[0].Angle + uTime);\n"
		"  EmitVertex();\n"
		"  gl_Position = gl_in[0].gl_Position + rotate_by(vec4( 0.0, -0.1, 0.0, 0.0), gs_in[0].Angle + uTime);\n"
		"  EmitVertex();\n"
		"  EndPrimitive();\n"
		"}\n"
	, sgl::shader::GEOMETRY);

	shader.load_from_memory(
		"#version 330 core\n"

		"in vec3 Color;\n"

		"out vec4 FragColor;\n"

		"void main() {\n"
		"  FragColor = vec4(Color, 1.0);\n"
		"}\n"
	, sgl::shader::FRAGMENT);
	
	shader.compile(sgl::shader::VERTEX);
	shader.compile(sgl::shader::GEOMETRY);
	shader.compile(sgl::shader::FRAGMENT);
	shader.link();
	
	window.on_update([&](sgl::window &) {
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		shader["uTime"] = (float)glfwGetTime();
		shader.use();
		data.render(GL_POINTS);
	});

}

