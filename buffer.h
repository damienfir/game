#pragma once

#include <GL/glew.h>
#include "shader.h"
#include "math.h"
#include "camera.h"


struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLenum mode = GL_TRIANGLES;
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

Mesh axis(int ax);