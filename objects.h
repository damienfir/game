#pragma once

#include "mesh.h"
#include "buffer.h"

enum ObjectType { Tetrahedron, Octahedron };

struct TetraOcta {
    float circumsphere_radius;
    Mesh mesh;
    SolidObjectProperties obj;
    BasicRenderingBuffer rendering;
};

struct PolyObject {
    std::vector<unsigned int> parts;
};

TetraOcta make_tetra_or_octa(Mesh mesh, ObjectType type);