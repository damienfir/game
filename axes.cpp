#include "axes.h"

#include <GL/glew.h>

template <typename T> long byte_size(const std::vector<T> &vector) {
    return vector.size() * sizeof(T);
}

void draw(const Axis &axis, const Camera &camera) {
    UseShader use(axis.shader.program);
    glBindVertexArray(axis.VAO);

    set_matrix4(axis.shader, "view", camera.view());
    set_matrix4(axis.shader, "projection", camera.projection());
    set_vec3(axis.shader, "color", axis.color);

    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, axis.vertices.size());
    glDrawArrays(GL_LINE_STRIP, 0, axis.vertices.size());
    glBindVertexArray(0);
}

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

void draw(Axes axes, const Camera &camera) {
    draw(axes.x_axis, camera);
    draw(axes.y_axis, camera);
    draw(axes.z_axis, camera);
}

Axes make_axes() {
    Axes a;
    a.x_axis = make_axis(0);
    a.y_axis = make_axis(1);
    a.z_axis = make_axis(2);
    return a;
}