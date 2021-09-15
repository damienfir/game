#include "buffer.h"

#include <GL/glew.h>

void draw(const Object &object) {
    use(object.shader);
    glBindVertexArray(object.VAO);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Object make_object() {
    const char *vs =
            "#version 330 core\n"
            "layout (location = 0) in vec2 coord2d;                  "
            "void main(void) {                        "
            "  gl_Position = vec4(coord2d, 0.0, 1.0); "
            "}";

    const char *fs =
            "#version 330\n"
            "out vec4 FragColor;"
            "void main(void) {        "
            "  FragColor = vec4(0.0, 0.0, 1.0, 1.0); "
            "}";

    Object obj{};
    obj.shader = compile(vs, fs);

    float vertices[] = {-1.0, -1.0, 0, 1, 1, -1};
    unsigned int indices[] = {0, 1, 2};

    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);

    return obj;
}
