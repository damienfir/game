#pragma once

#include "mesh.h"
#include "buffer.h"

struct TetraOcta {
    Mesh mesh;
    SolidObjectProperties obj;
    BasicRenderingBuffer rendering;
};

TetraOcta make_tetra_or_octa(Mesh mesh);
TetraOcta tetra_from_octa_face(const TetraOcta &octa, int face);
TetraOcta make_tetra();
