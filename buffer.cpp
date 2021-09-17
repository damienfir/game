#include "buffer.h"

#include "logging.h"
#include <GL/glew.h>

template <typename T> long byte_size(const std::vector<T> &vector) {
    return vector.size() * sizeof(T);
}

template <typename T> void draw(const Buffer<T> &object, const Camera &camera) {
    if (object.type == Buffer<T>::ObjectType::Axis) {

        use(object.shader);
        glBindVertexArray(object.VAO);

        set_matrix4(object.shader, "view", camera.view());
        set_matrix4(object.shader, "projection", camera.projection());
        set_vec3(object.shader, "color", object.color);

        glPointSize(10);
        glDrawArrays(GL_POINTS, 0, object.elements.size());
        glDrawArrays(GL_LINE_STRIP, 0, object.elements.size());
        glBindVertexArray(0);

    } else if (object.type == Buffer<T>::ObjectType::Cube) {
        use(object.shader);
        glBindVertexArray(object.VAO);

        set_matrix4(object.shader, "model", object.transform);
        set_matrix4(object.shader, "view", camera.view());
        set_matrix4(object.shader, "projection", camera.projection());
        set_vec3(object.shader, "color", object.color);
        set_vec3(object.shader, "viewer_pos", camera.position());

        glDrawArrays(GL_TRIANGLES, 0, object.elements.size());
        glBindVertexArray(0);
    }
}

template void draw<Vertex>(const Buffer<Vertex> &object, const Camera &camera);
template void draw<VertexNormal>(const Buffer<VertexNormal> &object, const Camera &camera);

Buffer<VertexNormal> make_surface(int rows, int cols) {
    Buffer<VertexNormal> buffer;
    buffer.type = Buffer<VertexNormal>::ObjectType::Cube;
    buffer.color = {0.2, 1, 0};
    buffer.transform = scale(translate(eye(), {-5, 0, -5}), 10);
    buffer.shader = compile("shaders/phong_vertex.glsl", "shaders/phong_fragment.glsl");

    for (int i = 0; i < rows - 1; ++i) {
        for (int j = 0; j < cols - 1; ++j) {
            float ii = i;
            float jj = j;
            std::vector<Vertex> vertices = {{ii, 0, jj},     {ii + 1, 0, jj}, {ii, 0, jj + 1},
                                            {ii, 0, jj + 1}, {ii + 1, 0, jj}, {ii + 1, 0, jj + 1}};

            for (auto v : vertices) {
                buffer.elements.push_back({v, {0, 1, 0}});
            }
        }
    }

    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);
    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, byte_size(buffer.elements), buffer.elements.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void *)sizeof(Vertex));
    glEnableVertexAttribArray(1);

    return buffer;
}

Buffer<Vertex> make_axis(int ax) {
    int size = 100;
    int n = size * 2 + 1;
    Buffer<Vertex> buffer;
    buffer.type = Buffer<Vertex>::ObjectType::Axis;
    buffer.elements.reserve(n * 3);
    for (int i = 0; i < n; ++i) {
        float ii = i;
        buffer.elements.push_back(
            {ax == 0 ? ii - size : 0, ax == 1 ? ii - size : 0, ax == 2 ? ii - size : 0});
    }

    buffer.color = (ax == 0) ? Vec3{1, 0, 0} : (ax == 1) ? Vec3{0, 1, 0} : Vec3{0, 0, 1};
    buffer.shader = compile("shaders/axis_vertex.glsl", "shaders/axis_fragment.glsl");

    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);
    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, byte_size(buffer.elements), buffer.elements.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    return buffer;
}

Buffer<VertexNormal> make_rectangle(float width, float height, float depth) {
    Buffer<VertexNormal> buffer;
    buffer.type = Buffer<VertexNormal>::ObjectType::Cube;
    buffer.transform = eye();

    buffer.elements = {
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},   {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},  {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},

        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},   {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},     {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},   {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},  {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},

        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},     {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},   {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},

        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}}, {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},   {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},  {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},

        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},   {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},     {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}};

    // rescale
    for (auto &e : buffer.elements) {
        e.vertex.x *= width;
        e.vertex.y *= height;
        e.vertex.z *= depth;
    }

    buffer.color = {0.1, 0.4, 0.3};
    buffer.shader = compile("shaders/phong_vertex.glsl", "shaders/phong_fragment.glsl");

    glGenVertexArrays(1, &buffer.VAO);
    glGenBuffers(1, &buffer.VBO);
    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, byte_size(buffer.elements), buffer.elements.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (void *)sizeof(Vec3));
    glEnableVertexAttribArray(1);

    return buffer;
}

Buffer<VertexNormal> make_cube(float size) { return make_rectangle(size, size, size); }

void log(const VertexNormal &vn) {
    log("vertex: " + string(vn.vertex) + " normal: " + string(vn.normal));
}