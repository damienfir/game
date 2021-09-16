#include "camera.h"

#include "logging.h"
#include "cmath"

Vec3 direction_from_euler(float yaw, float pitch) {
    Vec3 dir;
    dir.x = std::cos(radians(yaw)) * std::cos(radians(pitch));
    dir.y = std::sin(radians(pitch));
    dir.z = std::sin(radians(yaw)) * std::cos(radians(pitch));
    return normalize(dir);
}

void Camera::move_horizontal(float t) {
    m_position += normalize(cross(m_direction, m_up)) * t * m_movement_speed;
    update_view_matrix();
}

void Camera::move_vertical(float t) {
    m_position += m_up * t * m_movement_speed;
    update_view_matrix();
}

void Camera::move_towards(float t) {
    Vec3 horizontal_dir = {m_direction.x, 0, m_direction.z};
    m_position += normalize(horizontal_dir) * t * m_movement_speed;
    update_view_matrix();
}

void Camera::rotate_direction(float dx, float dy) {
    m_pitch += dy * m_sensitivity;
    m_yaw += dx * m_sensitivity;

    if (m_pitch > 89) {
        m_pitch = 89;
    }

    if (m_pitch < -89) {
        m_pitch = -89;
    }

    update_direction();
    update_view_matrix();
}

void Camera::update_direction() {
    m_direction = direction_from_euler(m_yaw, m_pitch);
    log(string(m_direction));
}

void Camera::update_view_matrix() {
    m_view = lookat(m_position, m_position + m_direction, m_up);
}
