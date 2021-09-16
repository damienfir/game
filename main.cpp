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
    bool move_up;
    bool move_down;
    float rotate_x;
    float rotate_y;
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

    axes.x_axis = make_object(axis(0));
    axes.y_axis = make_object(axis(1));
    axes.z_axis = make_object(axis(2));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
}

void update(float dt) {
    if (controls.move_right) {
        camera.move_horizontal(dt);
    } else if (controls.move_left) {
        camera.move_horizontal(-dt);
    }

    if (controls.move_backwards) {
        camera.move_towards(-dt);
    } else if (controls.move_forward) {
        camera.move_towards(dt);
    }

    if (controls.move_up) {
        camera.move_vertical(dt);
    } else if (controls.move_down) {
        camera.move_vertical(-dt);
    }

    if (controls.rotate_x != 0 || controls.rotate_y != 0) {
        log(controls.rotate_x);
        camera.rotate_direction(controls.rotate_x, controls.rotate_y);
        controls.rotate_x = 0;
        controls.rotate_y = 0;
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


    if (key == GLFW_KEY_E) {
        if (action == GLFW_PRESS) {
            controls.move_down = false;
            controls.move_up = true;
        } else if (action == GLFW_RELEASE) {
            controls.move_up = false;
        }
    }

    if (key == GLFW_KEY_Q) {
        if (action == GLFW_PRESS) {
            controls.move_up = false;
            controls.move_down = true;
        } else if (action == GLFW_RELEASE) {
            controls.move_down = false;
        }
    }
}

struct Mouse {
    int x;
    int y;
    bool already_moved = false;
};

Mouse mouse;

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (!mouse.already_moved) {
        mouse.already_moved = true;
        mouse.x = xpos;
        mouse.y = ypos;
    }

    controls.rotate_x = xpos - mouse.x;
    controls.rotate_y = mouse.y - ypos; // reversed
    mouse.x = xpos;
    mouse.y = ypos;
}

int main(int argc, char **argv) {
    GLFWwindow *window;

    glfwInit();

    window = glfwCreateWindow(640, 480, "Game", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
