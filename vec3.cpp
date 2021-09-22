#include "vec3.h"

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

Vec3 centroid(const std::vector<Vec3> &points) {
    Vec3 c = {0, 0, 0};
    float n = points.size();
    for (const Vec3 &p : points) {
        c += p / n;
    }
    return c;
}
