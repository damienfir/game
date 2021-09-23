#include "objects.h"

#include "maths.h"
#include "world.h"
#include <cmath>

TetraOcta make_tetra_or_octa(Mesh mesh, ObjectType type) {
    TetraOcta obj;
    obj.mesh = mesh;
    obj.obj.transform = eye();
    obj.obj.color = {0.1, 0.1, 0.7};
    obj.rendering = init_rendering(obj.mesh);
    if (type == ObjectType::Tetrahedron) {
        obj.circumsphere_radius = norm(mesh.vertices[0] - mesh.vertices[1]) * std::sqrt(6.f) / 4.f;
    } else if (type == ObjectType::Octahedron) {
        obj.circumsphere_radius = norm(mesh.vertices[0] - mesh.vertices[1]) * std::sqrt(2.f) / 4.f;
    }
    return obj;
}

TetraOcta make_tetra() {
    Vec3 v0 = {1, 0, -1 / std::sqrt(2.f)};
    Vec3 v1 = {-1, 0, -1 / std::sqrt(2.f)};
    Vec3 v2 = {0, 1, 1 / std::sqrt(2.f)};
    return make_tetra_or_octa(tetra_from_face(v0, v1, v2), ObjectType::Tetrahedron);
}

TetraOcta tetra_from_octa_face(const TetraOcta &octa, int face) {
    Vec3 a = octa.mesh.vertices[face * 3];
    Vec3 b = octa.mesh.vertices[face * 3 + 1];
    Vec3 c = octa.mesh.vertices[face * 3 + 2];
    return make_tetra_or_octa(tetra_from_face(a, b, c), ObjectType::Tetrahedron);
}