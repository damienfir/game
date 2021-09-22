#include "buffer.h"

#include "logging.h"
#include <GL/glew.h>

template <typename T> long byte_size(const std::vector<T> &vector) {
    return vector.size() * sizeof(T);
}

std::vector<float> pack_vertices_and_normals(const std::vector<Vec3> &vertices,
                                             const std::vector<Vec3> &normals) {
    std::vector<float> data;
    data.reserve(vertices.size() * 6);
    for (int i = 0; i < vertices.size(); ++i) {
        data.push_back(vertices[i].x);
        data.push_back(vertices[i].y);
        data.push_back(vertices[i].z);
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
    }
    return data;
}

void draw(const Axis &axis, const Camera &camera) {
    use(axis.shader);
    glBindVertexArray(axis.VAO);

    set_matrix4(axis.shader, "view", camera.view());
    set_matrix4(axis.shader, "projection", camera.projection());
    set_vec3(axis.shader, "color", axis.color);

    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, axis.vertices.size());
    glDrawArrays(GL_LINE_STRIP, 0, axis.vertices.size());
    glBindVertexArray(0);
}

void draw(const BasicRenderingBuffer &buffer, const SolidObjectProperties &obj,
          const Camera &camera) {
    use(buffer.shader);
    glBindVertexArray(buffer.VAO);

    set_matrix4(buffer.shader, "model", obj.transform);
    set_matrix4(buffer.shader, "view", camera.view());
    set_matrix4(buffer.shader, "projection", camera.projection());
    set_vec3(buffer.shader, "color", obj.color);
    set_vec3(buffer.shader, "viewer_pos", camera.position());

    glDrawArrays(GL_TRIANGLES, 0, buffer.n_vertices);
    glBindVertexArray(0);
}

// Buffer make_surface(int rows, int cols) {
//     Buffer buffer;
//     buffer.color = {0.2, 1, 0};
//     buffer.transform = scale(translate(eye(), {-5, 0, -5}), 10);
//     buffer.shader = compile("shaders/phong_vertex.glsl", "shaders/phong_fragment.glsl");
//     buffer.has_normals = true;
//
//     for (int i = 0; i < rows - 1; ++i) {
//         for (int j = 0; j < cols - 1; ++j) {
//             float ii = i;
//             float jj = j;
//             std::vector<Vec3> vertices = {{ii, 0, jj},     {ii + 1, 0, jj}, {ii, 0, jj + 1},
//                                           {ii, 0, jj + 1}, {ii + 1, 0, jj}, {ii + 1, 0, jj + 1}};
//
//             for (auto v : vertices) {
//                 buffer.vertices.push_back(v);
//                 buffer.normals.push_back({0, 1, 0});
//             }
//         }
//     }
//
//     std::vector<float> data = pack_vertices_and_normals(buffer.vertices, buffer.normals);
//
//     glGenVertexArrays(1, &buffer.VAO);
//     glGenBuffers(1, &buffer.VBO);
//     glBindVertexArray(buffer.VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
//     glBufferData(GL_ARRAY_BUFFER, byte_size(data), data.data(), GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 *
//     sizeof(float))); glEnableVertexAttribArray(1);
//
//     return buffer;
// }

Axis make_axis(int ax) {
    int size = 100;
    int n = size * 2 + 1;
    Axis buffer;
    buffer.vertices.reserve(n);
    buffer.color = (ax == 0) ? Vec3{1, 0, 0} : (ax == 1) ? Vec3{0, 1, 0} : Vec3{0, 0, 1};
    buffer.shader = compile("shaders/axis_vertex.glsl", "shaders/axis_fragment.glsl");

    for (int i = 0; i < n; ++i) {
        float ii = i;
        buffer.vertices.push_back(
            {ax == 0 ? ii - size : 0, ax == 1 ? ii - size : 0, ax == 2 ? ii - size : 0});
    }

    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);
    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, byte_size(buffer.vertices), buffer.vertices.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    return buffer;
}

BasicRenderingBuffer init_rendering(const Mesh &mesh) {
    std::vector<float> data = pack_vertices_and_normals(mesh.vertices, mesh.normals);

    BasicRenderingBuffer buffer;
    buffer.shader = compile("shaders/phong_vertex.glsl", "shaders/phong_fragment.glsl");
    buffer.n_vertices = mesh.vertices.size();

    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);
    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, byte_size(data), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return buffer;
}

Rectangle make_rectangle(float width, float height, float depth) {
    Rectangle rect;
    rect.width = width;
    rect.height = height;
    rect.depth = depth;
    rect.obj.transform = eye();
    rect.center = {0, 0, 0};
    rect.obj.color = {0.1, 0.4, 0.3};

    rect.mesh.vertices = {{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
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

    rect.mesh.normals = {{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
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

    for (auto &vertex : rect.mesh.vertices) {
        vertex.x *= width;
        vertex.y *= height;
        vertex.z *= depth;
    }

    rect.rendering = init_rendering(rect.mesh);

    return rect;
}

Rectangle make_cube(float size) { return make_rectangle(size, size, size); }