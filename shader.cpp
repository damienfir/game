#include "shader.h"

#include <GL/glew.h>
#include <string>
#include <stdexcept>


Shader compile(const char *vertex_source, const char *fragment_source) {
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);
    GLint ok;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint length;
        char log[256];
        glGetShaderInfoLog(vertex_shader, 256, &length, log);
        throw std::runtime_error(std::string("Vertex shader: ") + log);
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint length;
        char log[256];
        glGetShaderInfoLog(fragment_shader, 256, &length, log);
        throw std::runtime_error(std::string("Fragment shader: ") + log);
    }

    Shader shader{};
    shader.program = glCreateProgram();
    glAttachShader(shader.program, vertex_shader);
    glAttachShader(shader.program, fragment_shader);
    glLinkProgram(shader.program);
    glGetProgramiv(shader.program, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint length;
        char log[256];
        glGetProgramInfoLog(shader.program, 256, &length, log);
        throw std::runtime_error(log);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader;
}

void use(const Shader &shader) {
    glUseProgram(shader.program);
}