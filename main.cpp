#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <optional>

#include "buffer.h"
#include "logging.h"

class Timer {
  public:
    Timer() { reset(); }

    void reset() { m_timepoint = std::chrono::high_resolution_clock::now(); }

    float seconds_elapsed() {
        auto new_timepoint = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(new_timepoint - m_timepoint)
                      .count();
        return ms / 1000.f;
    }

    float tick() {
        auto s = seconds_elapsed();
        reset();
        return s;
    }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_timepoint;
};

class FPSCounter {
  public:
    FPSCounter() : m_last_dt(1) {}

    void tick(float dt) { m_last_dt = dt; }

    float fps() const { return 1.f / m_last_dt; }

  private:
    float m_last_dt;
};

struct CameraControls {
    bool move_left;
    bool move_right;
    bool move_forward;
    bool move_backwards;
    bool move_up;
    bool move_down;
    bool move_faster;
    float rotation_x;
    float rotation_y;
};

struct RenderControls {
    bool wireframe = false;
};

struct Axes {
    Buffer x_axis;
    Buffer y_axis;
    Buffer z_axis;
};

Camera camera;
Axes axes;
Buffer floor;
Buffer cube;
CameraControls camera_controls;
RenderControls render_controls;

Axes make_axes() {
    Axes a;
    a.x_axis = make_axis(0);
    a.y_axis = make_axis(1);
    a.z_axis = make_axis(2);
    return a;
}

void draw(Axes axes, const Camera &camera) {
    draw(axes.x_axis, camera);
    draw(axes.y_axis, camera);
    draw(axes.z_axis, camera);
}

void display() {
    glClearColor(0.0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw(floor, camera);
    draw(axes, camera);
    draw(cube, camera);
}

void update(float dt) {
    if (camera_controls.move_right) {
        camera.move_horizontal(dt, camera_controls.move_faster);
    } else if (camera_controls.move_left) {
        camera.move_horizontal(-dt, camera_controls.move_faster);
    }

    if (camera_controls.move_backwards) {
        camera.move_towards(-dt, camera_controls.move_faster);
    } else if (camera_controls.move_forward) {
        camera.move_towards(dt, camera_controls.move_faster);
    }

    if (camera_controls.move_up) {
        camera.move_vertical(dt, camera_controls.move_faster);
    } else if (camera_controls.move_down) {
        camera.move_vertical(-dt, camera_controls.move_faster);
    }

    if (camera_controls.rotation_x != 0 || camera_controls.rotation_y != 0) {
        camera.rotate_direction(camera_controls.rotation_x, camera_controls.rotation_y);
        camera_controls.rotation_x = 0;
        camera_controls.rotation_y = 0;
    }
}

void init() {
    floor = make_surface(10, 10);
    cube = make_cube(3);
    axes = make_axes();
    glEnable(GL_DEPTH_TEST);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }

    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            camera_controls.move_right = false;
            camera_controls.move_left = true;
        } else if (action == GLFW_RELEASE) {
            camera_controls.move_left = false;
        }
    }

    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            camera_controls.move_left = false;
            camera_controls.move_right = true;
        } else if (action == GLFW_RELEASE) {
            camera_controls.move_right = false;
        }
    }

    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            camera_controls.move_backwards = false;
            camera_controls.move_forward = true;
        } else if (action == GLFW_RELEASE) {
            camera_controls.move_forward = false;
        }
    }

    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            camera_controls.move_forward = false;
            camera_controls.move_backwards = true;
        } else if (action == GLFW_RELEASE) {
            camera_controls.move_backwards = false;
        }
    }

    if (key == GLFW_KEY_E) {
        if (action == GLFW_PRESS) {
            camera_controls.move_down = false;
            camera_controls.move_up = true;
        } else if (action == GLFW_RELEASE) {
            camera_controls.move_up = false;
        }
    }

    if (key == GLFW_KEY_Q) {
        if (action == GLFW_PRESS) {
            camera_controls.move_up = false;
            camera_controls.move_down = true;
        } else if (action == GLFW_RELEASE) {
            camera_controls.move_down = false;
        }
    }

    if (key == GLFW_KEY_LEFT_SHIFT) {
        if (action == GLFW_PRESS) {
            camera_controls.move_faster = true;
        } else if (action == GLFW_RELEASE) {
            camera_controls.move_faster = false;
        }
    }

    if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
        render_controls.wireframe = !render_controls.wireframe;
        if (render_controls.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}



struct Mouse {
    int x;
    int y;
    bool already_moved = false;
};

Mouse mouse;
Timer mouse_throttle;

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (mouse_throttle.seconds_elapsed() < 0.016)
        return;
    mouse_throttle.tick();

    if (!mouse.already_moved) {
        mouse.already_moved = true;
        mouse.x = xpos;
        mouse.y = ypos;
    }

    camera_controls.rotation_x = xpos - mouse.x;
    camera_controls.rotation_y = mouse.y - ypos;
    mouse.x = xpos;
    mouse.y = ypos;
}



int main(int argc, char **argv) {
    GLFWwindow *window;

    glfwInit();
    float ratio = 16.f / 9.f;
    int width = 1920;

    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(width, width / ratio, "Game", NULL, NULL);
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

        auto dt = timer.tick();
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
