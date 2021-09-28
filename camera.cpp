#include "camera.h"

#include "logging.h"
#include "physics.h"
//#include "world.h"

Camera::Camera()
    : m_position({0.5f, 1.f, -0.5f}), m_direction(normalize({0.2f, -0.1f, -0.5f})), m_up({0, 1, 0}),
      m_movement_speed(5), m_faster_factor(2), m_sensitivity(0.2) {
    m_projection = perspective(0.1, 100.0, 0.05, 0.05 * 0.5625); // Use window size ratio

    update_view_matrix();
}

void Camera::set_position(Vec3 position) {
    m_position = position + Vec3{0.f, 1.f, 0.f};
    update_view_matrix();
}

Vec3 Camera::move_horizontal() const {
    return normalize(cross(m_direction, m_up)) * m_movement_speed;
}

Vec3 Camera::move_vertical() const { return m_up * m_movement_speed; }

Vec3 Camera::move_towards_restricted() const {
    Vec3 horizontal_dir = {m_direction.x, 0.f, m_direction.z};
    return normalize(horizontal_dir) * m_movement_speed;
}

Vec3 Camera::move_towards() const { return m_direction * m_movement_speed; }

void Camera::rotate_direction(float dx, float dy) {
    // Works for small (dx, dy), otherwise use yaw/pitch system
    // I removed it because it was too much state to maintain and it had to be recomputed depending
    // on which rotation you are using
    m_direction += normalize(cross(m_direction, m_up)) * dx * m_sensitivity * 0.015;
    m_direction += m_up * dy * m_sensitivity * 0.015;
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



//void update_move_around(Camera &camera, float dt) {
//    Vec3 velocity;
//
//    if (camera.controls.move_right) {
//        velocity = camera.move_horizontal();
//    } else if (camera.controls.move_left) {
//        velocity = 0 - camera.move_horizontal();
//    }
//
//    if (camera.controls.move_backwards) {
//        velocity -= camera.move_towards();
//    } else if (camera.controls.move_forward) {
//        velocity += camera.move_towards();
//    }
//
//    if (camera.controls.move_up) {
//        velocity += camera.move_vertical();
//    } else if (camera.controls.move_down) {
//        velocity -= camera.move_vertical();
//    }
//
//    if (camera.controls.move_faster)
//        velocity *= 2.f;
//
//    camera.set_position(camera.position() + velocity * dt);
//
//    camera.rotate_around(face_centroid(world.tetraoctas[world.editor->selected.target_index].mesh,
//                                       world.editor->selected.face_index),
//                         -camera.controls.dx, -camera.controls.dy);
//
//    camera.controls.dx = 0;
//    camera.controls.dy = 0;
//}
