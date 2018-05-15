#include <iostream>
#include <vector>
#include <cmath>
#include <map>

#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "sgl_window.hpp"
#include "sgl_window_context.hpp"
#include "sgl_shader.hpp"
#include "sgl_mesh.hpp"
#include "sgl_texture.hpp"
#include "sgl_joystick.hpp"
#include "sgl_camera.hpp"
#include "sgl_timer.hpp"

struct CameraNode {
	glm::vec3 pos;
	float rotation[2];
	
	CameraNode(glm::vec3 wp, float y, float p)
		: pos(wp), rotation{y, p}
	{
	}
	
	CameraNode(sgl::camera &cam)
		: CameraNode(cam.pos, cam.get_yaw(), cam.get_pitch())
	{
	}
	
	float distance_to(CameraNode &other)
	{
		return glm::distance(this->pos, other.pos);
	}
	
};

struct CameraPath {
	sgl::camera &camera;
	std::vector<CameraNode> nodes;
	float total_length;
	size_t node_index;
	
	CameraPath(sgl::camera &cam)
		: camera(cam), total_length(0.0f), node_index(-1)
	{
	}
	
	void calculate_length()
	{
		this->total_length = 0.0f;
		
		for (size_t i = 0; i < this->nodes.size() - 1; ++i) {
			CameraNode curr = this->nodes.at(i);
			CameraNode next = this->nodes.at(i + 1);
			this->total_length += curr.distance_to(next);
		}
	}
	
	float distance(size_t index)
	{
		if (index >= this->nodes.size() - 1)
			return -1.0f;
		
		return this->nodes.at(index).distance_to(this->nodes.at(index + 1));
	}
	
	void add(CameraNode cn)
	{
		this->nodes.push_back(cn);
		this->calculate_length();
	}
	
	void goto_start()
	{
		CameraNode start = this->nodes.at(0);
		camera.set_pos(start.pos);
		camera.set_rotation(start.rotation[0], start.rotation[1]);
	}
	
	void start()
	{
		this->node_index = 1;
	}
	
	void follow(sgl::camera &camera, float speed)
	{
		if (this->node_index > 0 && this->node_index < this->nodes.size()) {
			glm::vec3 target = this->nodes.at(this->node_index).pos;
			float target_yaw   = this->nodes.at(this->node_index).rotation[0],
				  target_pitch = this->nodes.at(this->node_index).rotation[1];
			float last_yaw     = this->nodes.at(this->node_index - 1).rotation[0],
				  last_pitch   = this->nodes.at(this->node_index - 1).rotation[1];
			
			glm::vec3 target_dir = glm::normalize(target - camera.pos);
			camera.move_worldspace(target_dir, speed);
			
			const float dist_to_target = glm::distance(camera.pos, target),
						dist_to_last   = glm::distance(camera.pos, this->nodes.at(this->node_index - 1).pos);
			float completed = dist_to_last / (dist_to_last + dist_to_target);
			
			camera.set_rotation(
				last_yaw + (target_yaw - last_yaw) * completed,
				last_pitch + (target_pitch - last_pitch) * completed
			);
			
			if (glm::distance(camera.pos, target) < speed)
				speed = glm::distance(camera.pos, target);
			
			if (glm::distance(camera.pos, target) <= speed) {
				node_index++;
				if (node_index >= this->nodes.size())
					node_index = -1;
			}
		}
	}
};

void controller_window(sgl::joystick &controller)
{
	
}

void render_window(sgl::window &window, sgl::shader &world_shader, float *render_dist)
{
	
}

