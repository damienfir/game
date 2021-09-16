#include "buffer.h"

#include <GL/glew.h>
#include "logging.h"


void draw(const Buffer &object, const Camera &camera) {
    use(object.shader);
    glBindVertexArray(object.VAO);

    int model_loc = glGetUniformLocation(object.shader.program, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, object.transform.ptr());

    int view_loc = glGetUniformLocation(object.shader.program, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, camera.view.ptr());

    int projection_loc = glGetUniformLocation(object.shader.program, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, camera.projection.ptr());

    glDrawElements(object.mesh.mode, object.mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh grid_mesh(int rows, int cols) {
    auto linear = [&](int r, int c) {
        return r * cols + c;
    };

    Mesh mesh;
    mesh.vertices.reserve(rows * cols * 3);
    mesh.indices.reserve((rows - 1) * (cols - 1) * 6);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mesh.vertices.push_back(j); // x
            mesh.vertices.push_back(i); // y
            mesh.vertices.push_back(-1); // z

            if (i < rows - 1 && j < cols - 1) {
                mesh.indices.push_back(linear(i, j));
                mesh.indices.push_back(linear(i, j + 1));
                mesh.indices.push_back(linear(i + 1, j));
                mesh.indices.push_back(linear(i + 1, j));
                mesh.indices.push_back(linear(i, j + 1));
                mesh.indices.push_back(linear(i + 1, j + 1));
            }
        }
    }

    return mesh;
}

Mesh axis(int ax) {
    int size = 100;
    int n = size * 2 + 1;
    Mesh mesh;
    mesh.vertices.reserve(n * 3);
    mesh.indices.reserve(n);
    mesh.mode = GL_POINTS;
    for (int i = 0; i < n; ++i) {
        mesh.vertices.push_back(ax == 0 ? i - size : 0);
        mesh.vertices.push_back(ax == 1 ? i - size : 0);
        mesh.vertices.push_back(ax == 2 ? i - size : 0);
        mesh.indices.push_back(ax * n + i);
    }
    return mesh;
}

Buffer make_object(const Mesh& mesh) {
    Buffer obj{};
    obj.shader = compile("vertex.glsl", "fragment.glsl");
    obj.mesh = mesh;
    obj.transform = eye();

    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, obj.mesh.vertices.size() * sizeof(float), obj.mesh.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.mesh.indices.size() * sizeof(unsigned int), obj.mesh.indices.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);

    return obj;
}
