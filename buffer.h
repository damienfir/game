#pragma once

#include "shader.h"

#include <vector>

struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

struct Buffer {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    Shader shader;
    Mesh mesh;
};

void draw(const Buffer &object);

Buffer make_object();
