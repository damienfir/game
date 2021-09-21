#pragma once

#include "math.h"

class Camera {
  public:
    //    {3, 3, 10}
    Camera()
        : m_position({3, 2, 5}), m_direction(normalize({-0.5, -0.2, -1})), m_up({0, 1, 0}),
          m_movement_speed(6), m_faster_factor(2), m_sensitivity(0.2) {
        m_projection = perspective(0.1, 100.0, 0.05, 0.05 * 0.5625); // Use window size ratio

        update_view_matrix();
    }

    void set_position(Vec3 position);

    Vec3 move_horizontal() const;

    Vec3 move_vertical() const;

    Vec3 move_towards() const;

    void rotate_direction(float dx, float dy);

    void rotate_around(Vec3 p, float dx, float dy);

    Vec3 position() const { return m_position; }

    Vec3 direction() const { return m_direction; }

    Matrix projection() const { return m_projection; }

    Matrix view() const { return m_view; }

  private:
    void update_view_matrix();

    Vec3 m_position;
    Vec3 m_direction;
    Vec3 m_up;
    Matrix m_view;
    Matrix m_projection;

    float m_movement_speed;
    float m_faster_factor;
    float m_sensitivity;
};