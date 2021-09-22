#pragma once

#include <vector>

struct Vec3 {
    float x{0};
    float y{0};
    float z{0};
};

Vec3 operator+(const Vec3 &p, const Vec3 &q);

void operator+=(Vec3 &p, const Vec3 &q);

Vec3 operator-(const Vec3 &p, const Vec3 &q);

Vec3 operator-(float a, const Vec3 &p);

void operator-=(Vec3 &p, const Vec3 &q);

Vec3 operator/(const Vec3 &v, float a);

Vec3 operator*(const Vec3 &v, float a);

void operator*=(Vec3 &p, float a);

Vec3 normalize(const Vec3 &v);

float norm(const Vec3 &v);

Vec3 cross(const Vec3 &a, const Vec3 &b);

float dot(const Vec3 &a, const Vec3 &b);

Vec3 centroid(const std::vector<Vec3> &points);