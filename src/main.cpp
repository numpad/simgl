#include <iostream>
#include <vector>
#include <cmath>

#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <imgui/imgui.h>

#include "sgl_window.hpp"
#include "sgl_window_context.hpp"
#include "sgl_shader.hpp"
#include "sgl_mesh.hpp"
#include "sgl_texture.hpp"
#include "sgl_joystick.hpp"
#include "sgl_camera.hpp"

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
	ImGui::Begin("Controller");
		ImGui::Columns(2);
		ImGui::Text("Axes:");
		for (int i = 0; i < controller.get_axes_count(); ++i) {
			float v = controller.get_axis(i);
			ImGui::TextColored(ImVec4(1.0 - (v * 0.5 + 0.5), 0.0, v * 0.5 + 0.5, 1.0), "%d: %g\n", i, v);
		}
		ImGui::NextColumn();
		ImGui::Text("Buttons:");
		for (int i = 0; i < controller.get_button_count(); ++i) {
			ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "%d: %d\n", i, controller.get_button(i));
		}
	ImGui::End();
}

void render_window(sgl::window &window, sgl::shader &world_shader, float *render_dist)
{
	ImGui::Begin("Render Settings");
		static bool _wireframe_enabled = false;
		static bool _render_depthbuffer = false;
		
		if (ImGui::SliderFloat("Render Distance", render_dist, 1.0f, 1000.0f)) {
			world_shader["z_far"] = *render_dist;
		}
		
		ImGui::Separator();
		
		if (ImGui::Checkbox("Wireframe", &_wireframe_enabled)) {
			window.render_wireframe(_wireframe_enabled);
		}
		if (ImGui::Checkbox("Depthbuffer", &_render_depthbuffer)) {
			world_shader["render_depthbuffer"] = _render_depthbuffer;
		}
	ImGui::End();
}

#if defined(_WIN32) && !defined(_DEBUG)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char *argv[]) {
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
	
	skybox.load_cubemap({
		"assets/posx.png",
		"assets/negx.png",
		"assets/posy.png",
		"assets/negy.png",
		"assets/posz.png",
		"assets/negz.png",
	});
	
	/* asteroid field */
	sgl::mesh asteroid_mesh("assets/asteroid.obj");
	sgl::texture asteroid_texture("assets/asteroid.png");
	sgl::shader asteroid_shader("assets/instance_vert.glsl", "assets/instance_frag.glsl");
	
	std::vector<glm::mat4> asteroid_models;
	srand(glfwGetTime() + 1273512);
	for (int i = 0; i < 600; ++i) {
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
	}
	
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, asteroid_models.size() * sizeof(glm::mat4), &asteroid_models[0], GL_DYNAMIC_DRAW);
	
	glBindVertexArray(asteroid_mesh.get_vertex_array());

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(0 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(1 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(3 * sizeof(glm::vec4)));
	
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* opengl */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	ImGui::StyleColorsLight();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	
	CameraPath cam_path(camera);
	
	window.on_update([&](sgl::window &) {
		/* inputs */
		static float render_distance = 10000.0f;
		controller_window(controller);
		render_window(window, world_shader, &render_distance);
		
		ImGui::Begin("Camera");
			float rot[] = {camera.get_yaw(), camera.get_pitch()};
			static float camera_move_speed = 1.0f;
			
			ImGui::DragFloat3("Position", &camera.pos[0], 0.2f);
			if (ImGui::DragFloat2("Rotation", rot, 0.5f)) {
				rot[0] = fmodf(rot[0], 360.0f);
				rot[1] = fmodf(rot[1], 360.0f);
				camera.set_rotation(rot[0], rot[1]);
			}
			ImGui::SliderFloat("Speed", &camera_move_speed, 0.1f, 100.0f);
			
			static bool recording = false;
			
			if (ImGui::Button("Add Node")) {
				cam_path.add(CameraNode(camera));
			}
			ImGui::SameLine();
			if (ImGui::Button("Follow Path")) {
				cam_path.goto_start();
				cam_path.start();
			}
			ImGui::SameLine();
			if (ImGui::Button("Go to end")) {
				CameraNode &end = cam_path.nodes.back();
				camera.pos = end.pos;
				camera.set_rotation(end.rotation[0], end.rotation[1]);
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear")) {
				cam_path.nodes.clear();
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Record camera", &recording)) {
				if (cam_path.nodes.size() == 0)
					cam_path.add(CameraNode(camera));
			}
			
			ImGui::Separator();
			
			ImGui::Text("Path Nodes:");
			ImGui::BeginChild("Path", ImVec2(0.0f, -20.0f));
				ImGui::Columns(2);
				for (size_t i = 0; i < cam_path.nodes.size(); ++i) {
					glm::vec3 p = cam_path.nodes.at(i).pos;
					float *r = cam_path.nodes.at(i).rotation;
					
					ImGui::Text("#%d at (%g, %g, %g), (%g, %g)", (int)i, p.x, p.y, p.z, r[0], r[1]);
					ImGui::SameLine();
					ImGui::PushID(i);
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.2f, 0.25f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
					if (ImGui::SmallButton("X")) {
						cam_path.nodes.erase(cam_path.nodes.begin() + i);
						cam_path.calculate_length();
					}
					ImGui::PopStyleColor(3);
					ImGui::PopStyleVar();
					ImGui::PopID();
					
					ImGui::NextColumn();
					
					float d = cam_path.distance(i);
					float pd = d / cam_path.total_length;
					if (d >= 0.0f)
						ImGui::Text("%g (%.2f%%)", d, pd * 100.0f);
					
					ImGui::NextColumn();
				}
			static float cam_speed = 2.0f;
			ImGui::EndChild();
			ImGui::Text("Path Length: %g", cam_path.total_length);
			ImGui::SameLine();
			ImGui::SliderFloat("Speed", &cam_speed, 0.001f, 25.0f);
			
		ImGui::End();

		if (recording) {
			if (glm::distance(cam_path.nodes.back().pos, camera.pos) >= cam_speed)
				cam_path.add(CameraNode(camera));
		}
		
		cam_path.follow(camera, cam_speed);
		
		for (glm::mat4 &mm : asteroid_models) {
			mm = glm::rotate(mm, glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, asteroid_models.size() * sizeof(glm::mat4), &asteroid_models[0], GL_DYNAMIC_DRAW);

		/* recalculate matrices */
		float c_l2 = (controller.get_axis(2) * 0.5 + 0.5) * -1.0f;
		float c_r2 = (controller.get_axis(5) * 0.5 + 0.5) *  1.0f;
		camera.move(glm::vec3(controller.get_axis(0), c_l2 + c_r2, controller.get_axis(1) * -1.0f), camera_move_speed);
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
		glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
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
		
		asteroid_shader.use();
		asteroid_texture.bind(0);
		asteroid_mesh.render_instanced(asteroid_models.size());

		/* world */
		world_tex.bind(0);
		world_shader.use();
		world_mesh.render();
		world_tex.unbind();

	});
	
	return 0;
}

