#include "shader.h"

#include "maths.h"

#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string read_from_file(const std::string &path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

Shader compile(const std::string &vertex, const std::string &fragment) {
    std::string vertex_source = read_from_file(vertex);
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

    std::string fragment_source = read_from_file(fragment);
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

// void use(const Shader &shader) { glUseProgram(shader.program); }

void set_matrix4(const Shader &shader, const std::string &name, const Mat4 &matrix) {
    int loc = glGetUniformLocation(shader.program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_TRUE, matrix.ptr());
}

void set_vec3(const Shader &shader, const std::string &name, const Vec3 &vec) {
    int loc = glGetUniformLocation(shader.program, name.c_str());
    float v[] = {vec.x, vec.y, vec.z};
    glUniform3fv(loc, 1, v);
}

void set_int(const Shader &shader, const std::string &name, int value) {
    int loc = glGetUniformLocation(shader.program, name.c_str());
    glUniform1i(loc, value);
}

UseShader::UseShader(unsigned int program) { glUseProgram(program); }

UseShader::~UseShader() { glUseProgram(0); }
