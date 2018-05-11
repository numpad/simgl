#ifndef SGL_CAMERA_HPP
#define SGL_CAMERA_HPP

#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sgl {
	class camera {
		glm::vec3 front, right, up;
		glm::vec3 world_up;
		glm::vec3 move_plane;

		float yaw, pitch;
		
		bool constrain_pitch;

		void update_vectors();
		
	public:
		/**
		 * @brief Camera Position.
		 */
		glm::vec3 pos;
		
		/**
		 * @brief Initialize a camera.
		 * 
		 * 
		 *
		 * @param pos Position.
		 * @param yaw Yaw, rotation around the Y-axis..
		 * @param pitch Pitch, rotation around the (local) X-axis.
		 * @param constrain_pitch Clamp the pitch in the range ]-90.0, 90.0[?
		 * @param fly_mode If false, sgl::camera::move() doesn't take sgl::camera#pitch into account but moves on the XZ plane.
		 * 
		 */
		camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = 0.0f, float pitch = 0.0f, bool constrain_pitch = true, bool fly_mode = true);
		
		/**
		 * @brief Get the rotation around the Y-axis.
		 * 
		 * @return Rotation around the Y-axis.
		 */
		float get_yaw();
		/**
		 * @brief Get the rotation around the (local) X-axis.
		 * 
		 * @return Rotation around the (local) X-axis.
		 */
		float get_pitch();
		
		/**
		 * @brief Set the camera position.
		 * 
		 * @param pos Position.
		 */
		void set_pos(glm::vec3 pos);
		
		/**
		 * @brief Move relative to the cameras rotation.
		 * 
		 * +Z will move the camera in the direction it is heading.
		 * +Y will move the camera upwards.
		 * +X will move the camera to the right.
		 */
		void move(glm::vec3 dir, float speed = 1.0f);
		void move_worldspace(glm::vec3 dir, float speed = 1.0f);
		
		void rotate(float yaw, float pitch, float sensitivity = 1.0f);
		void set_rotation(float yaw, float pitch);

		void set_flying(bool enable_flight = true);

		glm::mat4 get_view();
	};
}

#endif

