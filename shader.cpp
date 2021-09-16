#include "shader.h"

#include <GL/glew.h>
#include <string>
#include <stdexcept>
#include <fstream>

std::string read_from_file(const std::filesystem::path &path) {
    std::ifstream file(path.string());
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

Shader compile(const std::filesystem::path &vertex, const std::filesystem::path &fragment) {
    std::string vertex_source = read_from_file(vertex.string());
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto source = vertex_source.c_str();
    glShaderSource(vertex_shader, 1, &source, NULL);
    glCompileShader(vertex_shader);
    GLint ok;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint length;
        char log[256];
        glGetShaderInfoLog(vertex_shader, 256, &length, log);
        throw std::runtime_error(std::string("Vertex shader: ") + log);
    }

    std::string fragment_source = read_from_file(fragment.string());
    source = fragment_source.c_str();
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &source, NULL);
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