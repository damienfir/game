#pragma once

#include "maths.h"

struct CameraControls {
    bool move_left;
    bool move_right;
    bool move_forward;
    bool move_backwards;
    bool move_up;
    bool move_down;
    bool move_faster;
    bool move_around;
    float dx;
    float dy;
};

class Camera {
  public:
    Camera();

    void set_position(Vec3 position);

    Vec3 move_horizontal() const;

    Vec3 move_vertical() const;

    Vec3 move_towards_restricted() const;

    Vec3 move_towards() const;

    void rotate_direction(float dx, float dy);

    void rotate_around(Vec3 p, float dx, float dy);

    Vec3 position() const { return m_position; }
    Vec3 direction() const { return m_direction; }
    Mat4 projection() const { return m_projection; }
    Mat4 view() const { return m_view; }

    CameraControls controls;

  private:
    void update_view_matrix();

    Vec3 m_position;
    Vec3 m_direction;
    Vec3 m_up;
    Mat4 m_view;
    Mat4 m_projection;

    float m_movement_speed;
    float m_faster_factor;
    float m_sensitivity;
};