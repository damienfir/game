#include "math.h"

#include "logging.h"
#include <cassert>
#include <cmath>

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

Matrix translation(const Vec3 &t) {
    Matrix m = eye();
    m(0, 3) = t.x;
    m(1, 3) = t.y;
    m(2, 3) = t.z;
    return m;
}

Vec3 operator*(const Matrix &A, const Vec3 &v) {
    Vec3 r;
    r.x = A.val(0, 0) * v.x + A.val(0, 1) * v.y + A.val(0, 2) * v.z + A.val(0, 3);
    r.y = A.val(1, 0) * v.x + A.val(1, 1) * v.y + A.val(1, 2) * v.z + A.val(1, 3);
    r.z = A.val(2, 0) * v.x + A.val(2, 1) * v.y + A.val(2, 2) * v.z + A.val(2, 3);
    return r;
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

Vec3 operator-(const Vec3 &p, const Vec3 &q) { return {p.x - q.x, p.y - q.y, p.z - q.z}; }

Vec3 operator-(float a, const Vec3 &p) {
    Vec3 q;
    q.x = a - p.x;
    q.y = a - p.y;
    q.z = a - p.z;
    return q;
}

void operator-=(Vec3 &p, const Vec3 &q) {
    p.x -= q.x;
    p.y -= q.y;
    p.z -= q.z;
}

Vec3 operator+(const Vec3 &p, const Vec3 &q) { return {p.x + q.x, p.y + q.y, p.z + q.z}; }

void operator+=(Vec3 &p, const Vec3 &q) {
    p.x += q.x;
    p.y += q.y;
    p.z += q.z;
}

Vec3 operator*(const Vec3 &v, float a) { return {v.x * a, v.y * a, v.z * a}; }

void operator*=(Vec3 &p, float a) {
    p.x *= a;
    p.y *= a;
    p.z *= a;
}

float norm(const Vec3 &v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

Vec3 normalize(const Vec3 &v) { return v / norm(v); }

Vec3 operator/(const Vec3 &v, float a) { return {v.x / a, v.y / a, v.z / a}; }

Vec3 cross(const Vec3 &a, const Vec3 &b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

float dot(const Vec3 &a, const Vec3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

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

Matrix transpose(const Matrix &A) {
    Matrix B;
    for (int i = 0; i < A.rows(); ++i) {
        for (int j = 0; j < A.cols(); ++j) {
            B(j, i) = A.val(i, j);
        }
    }
    return B;
}

float radians(float deg) { return deg * (float)M_PI / 180.f; }

float degrees(float rad) { return rad * 180.f / (float)M_PI; }

Matrix scale(const Matrix &A, float factor) {
    Matrix B = A;
    B(0, 0) *= factor;
    B(1, 1) *= factor;
    B(2, 2) *= factor;
    return B;
}
Matrix rotation_y(float deg) {
    Matrix R = eye();
    R(0, 0) = std::cos(radians(deg));
    R(0, 2) = -std::sin(radians(deg));
    R(2, 0) = std::sin(radians(deg));
    R(2, 2) = std::cos(radians(deg));
    return R;
}
Matrix rotate_y(const Matrix &A, float deg) { return rotation_y(deg) * A; }

void log(const Vec3 &v) { std::cout << string(v) << std::endl; }