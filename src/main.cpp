#include <GL/gl3w.h>
#include "simgl.hpp"
#include <string>
#include <imgui/imgui.h>

ImVec2 imvec2_sub(ImVec2 a, ImVec2 b) {
	ImVec2 c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return c;
}

int main(int argc, char *argv[]) {
	sgl::window_context wctx;
	wctx.resizable = false;
	wctx.width = 800;
	wctx.height = 600;
	wctx.dear_imgui = true;

	sgl::window window(wctx);
	window.on_resize([](sgl::window &, int w, int h) { glViewport(0, 0, w, h); });
	
	sgl::mesh cube("assets/cube.obj", {3, 3, 2});

	sgl::mesh m("assets/well.obj", {3, 3, 2});
	sgl::shader s;
	s.load_from_memory(R"(
		#version 330 core
	
		uniform mat4 uProjection;
		uniform mat4 uView;
		uniform mat4 uModel;
		
		layout (location = 0) in vec3 aPosition;
		
		void main() {
			gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
		}
	)", sgl::shader::VERTEX);
	
	s.load_from_memory(R"(
		#version 330 core
		
		void main() {
		}
	)", sgl::shader::FRAGMENT);
	s.compile();
	s.link();
	
	sgl::shader normal_shader;
	normal_shader.load_from_memory(R"(
		#version 330 core

		uniform mat4 uLightTransform;
		uniform mat4 uProjection;
		uniform mat4 uView;
		uniform mat4 uModel;
		
		layout (location = 0) in vec3 aPosition;
		layout (location = 1) in vec3 aNormal;
		layout (location = 2) in vec2 aTexCoord;

		out vec3 vPosition;
		out vec4 vShadowmapCoord;
		out vec2 vTexCoord;

		void main() {
			vTexCoord = aTexCoord;
			vPosition = vec3(uModel * vec4(aPosition, 1.0));
			vShadowmapCoord = uLightTransform * uModel * vec4(aPosition, 1.0);
			gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
		}

	)", sgl::shader::VERTEX);
	normal_shader.load_from_memory(R"(
		#version 330 core
		
		uniform float uTextureTile;
		uniform sampler2D uDiffuse;
		uniform sampler2DShadow uShadowmap;
		
		in vec3 vPosition;
		in vec4 vShadowmapCoord;
		in vec2 vTexCoord;

		out vec4 FragColor;

		void main() {
			float shadow_bias = 0.000075;
			vec3 shadowcoord = 0.5 + 0.5 * vShadowmapCoord.xyz / vShadowmapCoord.w;
			shadowcoord.z -= shadow_bias;
			//float shadow = texture(uShadowmap, shadowcoord);
			
			float shadow = 0.0;
			int krad = 2;
			vec2 texelsize = 1.0 / textureSize(uShadowmap, 0);
			for (int x = -krad; x <= krad; ++x) {
				for (int y = -krad; y <= krad; ++y) {
					float pcfDepth = texture(uShadowmap, shadowcoord.xyz + vec3(x, y, 0.0) * vec3(texelsize, 0.0));
					shadow += pcfDepth;
				}
			}
			shadow /= (krad * 2.0 + 1.0) * (krad * 2.0 + 1.0);
			
			FragColor = vec4(texture(uDiffuse, vTexCoord * uTextureTile).rgb * max(shadow, 0.175), 1.0);
		}

	)", sgl::shader::FRAGMENT);
	normal_shader.compile();
	normal_shader.link();

	sgl::texture t("assets/well.jpg");
	sgl::texture tfloor("assets/paving.png");
	
	glm::mat4 uProj;
	glm::mat4 uView = glm::mat4(1.0f);
	glm::mat4 uModel = glm::mat4(1.0f);
	
	uView = glm::translate(uView, glm::vec3(0.0f, -1.0f, -6.0f));
	glm::vec3 lightpos(0.0f, 8.0f, -15.0f);
	sgl::framebuffer fb;
	fb.load_depthbuffer(1024, 1024);

	sgl::mesh floor("assets/grid.obj", {3, 3, 2});
	
	sgl::camera cam;
	cam.set_speed(0.1f);

	glEnable(GL_DEPTH_TEST);
	window.on_update([&](sgl::window &) {
		/* update */
		if (sgl::input::get_key(GLFW_KEY_A)) cam.move(glm::vec3(-1.0f,  0.0f,  0.0f));
		if (sgl::input::get_key(GLFW_KEY_D)) cam.move(glm::vec3( 1.0f,  0.0f,  0.0f));
		if (sgl::input::get_key(GLFW_KEY_W)) cam.move(glm::vec3( 0.0f,  0.0f,  1.0f));
		if (sgl::input::get_key(GLFW_KEY_S)) cam.move(glm::vec3( 0.0f,  0.0f, -1.0f));
		if (sgl::input::get_key(GLFW_KEY_SPACE)) cam.move(glm::vec3( 0.0f, 1.0f,  0.0f));
		if (sgl::input::get_key(GLFW_KEY_LEFT_SHIFT)) cam.move(glm::vec3( 0.0f, -1.0f,  0.0f));
		if (sgl::input::get_key(GLFW_KEY_LEFT)) cam.rotate(-1.0f,  0.0f);
		if (sgl::input::get_key(GLFW_KEY_RIGHT)) cam.rotate( 1.0f,  0.0f);
		if (sgl::input::get_key(GLFW_KEY_UP)) cam.rotate( 0.0f,  1.0f);
		if (sgl::input::get_key(GLFW_KEY_DOWN)) cam.rotate( 0.0f, -1.0f);
		
		static float time = 0.0f;
		time += glm::radians(0.001f);
		float lx = lightpos.x;
		float lz = lightpos.z;
		
		//lightpos.x = cos(time) * lx - sin(time) * lz;
		//lightpos.z = sin(time) * lx + cos(time) * lz;
		
		/* render */
		glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 light_proj = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
		glm::mat4 light_transform = glm::lookAt(
			lightpos,
			glm::vec3(0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		uProj = glm::perspective(glm::radians(45.0f), window.aspect(), 0.1f, 1000.0f);

		ImGui::Begin("Shadow Camera");
			ImGui::DragFloat3("Light Pos", &lightpos[0]);
			//ImGui::Image((void *)(GLuint)fb, imvec2_sub(ImGui::GetWindowSize(), ImVec2(16.0f, 48.0f)));
		ImGui::End();
		
		/* render scene to shadowmap */
		fb.bind();
			glCullFace(GL_FRONT);
			glClear(GL_DEPTH_BUFFER_BIT);
			s.use();
			s["uProjection"] = light_proj;
			s["uView"] = light_transform;
			s["uModel"] = uModel;
			t.bind(0);
			m.render();
			s["uModel"] = glm::scale(glm::mat4(1.0f), glm::vec3(15.0f));
			floor.render();
			glCullFace(GL_BACK);
		fb.unbind();
		

		/* render scene */
		normal_shader["uDiffuse"] = 0;
		normal_shader["uShadowmap"] = 1;

		normal_shader["uLightTransform"] = light_proj * light_transform;
		normal_shader["uProjection"] = uProj;
		normal_shader["uView"] = cam.get_view();
		normal_shader["uModel"] = uModel;
		
		normal_shader.use();
		t.bind(0);
		fb.bind_texture(1);
		normal_shader["uTextureTile"] = 1.0f;
		m.render();
		normal_shader["uModel"] = glm::scale(glm::mat4(1.0f), glm::vec3(15.0f));
		normal_shader["uTextureTile"] = 4.0f;
		tfloor.bind(0);
		floor.render();
		normal_shader["uModel"] = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)), lightpos);
		cube.render();
		t.unbind();
		fb.unbind_texture();

	});

	return 0;
}

