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
    glm::vec3 pos;

    camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = 0.0f, float pitch = 0.0f, bool constrain_pitch = true, bool fly_mode = true);

    float get_yaw();
    float get_pitch();

    void set_pos(glm::vec3 pos);

    void move(glm::vec3 dir, float speed = 1.0f);

    void rotate(float yaw, float pitch, float sensitivity = 1.0f);
    void set_rotation(float yaw, float pitch);

    void set_flying(bool enable_flight = true);

    glm::mat4 get_view();
};
}

#endif

