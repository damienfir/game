#pragma once

#include <string>
#include <vector>

class Vec3;

class Mat4 {
  public:
    Mat4() { values.resize(16); }

    float val(int i, int j) const { return values[i * cols() + j]; }

    float &operator()(int i, int j) { return values[i * cols() + j]; }

    const float *ptr() const { return values.data(); }

    int rows() const { return 4; }

    int cols() const { return 4; }

    std::vector<float> values;
};

Mat4 eye();

Mat4 perspective(float near, float far, float right, float top);

Vec3 operator*(const Mat4 &A, const Vec3 &v);

Mat4 operator*(const Mat4 &A, const Mat4 &B);

Mat4 translate(const Mat4 &A, const Vec3 &t);

Mat4 lookat(Vec3 position, Vec3 target, Vec3 up);

std::string string(const Mat4 &m);

Mat4 transpose(const Mat4 &A);

float radians(float deg);

float degrees(float rad);

Mat4 scale(const Mat4 &A, float factor);

Mat4 rotate_y(const Mat4 & A, float deg);