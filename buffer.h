#pragma once

#include "camera.h"
#include "maths.h"
#include "mesh2.h"
#include "shader.h"

struct BasicRenderingBuffer {
    unsigned int VAO{};
    unsigned int VBO{};
    unsigned int VBO_face_indices{};
    Shader shader;
    int n_vertices{};
};

struct RenderingParameters {
    Vec3 color;
    Mat4 model_transform;
    Mat4 view_transform;
    Mat4 perspective_transform;
    Vec3 camera_position;
    bool show_normals;
};

// struct SolidObjectProperties {
//     Vec3 color;
//     Mat4 transform;
// };

// struct Rectangle {
//     float width;
//     float height;
//     float depth;
//     Vec3 center;
//     Mesh mesh;
//     SolidObjectProperties obj;
//     BasicRenderingBuffer rendering;
// };

void draw(const BasicRenderingBuffer &buffer, const RenderingParameters &param);

BasicRenderingBuffer init_rendering(const Mesh &mesh);

// void draw(const Rectangle &cube, const Camera &camera);

// Buffer make_surface(int rows, int cols);

// Rectangle make_rectangle(float width, float height, float depth);
//
// Rectangle make_cube(float size);
