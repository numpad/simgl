#include "sgl_camera.hpp"

sgl::camera::camera(glm::vec3 pos, float yaw, float pitch, bool constrain_pitch, bool fly_mode) {
    this->pos = pos;
    this->world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->yaw = yaw;
    this->pitch = pitch;
    this->constrain_pitch = constrain_pitch;

    this->set_flying(fly_mode);
    this->update_vectors();
}

float sgl::camera::get_yaw() {
    return this->yaw;
}

float sgl::camera::get_pitch() {
    return this->pitch;
}

/* returns a normalized vector in the direction the camera is pointing */
void sgl::camera::update_vectors() {
    glm::vec3 front(
        cosf(glm::radians(this->pitch)) * cosf(glm::radians(this->yaw)),
        sinf(glm::radians(this->pitch)),
        cosf(glm::radians(this->pitch)) * sinf(glm::radians(this->yaw))
    );
    this->front = glm::normalize(front);

    this->right = glm::normalize(glm::cross(this->front, this->world_up));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

void sgl::camera::set_pos(glm::vec3 pos) {
    this->pos = pos;
}

void sgl::camera::move(glm::vec3 dir, float speed) {
    this->pos += (glm::normalize(this->front * this->move_plane) * dir.z) * speed
                 + (glm::normalize(this->right) * dir.x) * speed
                 + (glm::normalize(this->world_up) * dir.y) * speed;
}

void sgl::camera::rotate(float yaw, float pitch, float sensitivity) {
    this->yaw += yaw * sensitivity;
    this->pitch += pitch * sensitivity;

    if (this->constrain_pitch) {
        if (this->pitch > 89.9f)
            this->pitch = 89.9f;
        else if (this->pitch < -89.9f)
            this->pitch = -89.9f;
    }

    this->update_vectors();
}

void sgl::camera::set_rotation(float yaw, float pitch) {
    this->yaw = yaw;
    this->pitch = pitch;

    this->update_vectors();
}

void sgl::camera::set_flying(bool enable_flight) {
    if (enable_flight)
        this->move_plane = glm::vec3(1.0, 1.0, 1.0);
    else
        this->move_plane = glm::vec3(1.0, 0.0, 1.0);
}

glm::mat4 sgl::camera::get_view() {
    return glm::lookAt(
        this->pos,
        this->pos + this->front,
        this->up
    );
}

