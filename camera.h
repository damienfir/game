#pragma once

#include "math.h"

class Camera {
public:
    Camera() : m_position({3, 3, 10}), m_up({0, 1, 0}), m_pitch(-15), m_yaw(-100), m_movement_speed(3),
               m_faster_factor(2),
               m_sensitivity(0.2) {
        m_projection = perspective(0.1, 100.0, 0.05, 0.05 * 0.5625);  // Use window size ratio

//        m_projection = eye();
        update_direction();
        update_view_matrix();
    }

    void move_horizontal(float t, bool faster);

    void move_vertical(float t, bool faster);

    void move_towards(float t, bool faster);

    void rotate_direction(float dx, float dy);

    Matrix projection() const {
        return m_projection;
    }

    Matrix view() const {
        return m_view;
    }
private:

    void update_view_matrix();

    void update_direction();
    Vec3 m_position;
    Vec3 m_direction;
    Vec3 m_up;
    float m_pitch;
    float m_yaw;
    Matrix m_view;
    Matrix m_projection;
    float m_movement_speed;
    float m_faster_factor;

    float m_sensitivity;
};