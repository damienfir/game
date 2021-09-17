#pragma once

#include <GL/glew.h>
#include "shader.h"
#include "math.h"
#include "camera.h"


struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLenum mode = GL_TRIANGLES;
    Vec3 color;
};

struct Buffer {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    Shader shader;
    Mesh mesh;
    Matrix transform;
};

void draw(const Buffer &object, const Camera &camera);

Buffer make_object(const Mesh &mesh);

Mesh grid_mesh(int rows, int cols);

Mesh make_axis(int ax);

Mesh make_rectangle(float width, float height, float depth);

Mesh make_cube(float size);