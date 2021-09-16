#include "math.h"

#include <cmath>
#include <cassert>
#include "logging.h"

Matrix eye() {
    Matrix m;
    for (int i = 0; i < m.rows(); ++i) {
        for (int j = 0; j < m.cols(); ++j) {
            m(i, j) = 0;
        }
        m(i, i) = 1;
    }
    return m;
}

Matrix perspective(float near, float far, float right, float top) {
    // from http://www.songho.ca/opengl/gl_projectionmatrix.html
    Matrix m;
    m(0, 0) = near / right;
    m(1, 1) = near / top;
    m(2, 2) = -(far + near) / (far - near);
    m(2, 3) = -2 * far * near / (far - near);
    m(3, 2) = -1;
    return m;
}

Matrix lookat(Vec3 position, Vec3 target, Vec3 up) {
    Vec3 direction = normalize(position - target);
    Vec3 right = normalize(cross(up, direction));
    Vec3 cam_up = cross(direction, right);

    Matrix rot;
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

    Matrix trans = eye();
    trans(0, 3) = -position.x;
    trans(1, 3) = -position.y;
    trans(2, 3) = -position.z;

    return rot * trans;
}

Matrix operator*(const Matrix &A, const Matrix &B) {
    assert(A.cols() == B.rows());

    Matrix C;
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

std::string string(const Matrix &m) {
    std::string s;
    for (int i = 0; i < m.rows(); ++i) {
        for (int j = 0; j < m.cols(); ++j) {
            s += std::to_string(m.val(i, j)) + " ";
        }
        s += "\n";
    }
    return s;
}

Vec3 operator-(const Vec3 &p, const Vec3 &q) {
    return {p.x - q.x, p.y - q.y, p.z - q.z};
}

float norm(const Vec3 &v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 normalize(const Vec3 &v) {
    return v / norm(v);
}

Vec3 operator/(const Vec3 &v, float a) {
    return {v.x / a, v.y / a, v.z / a};
}

Vec3 cross(const Vec3 &a, const Vec3 &b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

std::string string(const Vec3 &v) {
    return std::to_string(v.x) + " " + std::to_string(v.y) + " " + std::to_string(v.z);
}

Matrix translate(const Matrix &A, const Vec3 &t) {
    Matrix B = A;
    B(0, 3) += t.x;
    B(1, 3) += t.y;
    B(2, 3) += t.z;
    return B;
}
