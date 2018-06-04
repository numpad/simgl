#include <GL/gl3w.h>
#include "simgl.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>

int main(int argc, char *argv[]) {
	
	sgl::window_context wctx;
	wctx.width = 800;
	wctx.height = 600;
	wctx.resizable = false;
	wctx.dear_imgui = true;
	sgl::window window(wctx);
	window.on_resize([](sgl::window &, int w, int h) { glViewport(0, 0, w, h); });
	
	sgl::mesh screen_rect(std::vector<GLuint>{2, 2});
	screen_rect.add_vertex({-1.0f, -1.0f, 	0.0f, 1.0f});
	screen_rect.add_vertex({ 1.0f, -1.0f, 	1.0f, 1.0f});
	screen_rect.add_vertex({-1.0f,  1.0f, 	0.0f, 0.0f});
	screen_rect.add_vertex({ 1.0f,  1.0f, 	1.0f, 0.0f});
	screen_rect.set_indices({0, 1, 2, 1, 3, 2});
	screen_rect.load();
	
	sgl::texture world_texture("assets/floor.jpg");
	
	sgl::shader screen_shader;
	screen_shader.load_from_memory(R"(
		#version 330 core
		uniform mat4 uView;
		uniform mat4 uProjection;
		layout (location = 0) in vec2 vPosition;
		layout (location = 1) in vec2 vTexCoord;
		out vec2 fTexCoord;
		void main() {
			fTexCoord = vTexCoord;
			gl_Position = uProjection * uView * vec4(vPosition, 0.0, 1.0);
		}
	)", sgl::shader::VERTEX);
	screen_shader.load_from_memory(R"(
		#version 330 core
		uniform vec2 uWorldRepeat;
		uniform sampler2D uWorldTexture;
		uniform sampler2D uMap;
		in vec2 fPosition;
		in vec2 fTexCoord;
		out vec4 FragColor;
		void main() {
			vec4 pixel = texture(uWorldTexture, fTexCoord * uWorldRepeat);
			vec4 map = texture(uMap, fTexCoord);
			float alpha = 1.0;
			
			if (map.a < 0.1)
				alpha = 0.0;
			
			FragColor = vec4(pixel.rgb, map.a);
		}
	)", sgl::shader::FRAGMENT);
	screen_shader.compile();
	screen_shader.link();

	sgl::framebuffer map_data("assets/worms_map.png");
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	window.on_update([&](sgl::window &) {
		glClearColor(0.4f, 0.5f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		/* imgui */
		static float _repeat = 1.65f,
		             _scale = 1.0f;
		static glm::vec3 _pos;
		ImGui::Begin("World");
			ImGui::DragFloat3("Camera", &_pos[0], 0.01f);
			ImGui::SliderFloat("Repeat", &_repeat, 0.0f, 10.0f);
			ImGui::SliderFloat("Scale", &_scale, 0.1f, 10.0f);
		ImGui::End();

		float aspect = window.aspect();
		glm::mat4 uProj = glm::ortho(
			-1.0f,
			 1.0f,
			 1.0f / aspect,
			-1.0f / aspect,
			1.0f, 0.0f
		);
		
		/* input */
		glm::vec3 dir(0.0f);
		const float speed = 0.045f;
		if (sgl::input::get_key(GLFW_KEY_LEFT))
			dir.x += speed;
		if (sgl::input::get_key(GLFW_KEY_RIGHT))
			dir.x -= speed;
		if (sgl::input::get_key(GLFW_KEY_UP))
			dir.y += speed;
		if (sgl::input::get_key(GLFW_KEY_DOWN))
			dir.y -= speed;
		_pos += dir;
		
		static float _sx = 1.0f;
		ImGui::Begin("Mouse");
			ImGui::DragFloat("Scale X", &_sx);
		ImGui::End();

		glm::vec2 mouse;
		sgl::input::get_mouse(mouse.x, mouse.y);
		mouse.x = mouse.x * _sx;
		mouse.y = (window.height - mouse.y);

		glBindTexture(GL_TEXTURE_2D, (GLuint)map_data);
			int s = 10;
			GLfloat *pixels = new GLfloat[s * s * 4];
			for (int i = 0; i < s * s * 4; ++i) {
				pixels[i] = 1.0f;
			}
			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				mouse.x, mouse.y,
				s, s,
				GL_RGBA,
				GL_FLOAT,
				pixels
			);
			delete pixels;
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 uView = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		uView = glm::scale(uView, glm::vec3(_scale));
		uView = glm::translate(uView, _pos);
		

		screen_shader.use();
		world_texture.bind(0);
		map_data.bind_texture(1);
		screen_shader["uWorldRepeat"] = glm::vec2(_repeat);
		screen_shader["uWorldTexture"] = 0;
		screen_shader["uMap"] = 1;
		screen_shader["uProjection"] = uProj;
		screen_shader["uView"] = uView;
		screen_rect.render_indexed();
		world_texture.unbind();
		
	});

	return 0;
}

