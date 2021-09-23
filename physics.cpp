#include "physics.h"

#include "logging.h"
#include "maths.h"
#include "world.h"

IntersectData intersect_tetras_point(Sphere sphere) {
    IntersectData d;
    for (TetraOcta &obj : world.tetraoctas) {

        // FIXME: no need to use duplicated vertices
        if (norm(sphere.pos - centroid(obj.mesh.vertices)) - sphere.radius >
            obj.circumsphere_radius) {
            continue;
        }

        int n_faces = obj.mesh.normals.size() / 3;
        for (int i = 0; i < n_faces; ++i) {
            Vec3 normal = obj.mesh.normals[i * 3];
            Vec3 reference = obj.mesh.vertices[i * 3];
            float distance = dot(normal, sphere.pos - reference);
            if (distance > 0 && distance < sphere.radius) {
                d.intersected = true;
                // FIXME: need better way to handle collisions with multiple objects
                d.normal = normalize(d.normal + normal);
            }
        }
    }
    return d;
}

// IntersectData intersect(const Rectangle &r, const Sphere &sphere) {
//     IntersectData d;
//     Vec3 pos = r.obj.transform * r.center;
//
//     if (norm(sphere.pos - pos) - sphere.radius >
//         norm({r.width / 2.f, r.height / 2.f, r.depth / 2.f})) {
//         return d;
//     }
//
//     float distances[] = {
//         pos.x - r.width / 2.f - sphere.pos.x,    // left
//         sphere.pos.x - (pos.x + r.width / 2.f),  // right
//         sphere.pos.y - (pos.y + r.height / 2.f), // top
//         pos.y - r.height / 2.f - sphere.pos.y,   // bottom
//         sphere.pos.z - (pos.z + r.depth / 2.f),  // front
//         pos.z - r.depth / 2.f - sphere.pos.z,    // back
//     };
//     int index_smallest_distance = -1;
//     bool intersected = false;
//     for (int i = 0; i < 6; ++i) {
//         if (distances[i] >= 0 && distances[i] < sphere.radius) {
//             if (index_smallest_distance == -1 ||
//                 distances[i] < distances[index_smallest_distance]) {
//                 index_smallest_distance = i;
//             }
//             intersected = true;
//         }
//     }
//     Vec3 normals[] = {{-1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
//     if (intersected) {
//         d.intersected = true;
//         d.normal = normals[index_smallest_distance];
//     }
//     return d;
// }
