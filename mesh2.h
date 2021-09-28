#pragma once

#include "maths.h"

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
};

Mesh floor_mesh(int rows, int cols);
Mesh rectangle_mesh(float width, float height, float depth);
Mesh floor_tile_mesh(float width, float depth);
