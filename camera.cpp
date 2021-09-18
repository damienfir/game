#include "camera.h"

#include "cmath"
#include "logging.h"

Vec3 direction_from_euler(float yaw, float pitch) {
  Vec3 dir;
  dir.x = std::cos(radians(yaw)) * std::cos(radians(pitch));
  dir.y = std::sin(radians(pitch));
  dir.z = std::sin(radians(yaw)) * std::cos(radians(pitch));
  return normalize(dir);
}

void Camera::set_position(Vec3 position) {
    m_position = position;
    update_view_matrix();
}

Vec3 Camera::move_horizontal(float t) const {
  return normalize(cross(m_direction, m_up)) * t * m_movement_speed;
}

Vec3 Camera::move_vertical(float t) const{
  return m_up * t * m_movement_speed;
}

Vec3 Camera::move_towards(float t) const {
  Vec3 horizontal_dir = {m_direction.x, 0, m_direction.z};
  return normalize(horizontal_dir) * t * m_movement_speed;
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
}
void Camera::update_view_matrix() {
  m_view = lookat(m_position, m_position + m_direction, m_up);
}
