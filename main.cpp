#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLfloat vertices[6] = {-1.0, -1.0, 0, 1, 1, -1};
GLuint program;
GLint attribute_coord2d;

void display() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glEnableVertexAttribArray(attribute_coord2d);
    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(attribute_coord2d);
}

GLuint compile_shader(const char *source, GLenum shader_type) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint length;
        char log[256];
        glGetShaderInfoLog(shader, 256, &length, log);
        throw std::runtime_error(log);
    }
    return shader;
}

GLuint link_program(GLuint vs, GLuint fs) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    GLint ok;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint length;
        char log[256];
        glGetProgramInfoLog(program, 256, &length, log);
        throw std::runtime_error(log);
    }
    return program;
}

void init() {
    const char *vs =
            "#version 120\n"  // OpenGL 2.1
            "attribute vec2 coord2d;                  "
            "void main(void) {                        "
            "  gl_Position = vec4(coord2d, 0.0, 1.0); "
            "}";
    auto vs_id = compile_shader(vs, GL_VERTEX_SHADER);

    const char *fs =
            "#version 120\n"  // OpenGL 2.1
            "void main(void) {        "
            "  gl_FragColor[0] = 0.0; "
            "  gl_FragColor[1] = 0.0; "
            "  gl_FragColor[2] = 1.0; "
            "}";
    auto fs_id = compile_shader(fs, GL_FRAGMENT_SHADER);
    program = link_program(vs_id, fs_id);

    attribute_coord2d = glGetAttribLocation(program, "coord2d");
    if (attribute_coord2d == -1) {
        throw std::runtime_error("Cannot bind 'coord2d'");
    }
}

int main(int argc, char **argv) {
    GLFWwindow *window;

    glfwInit();

    window = glfwCreateWindow(640, 480, "Game", NULL, NULL);
    glfwMakeContextCurrent(window);

    glewInit();
    init();

    while (!glfwWindowShouldClose(window))
    {
        display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();


    return 0;
}
