#pragma once

#include "camera.h"
#include "maths.h"
#include "mesh.h"
#include "shader.h"
#include <GL/glew.h>

struct BasicRenderingBuffer {
    unsigned int VAO{};
    unsigned int VBO{};
    unsigned int VBO_face_indices{};
    Shader shader;
    int n_vertices{};
};

struct SolidObjectProperties {
    Vec3 color;
    Mat4 transform;
    int highlighted_face{-1};
    float alpha{1};
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

// void draw(const Rectangle &cube, const Camera &camera);

// Buffer make_surface(int rows, int cols);

Rectangle make_rectangle(float width, float height, float depth);

Rectangle make_cube(float size);
