#include "buffer.h"

#include <GL/glew.h>

void draw(const Buffer &object) {
    use(object.shader);
    glBindVertexArray(object.VAO);
    glDrawElements(GL_TRIANGLES, object.mesh.indices.size(), GL_UNSIGNED_INT, 0);
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
            mesh.vertices.push_back(j * 0.1); // x
            mesh.vertices.push_back(i * 0.1); // y
            mesh.vertices.push_back(0); // z

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

Buffer make_object() {
    const char *vs =
            "#version 330 core\n"
            "layout (location = 0) in vec3 coord3d;                  "
            "void main(void) {                        "
            "  gl_Position = vec4(coord3d, 1.0); "
            "}";

    const char *fs =
            "#version 330\n"
            "out vec4 FragColor;"
            "void main(void) {        "
            "  FragColor = vec4(0.0, 0.0, 1.0, 1.0); "
            "}";

    Buffer obj{};
    obj.shader = compile(vs, fs);
    obj.mesh = grid_mesh(10, 10);

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
