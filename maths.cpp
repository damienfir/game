#include "maths.h"

#include "logging.h"
#include <cassert>
#include <cmath>

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

Vec3 centroid(Vec3 a, Vec3 b, Vec3 c) { return (a + b + c) / 3.f; }

Vec3 centroid(const std::vector<Vec3> &points) {
    Vec3 c = {0, 0, 0};
    float n = points.size();
    for (const Vec3 &p : points) {
        c += p / n;
    }
    return c;
}

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

Vec4 operator*(const Mat4 &A, const Vec4 &v) {
    Vec4 r;
    r.x = A.val(0, 0) * v.x + A.val(0, 1) * v.y + A.val(0, 2) * v.z + A.val(0, 3) * v.w;
    r.y = A.val(1, 0) * v.x + A.val(1, 1) * v.y + A.val(1, 2) * v.z + A.val(1, 3) * v.w;
    r.z = A.val(2, 0) * v.x + A.val(2, 1) * v.y + A.val(2, 2) * v.z + A.val(2, 3) * v.w;
    r.w = A.val(3, 0) * v.x + A.val(3, 1) * v.y + A.val(3, 2) * v.z + A.val(3, 3) * v.w;
    return r;
}

Mat4 invert(const Mat4 &A) {
    // Works for both column-major and row-major.
    // inverse(transpose(A)) == transpose(inverse(A))
    // https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
    auto m = A.m_values.data();
    double inv[16], det;
    int i;

    // clang-format off
    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
             m[4]  * m[11] * m[14] +
             m[8]  * m[6]  * m[15] -
             m[8]  * m[7]  * m[14] -
             m[12] * m[6]  * m[11] +
             m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
              m[4]  * m[10] * m[13] +
              m[8]  * m[5] * m[14] -
              m[8]  * m[6] * m[13] -
              m[12] * m[5] * m[10] +
              m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
             m[1]  * m[11] * m[14] +
             m[9]  * m[2] * m[15] -
             m[9]  * m[3] * m[14] -
             m[13] * m[2] * m[11] +
             m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
             m[0]  * m[11] * m[13] +
             m[8]  * m[1] * m[15] -
             m[8]  * m[3] * m[13] -
             m[12] * m[1] * m[11] +
             m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
             m[0]  * m[7] * m[14] +
             m[4]  * m[2] * m[15] -
             m[4]  * m[3] * m[14] -
             m[12] * m[2] * m[7] +
             m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
              m[0]  * m[6] * m[13] +
              m[4]  * m[1] * m[14] -
              m[4]  * m[2] * m[13] -
              m[12] * m[1] * m[6] +
              m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
             m[1] * m[7] * m[10] +
             m[5] * m[2] * m[11] -
             m[5] * m[3] * m[10] -
             m[9] * m[2] * m[7] +
             m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
              m[0] * m[7] * m[9] +
              m[4] * m[1] * m[11] -
              m[4] * m[3] * m[9] -
              m[8] * m[1] * m[7] +
              m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];
    // clang-format on

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    assert(det != 0);

    det = 1.0 / det;

    Mat4 Ai;
    for (i = 0; i < 16; i++)
        Ai(i) = inv[i] * det;

    return Ai;
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