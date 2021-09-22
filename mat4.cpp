#include "mat4.h"

#include "logging.h"
#include <cassert>
#include <cmath>
#include "vec3.h"

Mat4 eye() {
    Mat4 m;
    for (int i = 0; i < m.rows(); ++i) {
        for (int j = 0; j < m.cols(); ++j) {
            m(i, j) = 0;
        }
        m(i, i) = 1;
    }
    return m;
}

Mat4 perspective(float near, float far, float right, float top) {
    // from http://www.songho.ca/opengl/gl_projectionmatrix.html
    Mat4 m;
    m(0, 0) = near / right;
    m(1, 1) = near / top;
    m(2, 2) = -(far + near) / (far - near);
    m(2, 3) = -2 * far * near / (far - near);
    m(3, 2) = -1;
    return m;
}

Mat4 lookat(Vec3 position, Vec3 target, Vec3 up) {
    Vec3 direction = normalize(position - target);
    Vec3 right = normalize(cross(up, direction));
    Vec3 cam_up = cross(direction, right);

    Mat4 rot;
    rot(0, 0) = right.x;
    rot(0, 1) = right.y;
    rot(0, 2) = right.z;
    rot(1, 0) = cam_up.x;
    rot(1, 1) = cam_up.y;
    rot(1, 2) = cam_up.z;
    rot(2, 0) = direction.x;
    rot(2, 1) = direction.y;
    rot(2, 2) = direction.z;
    rot(3, 3) = 1;

    Mat4 trans = eye();
    trans(0, 3) = -position.x;
    trans(1, 3) = -position.y;
    trans(2, 3) = -position.z;

    return rot * trans;
}

Mat4 translation(const Vec3 &t) {
    Mat4 m = eye();
    m(0, 3) = t.x;
    m(1, 3) = t.y;
    m(2, 3) = t.z;
    return m;
}

Vec3 operator*(const Mat4 &A, const Vec3 &v) {
    Vec3 r;
    r.x = A.val(0, 0) * v.x + A.val(0, 1) * v.y + A.val(0, 2) * v.z + A.val(0, 3);
    r.y = A.val(1, 0) * v.x + A.val(1, 1) * v.y + A.val(1, 2) * v.z + A.val(1, 3);
    r.z = A.val(2, 0) * v.x + A.val(2, 1) * v.y + A.val(2, 2) * v.z + A.val(2, 3);
    return r;
}

Mat4 operator*(const Mat4 &A, const Mat4 &B) {
    assert(A.cols() == B.rows());

    Mat4 C;
    for (int i = 0; i < A.rows(); ++i) {
        for (int j = 0; j < B.cols(); ++j) {
            float x = 0;
            for (int k = 0; k < B.rows(); ++k) {
                x += A.val(i, k) * B.val(k, j);
            }
            C(i, j) = x;
        }
    }
    return C;
}

Mat4 translate(const Mat4 &A, const Vec3 &t) {
    Mat4 B = A;
    B(0, 3) += t.x;
    B(1, 3) += t.y;
    B(2, 3) += t.z;
    return B;
}

Mat4 transpose(const Mat4 &A) {
    Mat4 B;
    for (int i = 0; i < A.rows(); ++i) {
        for (int j = 0; j < A.cols(); ++j) {
            B(j, i) = A.val(i, j);
        }
    }
    return B;
}

float radians(float deg) { return deg * (float)M_PI / 180.f; }

float degrees(float rad) { return rad * 180.f / (float)M_PI; }

Mat4 scale(const Mat4 &A, float factor) {
    Mat4 B = A;
    B(0, 0) *= factor;
    B(1, 1) *= factor;
    B(2, 2) *= factor;
    return B;
}
Mat4 rotation_y(float deg) {
    Mat4 R = eye();
    R(0, 0) = std::cos(radians(deg));
    R(0, 2) = -std::sin(radians(deg));
    R(2, 0) = std::sin(radians(deg));
    R(2, 2) = std::cos(radians(deg));
    return R;
}
Mat4 rotate_y(const Mat4 &A, float deg) { return rotation_y(deg) * A; }