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