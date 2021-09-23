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

TetraOcta make_tetra_or_octa(Mesh mesh, ObjectType type);
TetraOcta tetra_from_octa_face(const TetraOcta &octa, int face);
TetraOcta make_tetra();
