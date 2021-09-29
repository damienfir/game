#include "mesh2.h"

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

Mesh floor_mesh(int rows, int cols) {
    Mesh mesh;
    Vec3 translation = {-50, 0, 50};
    for (int i = 0; i < rows - 1; ++i) {
        for (int j = 0; j < cols - 1; ++j) {
            float ii = i;
            float jj = j;
            Vec3 v0 = {ii, 0.f, -jj};
            Vec3 v1 = {ii + 1, 0.f, -jj};
            Vec3 v2 = {ii + 1, 0.f, -(jj + 1)};
            Vec3 v3 = {ii, 0.f, -(jj + 1)};
            mesh.vertices.push_back(v0 + translation);
            mesh.vertices.push_back(v1 + translation);
            mesh.vertices.push_back(v3 + translation);
            mesh.vertices.push_back(v3 + translation);
            mesh.vertices.push_back(v1 + translation);
            mesh.vertices.push_back(v2 + translation);
        }
    }

    mesh.normals = compute_normals(mesh.vertices);

    return mesh;
}

Mesh floor_tile_mesh(float width, float depth) {
    Mesh mesh;
    float dx = width/2.f;
    float dz = -depth/2.f;
    mesh.vertices = {
        {-dx, -0.f, -dz}, {dx, -0.f, -dz}, {-dx, -0.f, dz},
        {-dx, -0.f, dz}, {dx, -0.f, -dz}, {dx, -0.f, dz}
    };
    mesh.normals = compute_normals(mesh.vertices);
    return mesh;
}

Mesh rectangle_mesh(float width, float height, float depth) {
    Mesh mesh;
    Vec3 v0 = {-0.5, -0.5, 0.5};
    Vec3 v1 = {-0.5, -0.5, -0.5};
    Vec3 v2 = {-0.5, 0.5, 0.5};
    Vec3 v3 = {-0.5, 0.5, -0.5};
    Vec3 v4 = {0.5, -0.5, 0.5};
    Vec3 v5 = {0.5, -0.5, -0.5};
    Vec3 v6 = {0.5, 0.5, 0.5};
    Vec3 v7 = {0.5, 0.5, -0.5};

    mesh.vertices = {v0, v2, v1, v2, v3, v1,  // left
                     v1, v3, v5, v5, v3, v7,  // back
                     v5, v7, v6, v6, v4, v5,  // right
                     v4, v6, v2, v2, v0, v4,  // front
                     v2, v6, v3, v3, v6, v7,  // top
                     v0, v1, v4, v4, v1, v5}; // bottom

    mesh.normals = compute_normals(mesh.vertices);

    for (auto &vertex : mesh.vertices) {
        vertex.x *= width;
        vertex.y = (vertex.y + 0.5f) * height;
        vertex.z *= depth;
    }
    return mesh;
}

