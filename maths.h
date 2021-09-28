#pragma once

#include <string>
#include <vector>

struct Vec3 {
    float x{0};
    float y{0};
    float z{0};
};

struct Vec4 {
    float x{0};
    float y{0};
    float z{0};
    float w{0};
};

class Mat4 {
  public:
    Mat4() { m_values.resize(16); }


    float val(int i, int j) const { return m_values[i * cols() + j]; }

    float &operator()(int i) { return m_values[i]; }

    float &operator()(int i, int j) { return m_values[i * cols() + j]; }

    const float *ptr() const { return m_values.data(); }

    int rows() const { return 4; }

    int cols() const { return 4; }

    std::vector<float> m_values;
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

Vec3 centroid(Vec3 a, Vec3 b, Vec3 c);
Vec3 centroid(const std::vector<Vec3> &points);

Mat4 eye();
Mat4 perspective(float near, float far, float right, float top);
Mat4 lookat(Vec3 position, Vec3 target, Vec3 up);

Vec3 operator*(const Mat4 &A, const Vec3 &v);
Mat4 operator*(const Mat4 &A, const Mat4 &B);
Vec4 operator*(const Mat4 &A, const Vec4 &v);
Mat4 translate(const Mat4 &A, const Vec3 &t);
Mat4 scale(const Mat4 &A, float factor);
Mat4 rotate_y(const Mat4 & A, float deg);
Mat4 transpose(const Mat4 &A);
Mat4 invert(const Mat4& A);


std::string string(const Mat4 &m);

float radians(float deg);
float degrees(float rad);