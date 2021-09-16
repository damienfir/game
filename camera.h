#pragma once

#include "math.h"

class Camera {
public:
    Camera() : m_position({0, 1, 2}), m_up({0, 1, 0}), m_pitch(-15), m_yaw(-90) {
        m_projection = perspective(0.1, 100.0, 1, 1);
//        m_projection = eye();
        update_direction();
        update_view_matrix();
    }

    void move_horizontal(float t);

    void move_vertical(float t);

    void move_towards(float t);

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
};