#include <iostream>
#include <vector>

#include <GL/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <imgui/imgui.h>

#include "sgl_window.hpp"
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
		ImGui::Begin("Path");
			ImGui::Text("Path Index: #%zd", node_index);
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
				
				ImGui::Text("Distance to Target: %.2f", glm::distance(camera.pos, target));
				ImGui::Text("Target Dir: %.2f, %.2f, %.2f", target_dir.x, target_dir.y, target_dir.z);
				ImGui::Text("Speed: %.2f", glm::length(target_dir));
				ImGui::Text("Local completed path: %.2f%%", completed);
				
				if (glm::distance(camera.pos, target) < speed)
					speed = glm::distance(camera.pos, target);
				
				if (glm::distance(camera.pos, target) <= speed) {
					node_index++;
					if (node_index >= this->nodes.size())
						node_index = -1;
				}
			}
		ImGui::End();
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

void render_window(sgl::window &window, sgl::shader &tshader, float *render_dist)
{
	ImGui::Begin("Render Settings");
		static bool _wireframe_enabled = false;
		static bool _render_depthbuffer = false;
		
		if (ImGui::SliderFloat("Render Distance", render_dist, 1.0f, 1000.0f)) {
			tshader["z_far"] = *render_dist;
		}
		
		ImGui::Separator();
		
		if (ImGui::Checkbox("Wireframe", &_wireframe_enabled)) {
			window.render_wireframe(_wireframe_enabled);
		}
		if (ImGui::Checkbox("Depthbuffer", &_render_depthbuffer)) {
			tshader["render_depthbuffer"] = _render_depthbuffer;
		}
	ImGui::End();
}

#if defined(_WIN32) && !defined(_DEBUG)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char *argv[]) {
#endif
	
	sgl::window window(800, 600);
	
	window.on_resize([](sgl::window &, int w, int h){ glViewport(0, 0, w, h); });
	
	sgl::joystick controller(0);
	
	/* matrices */
	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

	sgl::camera camera(glm::vec3(0.0f, 1.5f, 0.0f));
	glm::mat4 projection;
	glm::mat4 MVP;
	
	/* shader */
	sgl::shader tshader("assets/vert.glsl", "assets/frag.glsl");
	GLint uMVP = tshader["MVP"];
	tshader["z_far"] = 350.0f;
	
	/* model */
	sgl::mesh world_mesh("assets/world.obj");
	sgl::texture world_tex("assets/world.png");
	tshader["teximage"] = 0;
	
	/* opengl */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	ImGui::StyleColorsLight();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	
	CameraPath cam_path(camera);
	
	window.on_update([&](sgl::window &) {
		/* inputs */
		static float render_distance = 350.0f;
		controller_window(controller);
		render_window(window, tshader, &render_distance);
		
		ImGui::Begin("Camera");
			float rot[] = {camera.get_yaw(), camera.get_pitch()};
			
			ImGui::DragFloat3("Position", &camera.pos[0], 0.2f);
			if (ImGui::DragFloat2("Rotation", rot, 0.5f)) {
				rot[0] = fmodf(rot[0], 360.0f);
				rot[1] = fmodf(rot[1], 360.0f);
				camera.set_rotation(rot[0], rot[1]);
			}
			
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
		
		/* recalculate matrices */
		float c_l2 = (controller.get_axis(2) * 0.5 + 0.5) * -1.0f;
		float c_r2 = (controller.get_axis(5) * 0.5 + 0.5) *  1.0f;
		camera.move(glm::vec3(controller.get_axis(0), c_l2 + c_r2, controller.get_axis(1) * -1.0f));
		camera.rotate(controller.get_axis(3) * 1.25f, controller.get_axis(4) * -1.0f, 1.75f);
		projection = glm::perspective(45.0f, (float)window.width / (float)window.height, 0.1f, render_distance);
		MVP = projection * camera.get_view() * model;
		
		/* update shader uniform mvp */
		tshader["viewport"] = glm::vec2(window.width, window.height);
		tshader["projection"] = projection;
		tshader["model"] = model;
		tshader[uMVP] = MVP;
		
		/* render code */
		glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(tshader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, world_tex);
		world_mesh.render();
		glUseProgram(0);
		
	});
	
	return 0;
}

