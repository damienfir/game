#pragma once

#include "camera.h"
#include "maths.h"
#include "shader.h"

struct Axis {
    unsigned int VAO;
    unsigned int VBO;
    Shader shader;
    std::vector<Vec3> vertices;
    Vec3 color;
};

struct Axes {
    Axis x_axis;
    Axis y_axis;
    Axis z_axis;
};

void draw(Axes axes, const Camera &camera);
Axes make_axes();