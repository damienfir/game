#include "camera.h"

#include "logging.h"

void Camera::set_position(Vec3 position) {
    m_position = position;
    update_view_matrix();
}

Vec3 Camera::move_horizontal() const {
    return normalize(cross(m_direction, m_up)) * m_movement_speed;
}

Vec3 Camera::move_vertical() const { return m_up * m_movement_speed; }

Vec3 Camera::move_towards() const {
    Vec3 horizontal_dir = {m_direction.x, 0, m_direction.z};
    return normalize(horizontal_dir) * m_movement_speed;
}

void Camera::rotate_direction(float dx, float dy) {
    // Works for small (dx, dy), otherwise use yaw/pitch system
    // I removed it because it was too much state to maintain and it had to be recomputed depending
    // on which rotation you are using
    m_direction += normalize(cross(m_direction, m_up)) * dx * m_sensitivity * 0.02;
    m_direction += m_up * dy * m_sensitivity * 0.02;
    m_direction = normalize(m_direction);
    update_view_matrix();
}

void Camera::rotate_around(Vec3 p, float dx, float dy) {
    Vec3 to_camera = position() - p;
    float distance = norm(to_camera);
    Vec3 horizontal = normalize(cross(m_direction, m_up)) * dx * m_sensitivity;
    Vec3 vertical = m_up * dy * m_sensitivity;
    m_position = p + normalize(to_camera + vertical + horizontal) * distance;
    m_direction = normalize(p - m_position);
    update_view_matrix();
}

void Camera::update_view_matrix() { m_view = lookat(m_position, m_position + m_direction, m_up); }