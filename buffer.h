#pragma once

#include "shader.h"

struct Object {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    Shader shader;
};

void draw(const Object &object);

Object make_object();
