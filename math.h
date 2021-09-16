#pragma once

#include <vector>
#include <string>

struct Vec3 {
    float x;
    float y;
    float z;
};

Vec3 operator+(const Vec3 &p, const Vec3 &q);

void operator+=(Vec3 &p, const Vec3 &q);

Vec3 operator-(const Vec3 &p, const Vec3 &q);

Vec3 operator/(const Vec3 &v, float a);

Vec3 operator*(const Vec3&v, float a);

Vec3 normalize(const Vec3 &v);

float norm(const Vec3 &v);

Vec3 cross(const Vec3 &a, const Vec3 &b);


class Matrix {
public:
    Matrix() {
        values.resize(16);
    }

    float val(int i, int j) const {
        return values[i * cols() + j];
    }

    float &operator()(int i, int j) {
        return values[i * cols() + j];
    }

    const float *ptr() const {
        return values.data();
    }

    int rows() const { return 4; }

    int cols() const { return 4; }

    std::vector<float> values;
};

Matrix eye();

Matrix perspective(float near, float far, float right, float top);

Matrix operator*(const Matrix &A, const Matrix &B);

Matrix translate(const Matrix &A, const Vec3 &t);

Matrix lookat(Vec3 position, Vec3 target, Vec3 up);

std::string string(const Matrix &m);

std::string string(const Vec3 &v);

Matrix transpose(const Matrix& A);

float radians(float deg);