void new_asteroid(std::vector<glm::mat4> &asteroid_models, std::vector<glm::vec3> &asteroid_colors)
{
	glm::mat4 mat(1.0f);
	float angle = glm::radians(((float)rand() / (float)RAND_MAX) * 359.0f);
	float len = ((float)rand() / (float)RAND_MAX) * 10000.0f + 90.0;
	float height = ((float)rand() / (float)RAND_MAX) * 1000.0f - 200.0f;
	float size = ((float)rand() / (float)RAND_MAX) * 60.0f + 8.0f;
	
	float rotation = ((float)rand() / (float)RAND_MAX) * 359.0f;
	float rotx = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
	float roty = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
	float rotz = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;

	mat = glm::translate(mat, glm::vec3(cosf(angle) * len, height, sinf(angle) * len));
	mat = glm::scale(mat, glm::vec3(size));
	mat = glm::rotate(mat, glm::radians(rotation), glm::vec3(rotx, roty, rotz));

	asteroid_models.push_back(mat);
	asteroid_colors.push_back(glm::vec3(
		((float)rand() / RAND_MAX),
		((float)rand() / RAND_MAX),
		((float)rand() / RAND_MAX)
	));
}

#if defined(_WIN32) && !defined(_DEBUG)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
#else
int __main(int argc, char *argv[]) {
#endif
	sgl::window_context wctx;
	wctx.resizable = false;
	wctx.fullscreen = true;
	sgl::window window(wctx);

	window.on_resize([](sgl::window &, int w, int h){ glViewport(0, 0, w, h); });
	
	sgl::joystick controller(0);
	
	/* matrices */
	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(50.0f, 30.0f, 50.0f));

	sgl::camera camera(glm::vec3(0.0f, 1.5f, 0.0f));
	glm::mat4 projection;
	glm::mat4 MVP;
	
	/* shader */
	sgl::shader world_shader("assets/vert.glsl", "assets/frag.glsl");
	GLint uMVP = world_shader["MVP"];
	world_shader["z_far"] = 10000.0f;
	
	/* model */
	sgl::mesh world_mesh("assets/island.obj");
	sgl::texture world_tex("assets/island_texture.png");
	world_shader["teximage"] = 0;

	/* skybox */
	sgl::mesh skybox_mesh("assets/cube.obj");
	sgl::shader skybox_shader("assets/skybox_vert.glsl", "assets/skybox_frag.glsl");
	sgl::texture skybox(GL_TEXTURE_CUBE_MAP);
	glm::mat4 skybox_model(1.0f);
	
	sgl::timer::measure([&](){
		skybox.load_cubemap({
			"assets/posx.png",
			"assets/negx.png",
			"assets/posy.png",
			"assets/negy.png",
			"assets/posz.png",
			"assets/negz.png",
		});
	}, "Skybox loading");
	
	/* asteroid field */
	sgl::mesh asteroid_mesh("assets/asteroid.obj");
	sgl::texture asteroid_texture("assets/asteroid.png");
	sgl::texture asteroid_normals("assets/asteroid_normals.png");
	sgl::shader asteroid_shader("assets/instance_vert.glsl", "assets/instance_frag.glsl");
	
	std::vector<glm::mat4> asteroid_models;
	std::vector<glm::vec3> asteroid_colors;
	srand(glfwGetTime() * 1273512);
	for (int i = 0; i < 500; ++i) {
		new_asteroid(asteroid_models, asteroid_colors);
	}
	
	asteroid_mesh.add_instance_buffer({4, 4, 4, 4}, true);
	asteroid_mesh.add_instance_buffer({3});
	
	/* grass */
	sgl::texture grass_texture("assets/grass.png");
	grass_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	grass_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	sgl::mesh grass_mesh("assets/grass.obj");
	sgl::shader grass_shader("assets/grass_vert.glsl", "assets/grass_frag.glsl");
	
	grass_mesh.add_instance_buffer({4, 4, 4, 4}, true);
	
	std::vector<glm::mat4> grass_positions;
	for (size_t i = 0; i < 200; ++i) {
		glm::vec3 pos(
			((float)rand() / (float)RAND_MAX) * 100.0f - 50.0f,
			0.0f,
			((float)rand() / (float)RAND_MAX) * 100.0f - 50.0f
		);
		glm::vec3 scale(((float)rand() / RAND_MAX) * 8.0f + 2.0f);

		glm::mat4 mod(1.0f);
		mod = glm::translate(mod, pos);
		mod = glm::scale(mod, scale);
		grass_positions.push_back(mod);
	}
	grass_mesh.update_instance_buffer(0, &grass_positions[0], grass_positions.size());

	/* opengl */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	CameraPath cam_path(camera);
	
	window.on_update([&](sgl::window &) {
		/* inputs */
		static float render_distance = 10000.0f;
		controller_window(controller);
		render_window(window, world_shader, &render_distance);

		
		for (glm::mat4 &mm : asteroid_models) {
			mm = glm::rotate(mm, glm::radians(0.25f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		asteroid_mesh.update_instance_buffer(0, &asteroid_models[0], asteroid_models.size());
		asteroid_mesh.update_instance_buffer(1, &asteroid_colors[0], asteroid_colors.size());

		/* recalculate matrices */
		if (controller.get_button(4))
			new_asteroid(asteroid_models, asteroid_colors);
		float c_l2 = (controller.get_axis(2) * 0.5 + 0.5) * -1.0f;
		float c_r2 = (controller.get_axis(5) * 0.5 + 0.5) *  1.0f;
		camera.move(glm::vec3(controller.get_axis(0), c_l2 + c_r2, controller.get_axis(1) * -1.0f));
		camera.rotate(controller.get_axis(3) * 1.25f, controller.get_axis(4) * -1.0f, 1.75f);
		projection = glm::perspective(glm::radians(45.0f), (float)window.width / (float)window.height, 0.1f, render_distance);
		MVP = projection * camera.get_view() * model;
		
		/* update shader uniform mvp */
		world_shader["viewport"] = glm::vec2(window.width, window.height);
		world_shader["projection"] = projection;
		world_shader["model"] = model;
		world_shader[uMVP] = MVP;

		skybox_model = glm::rotate(skybox_model, glm::radians(0.01f), glm::vec3(0.1f, 1.0f, 0.4));
		skybox_shader["mProjection"] = projection;
		skybox_shader["mView"] = glm::mat4(glm::mat3(camera.get_view()));
		skybox_shader["mModel"] = skybox_model;
		skybox_shader["Time"] = (float)glfwGetTime();
		
		/* render code */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		/* skybox */
		glDepthMask(GL_FALSE);
		skybox.bind(0);
		skybox_shader.use();
		skybox_mesh.render();
		skybox.unbind();
		glDepthMask(GL_TRUE);
		
		/* asteroid */
		asteroid_shader["mProjection"] = projection;
		asteroid_shader["mView"] = camera.get_view();
		asteroid_shader["TexImage"] = 0;
		asteroid_shader["TexNormal"] = 1;
		
		asteroid_shader.use();
		asteroid_texture.bind(0);
		asteroid_normals.bind(1);
		asteroid_mesh.render_instanced(asteroid_models.size());

		/* world */
		world_tex.bind(0);
		world_shader.use();
		world_mesh.render();
		world_tex.unbind();
		
		/* grass */
		std::map<float, glm::mat4> sorted;
		for (size_t i = 0; i < grass_positions.size(); ++i) {
			glm::vec3 pos = glm::vec3(grass_positions[i][3]);
			float dist = glm::length(camera.pos - pos);
			sorted[dist] = grass_positions[i];
		}
		std::vector<glm::mat4> grass_models_rev;
		
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
			grass_models_rev.push_back(it->second);
		}

		grass_mesh.update_instance_buffer(0, &grass_models_rev[0], grass_models_rev.size());

		grass_shader["mProjection"] = projection;
		grass_shader["mView"] = camera.get_view();

		glEnable(GL_BLEND);
		grass_texture.bind(0);
		grass_shader.use();
		grass_mesh.render_instanced(grass_positions.size());
		glDisable(GL_BLEND);
	});
	
	return 0;
}

