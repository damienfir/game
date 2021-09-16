#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>

#include "buffer.h"
#include "logging.h"

class Timer {
public:
    void reset() {
        m_timepoint = std::chrono::high_resolution_clock::now();
    }

    float seconds_elapsed() {
        auto new_timepoint = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(new_timepoint - m_timepoint).count();
        reset();
        return ms / 1000.f;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_timepoint;
};

class FPSCounter {
public:
    FPSCounter() : m_last_dt(1) {}

    void tick(float dt) {
        m_last_dt = dt;
    }

    float fps() const {
        return 1.f / m_last_dt;
    }

private:
    float m_last_dt;
};

struct Controls {
    bool move_left;
    bool move_right;
    bool move_forward;
    bool move_backwards;
};

Buffer object;
Camera camera;
Controls controls;

struct Axes {
    Buffer x_axis;
    Buffer y_axis;
    Buffer z_axis;
};
Axes axes;

void draw(const Axes &axes, const Camera &camera) {
    glPointSize(10);
    draw(axes.x_axis, camera);
    draw(axes.y_axis, camera);
    draw(axes.z_axis, camera);
}

void display() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw(object, camera);
    draw(axes, camera);
}

void init() {
    object = make_object(grid_mesh(10, 10));
    camera.view = lookat({0, 0, 0}, {0, 0, -1}, {0, 1, 0});
//    camera.projection = perspective(0.1, 100.0, 5, 5);
    camera.projection = eye();

    axes.x_axis = make_object(axis(0));
    axes.y_axis = make_object(axis(1));
    axes.z_axis = make_object(axis(2));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
}

void update(float dt) {
    if (controls.move_right) {
        camera.view = translate(camera.view, {dt, 0, 0});
        log(string(camera.view));
    } else if (controls.move_left) {
        camera.view = translate(camera.view, {-dt, 0, 0});
        log(string(camera.view));
    }
    if (controls.move_backwards) {
        camera.view = translate(camera.view, {0, 0, -dt});
        log(string(camera.view));
    } else if (controls.move_forward) {
        camera.view = translate(camera.view, {0, 0, dt});
        log(string(camera.view));
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }

    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            controls.move_right = false;
            controls.move_left = true;
        } else if (action == GLFW_RELEASE) {
            controls.move_left = false;
        }
    }

    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            controls.move_left = false;
            controls.move_right = true;
        } else if (action == GLFW_RELEASE) {
            controls.move_right = false;
        }
    }

    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            controls.move_backwards = false;
            controls.move_forward = true;
        } else if (action == GLFW_RELEASE) {
            controls.move_forward = false;
        }
    }

    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            controls.move_forward = false;
            controls.move_backwards = true;
        } else if (action == GLFW_RELEASE) {
            controls.move_backwards = false;
        }
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
    auto timer = Timer();
    auto fps_counter = FPSCounter();

    while (!glfwWindowShouldClose(window)) {
        auto dt = timer.seconds_elapsed();
        update(dt);
        display();

//        fps_counter.tick(dt);
//        log("FPS: " + std::to_string(fps_counter.fps()));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
