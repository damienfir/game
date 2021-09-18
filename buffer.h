#pragma once

#include <GL/glew.h>
#include "shader.h"
#include "math.h"
#include "camera.h"

struct Axis {
    unsigned int VAO;
    unsigned int VBO;
    Shader shader;
    std::vector<Vec3> vertices;
    Vec3 color;
};

struct Rectangle {
    float width;
    float height;
    float depth;
    unsigned int VAO;
    unsigned int VBO;
    Shader shader;
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    Vec3 color;
    Matrix transform;
};

void draw(const Axis &axis, const Camera &camera);

void draw(const Rectangle &cube, const Camera &camera);

//Buffer make_surface(int rows, int cols);

Axis make_axis(int ax);

Rectangle make_rectangle(float width, float height, float depth);

Rectangle make_cube(float size);