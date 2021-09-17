#include "buffer.h"

#include <GL/glew.h>
#include "logging.h"


template<typename T>
long byte_size(const std::vector<T> &vector) {
    return vector.size() * sizeof(T);
}

void draw(const Buffer &object, const Camera &camera) {
    if (object.type == Buffer::ObjectType::Axis) {

        use(object.shader);
        glBindVertexArray(object.VAO);

        set_matrix4(object.shader, "view", camera.view());
        set_matrix4(object.shader, "projection", camera.projection());
        set_vec3(object.shader, "color", object.color);

        glPointSize(10);
        glDrawArrays(GL_POINTS, 0, object.vertices.size() / 3);
        glDrawArrays(GL_LINE_STRIP, 0, object.vertices.size() / 3);
        glBindVertexArray(0);

    } else if (object.type == Buffer::ObjectType::Cube) {
        use(object.shader);
        glBindVertexArray(object.VAO);

        set_matrix4(object.shader, "model", object.transform);
        set_matrix4(object.shader, "view", camera.view());
        set_matrix4(object.shader, "projection", camera.projection());
        set_vec3(object.shader, "color", object.color);
        set_vec3(object.shader, "viewer_pos", camera.position());

        glDrawArrays(GL_TRIANGLES, 0, object.vertices.size() / 6);
        glBindVertexArray(0);
    }
}

//Mesh grid_mesh(int rows, int cols) {
//    auto linear = [&](int r, int c) {
//        return r * cols + c;
//    };
//
//    Mesh mesh;
//    mesh.vertices.reserve(rows * cols * 3);
//    mesh.indices.reserve((rows - 1) * (cols - 1) * 6);
//    mesh.color = {1, 1, 1};
//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < cols; ++j) {
//            mesh.vertices.push_back(j); // x
//            mesh.vertices.push_back(i); // y
//            mesh.vertices.push_back(-1); // z
//
//            if (i < rows - 1 && j < cols - 1) {
//                mesh.indices.push_back(linear(i, j));
//                mesh.indices.push_back(linear(i, j + 1));
//                mesh.indices.push_back(linear(i + 1, j));
//                mesh.indices.push_back(linear(i + 1, j));
//                mesh.indices.push_back(linear(i, j + 1));
//                mesh.indices.push_back(linear(i + 1, j + 1));
//            }
//        }
//    }
//
//    return mesh;
//}

Buffer make_axis(int ax) {
    int size = 100;
    int n = size * 2 + 1;
    Buffer buffer;
    buffer.type = Buffer::ObjectType::Axis;
    buffer.vertices.reserve(n * 3);
    for (int i = 0; i < n; ++i) {
        buffer.vertices.push_back(ax == 0 ? i - size : 0);
        buffer.vertices.push_back(ax == 1 ? i - size : 0);
        buffer.vertices.push_back(ax == 2 ? i - size : 0);
    }

    buffer.color = (ax == 0) ? Vec3{1, 0, 0} : (ax == 1) ? Vec3{0, 1, 0} : Vec3{0, 0, 1};
    buffer.shader = compile("shaders/axis_vertex.glsl", "shaders/axis_fragment.glsl");

    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);
    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, byte_size(buffer.vertices), buffer.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);

    return buffer;
}

Buffer make_rectangle(float width, float height, float depth) {
    Buffer buffer;
    buffer.type = Buffer::ObjectType::Cube;
    buffer.transform = eye();

    buffer.vertices = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
    };

    // rescale
    for (int i = 0; i < buffer.vertices.size() / 6; ++i) {
        buffer.vertices[i * 6] *= width;
        buffer.vertices[i * 6 + 1] *= height;
        buffer.vertices[i * 6 + 2] *= depth;
    }

    buffer.color = {0.1, 0.4, 0.3};
    buffer.shader = compile("shaders/phong_vertex.glsl", "shaders/phong_fragment.glsl");

    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);
    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, byte_size(buffer.vertices), buffer.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return buffer;
}

Buffer make_cube(float size) {
    return make_rectangle(size, size, size);
}
