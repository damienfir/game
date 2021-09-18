#pragma once

#include <GL/glew.h>
#include "shader.h"
#include "math.h"
#include "camera.h"


struct Buffer {
    unsigned int VAO;
    unsigned int VBO;
    Shader shader;
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    bool has_normals = false;
    bool is_axis = false;
    Vec3 color;
    Matrix transform;
};

void draw(const Buffer& buffer, const Camera &camera);

Buffer make_surface(int rows, int cols);

Buffer make_axis(int ax);

Buffer make_rectangle(float width, float height, float depth);

Buffer make_cube(float size);