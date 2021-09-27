#include "physics.h"

#include "logging.h"
#include "maths.h"

//IntersectData intersect_tetras_point(Sphere sphere) {
//    IntersectData d;
//    for (TetraOcta &obj : world.tetraoctas) {
//
//        // FIXME: no need to use duplicated vertices
//        if (norm(sphere.pos - centroid(obj.mesh.vertices)) - sphere.radius >
//            obj.circumsphere_radius) {
//            continue;
//        }
//
//        int n_faces = obj.mesh.normals.size() / 3;
//        for (int i = 0; i < n_faces; ++i) {
//            Vec3 normal = obj.mesh.normals[i * 3];
//            Vec3 reference = obj.mesh.vertices[i * 3];
//            float distance = dot(normal, sphere.pos - reference);
//            if (distance > 0 && distance < sphere.radius) {
//                d.intersected = true;
//                // FIXME: need better way to handle collisions with multiple objects
//                d.normal = normalize(d.normal + normal);
//            }
//        }
//    }
//    return d;
//}

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


//void walk_on_surface() {
//    const Entity &entity = world.entities[entity_index];
//    for (int face_index = 0; face_index < entity.mesh.vertices.size() / 3; ++face_index) {
//        Vec3 v0 = entity.transform * entity.mesh.vertices[face_index * 3];
//        Vec3 v1 = entity.transform * entity.mesh.vertices[face_index * 3 + 1];
//        Vec3 v2 = entity.transform * entity.mesh.vertices[face_index * 3 + 2];
//        Vec3 n = entity.mesh.normals[face_index * 3];
//
//        Vec3 face_center = centroid(v0, v1, v2);
//        float circumsphere_radius = norm(v0 - face_center);
//        if (norm(sphere.pos - face_center) < circumsphere_radius) {
//
//            Vec3 p0 = sphere.pos - v0;
//            Vec3 p1 = sphere.pos - v1;
//            Vec3 p2 = sphere.pos - v2;
//            if (dot(p0, v1 - v0) > 0 && dot(p1, v2 - v1) > 0 && dot(p2, v0 - v2) > 0) {
//                float distance = dot(sphere.pos - v0, n);
//                if (distance - sphere.radius < 0) {
//                    log(distance - sphere.radius);
//                    velocity -= n * (distance - sphere.radius);
//                    break;
//                }
//            }
//        }
//    }
//}