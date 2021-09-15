#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "buffer.h"

Object object;

void display() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    draw(object);
}

void init() {
    object = make_object();
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
//    std::cout << xpos << ", " << ypos << std::endl;
}

int main(int argc, char **argv) {
    GLFWwindow *window;

    glfwInit();

    window = glfwCreateWindow(640, 480, "Game", NULL, NULL);
    glfwMakeContextCurrent(window);

    // set raw mouse input for FPV control
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glewInit();
    init();

    while (!glfwWindowShouldClose(window)) {
        display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
