#include "mesh.h"

#include <cmath>

Vec3 normal_for_face(Vec3 a, Vec3 b, Vec3 c) {
    Vec3 v = b - a;
    Vec3 w = c - a;
    Vec3 n = normalize(cross(v, w)); // verified correct order
    return n;
}

std::vector<Vec3> compute_normals(const std::vector<Vec3> &vertices) {
    // assuming the normals are packed by faces
    std::vector<Vec3> normals;
    normals.reserve(vertices.size());
    for (int i = 0; i < vertices.size(); i += 3) {
        Vec3 n = normal_for_face(vertices[i], vertices[i + 1], vertices[i + 2]);
        normals.push_back(n);
        normals.push_back(n);
        normals.push_back(n);
    }
    return normals;
}

Mesh tetra_mesh(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 v3) {
    Mesh mesh;
    mesh.vertices = {v0, v2, v3, v0, v1, v2, v0, v3, v1, v1, v3, v2};
    mesh.normals = compute_normals(mesh.vertices);
    mesh.face_indices = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3};
    return mesh;
}

Mesh tetra_from_face(Vec3 a, Vec3 b, Vec3 c) {
    Vec3 n = normal_for_face(a, b, c);
    Vec3 center = (a + b + c) / 3.f;
    float edge_length = norm(c - a);
    float height = edge_length * std::sqrt(6.f) / 3.f;
    Vec3 d = center - n * height;
    return tetra_mesh(a, b, c, d);
}

Face face_from_line(Vec3 a, Vec3 b, Vec3 normal) {
    Vec3 middle = (a + b) / 2.f;
    float height = norm(a - b) * std::sqrt(3.f) / 2.f;
    Vec3 dir = normalize(cross(a - b, normal));
    Vec3 c = middle + dir * height;
    return {a, b, c};
}

Mesh octa_mesh(Vec3 top, Vec3 bottom, Vec3 front, Vec3 back, Vec3 left, Vec3 right) {
    Mesh mesh;
    mesh.vertices = {right, top,    front, back, top,    right, left,   top,
                     back,  front,  top,   left, right,  front, bottom, back,
                     right, bottom, left,  back, bottom, front, left,   bottom};
    mesh.normals = compute_normals(mesh.vertices);
    mesh.face_indices = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7};
    return mesh;
}

Vec3 face_centroid(Mesh mesh, int face_index) {
    return (mesh.vertices[face_index * 3] + mesh.vertices[face_index * 3 + 1] +
            mesh.vertices[face_index * 3 + 2]) /
           3.f;
}

Mesh octa_from_face(Vec3 front, Vec3 top, Vec3 left) {
    Vec3 n = normal_for_face(front, top, left);
    float side_length = norm(left - front);
    float inscribed_sphere_radius = side_length * std::sqrt(6.f) / 6.f;
    Vec3 face_center = (front + top + left) / 3.f;
    Vec3 center = face_center - n * inscribed_sphere_radius;
    Vec3 bottom = center - (top - center);
    Vec3 back = center - (front - center);
    Vec3 right = center - (left - center);
    return octa_mesh(top, bottom, front, back, left, right);
}

Mesh rectangle_mesh(float width, float height, float depth) {
    Mesh mesh;
    mesh.vertices = {{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
                     {0.5f, 0.5f, -0.5f},   {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},

                     {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},
                     {0.5f, 0.5f, 0.5f},    {-0.5f, 0.5f, 0.5f},  {-0.5f, -0.5f, 0.5f},

                     {-0.5f, 0.5f, 0.5f},   {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
                     {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},

                     {0.5f, 0.5f, 0.5f},    {0.5f, 0.5f, -0.5f},  {0.5f, -0.5f, -0.5f},
                     {0.5f, -0.5f, -0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},

                     {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f},
                     {0.5f, -0.5f, 0.5f},   {-0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, -0.5f},

                     {-0.5f, 0.5f, -0.5f},  {0.5f, 0.5f, -0.5f},  {0.5f, 0.5f, 0.5f},
                     {0.5f, 0.5f, 0.5f},    {-0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, -0.5f}};

    mesh.normals = {{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
                    {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},

                    {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},
                    {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},

                    {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
                    {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},

                    {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},
                    {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},

                    {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
                    {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},

                    {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f}};

    for (auto &vertex : mesh.vertices) {
        vertex.x *= width;
        vertex.y *= height;
        vertex.z *= depth;
    }
    return mesh;
}
