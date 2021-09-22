#pragma once

#include "camera.h"
#include "mat4.h"
#include "shader.h"
#include <GL/glew.h>

struct Axis {
    unsigned int VAO;
    unsigned int VBO;
    Shader shader;
    std::vector<Vec3> vertices;
    Vec3 color;
};

struct BasicRenderingBuffer {
    unsigned int VAO{};
    unsigned int VBO{};
    Shader shader;
    int n_vertices{};
};

struct SolidObjectProperties {
    Vec3 color;
    Mat4 transform;
};

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
};

struct Rectangle {
    float width;
    float height;
    float depth;
    Vec3 center;
    Mesh mesh;
    SolidObjectProperties obj;
    BasicRenderingBuffer rendering;
};

void draw(const BasicRenderingBuffer &buffer, const SolidObjectProperties &obj,
          const Camera &camera);

BasicRenderingBuffer init_rendering(const Mesh &mesh);

void draw(const Axis &axis, const Camera &camera);

// void draw(const Rectangle &cube, const Camera &camera);

// Buffer make_surface(int rows, int cols);

Axis make_axis(int ax);

Rectangle make_rectangle(float width, float height, float depth);

Rectangle make_cube(float size);
