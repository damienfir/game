#pragma once

#include "maths.h"

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<int> face_indices;
};

Mesh tetra_from_face(Vec3 a, Vec3 b, Vec3 c);
Mesh octa_from_face(Vec3 front, Vec3 top, Vec3 left);
Vec3 face_centroid(Mesh mesh, int face_index);
Mesh rectangle_mesh(float width, float height, float depth);