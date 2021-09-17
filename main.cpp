#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>

#include "buffer.h"
#include "logging.h"

class Timer {
public:
    Timer() {
        reset();
    }

    void reset() {
        m_timepoint = std::chrono::high_resolution_clock::now();
    }

    float seconds_elapsed() {
        auto new_timepoint = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(new_timepoint - m_timepoint).count();
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

    void tick(float dt) {
        m_last_dt = dt;
    }

    float fps() const {
        return 1.f / m_last_dt;
    }

private:
    float m_last_dt;
};

template<typename T>
class CircularBuffer {
public:
    explicit CircularBuffer(int n) : m_next(0) {
        m_buffer.resize(n);
    }

    void add(T element) {
        m_buffer[m_next++ % size()] = element;
    }

    T operator[](int i) const {
        return m_buffer[i];
    }

    bool is_full() const {
        return m_next >= size();
    }

    int size() const {
        return m_buffer.size();
    }

private:
    std::vector<T> m_buffer;
    int m_next;
};

float average(const CircularBuffer<float> &buffer) {
    int n = buffer.size();
    float value = 0;
    for (int i = 0; i < n; ++i) value += buffer[i] / (float) n;
    return value;
}

class CameraRotation {
public:
    CameraRotation() : m_xs(CircularBuffer<float>(3)), m_ys(CircularBuffer<float>(3)), m_new(false) {}

    void set(float x, float y) {
        m_xs.add(x);
        m_ys.add(y);
        m_new = true;
    }

    float x() const {
        if (!m_new) return 0;
        if (!m_xs.is_full()) return 0;
        return average(m_xs);
    }

    float y() const {
        if (!m_new) return 0;
        if (!m_ys.is_full()) return 0;
        return average(m_ys);
    }

    void reset() {
        m_new = false;
    }

private:
    CircularBuffer<float> m_xs;
    CircularBuffer<float> m_ys;
    bool m_new;
};

struct Controls {
    bool move_left;
    bool move_right;
    bool move_forward;
    bool move_backwards;
    bool move_up;
    bool move_down;
    bool move_faster;
    CameraRotation rotation;
    bool wireframe = false;
};


Buffer object;
Camera camera;
Controls controls;
Buffer cube;

struct Axes {
    Buffer x_axis;
    Buffer y_axis;
    Buffer z_axis;
};
Axes axes;

void draw(Axes axes, const Camera &camera) {
    glPointSize(10);
    axes.x_axis.mesh.mode = GL_POINTS;
    draw(axes.x_axis, camera);
    axes.y_axis.mesh.mode = GL_POINTS;
    draw(axes.y_axis, camera);
    axes.z_axis.mesh.mode = GL_POINTS;
    draw(axes.z_axis, camera);

    axes.x_axis.mesh.mode = GL_LINE_STRIP;
    draw(axes.x_axis, camera);
    axes.y_axis.mesh.mode = GL_LINE_STRIP;
    draw(axes.y_axis, camera);
    axes.z_axis.mesh.mode = GL_LINE_STRIP;
    draw(axes.z_axis, camera);
}

void display() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    draw(object, camera);
    draw(axes, camera);
    draw(cube, camera);
}

void init() {
    object = make_object(grid_mesh(10, 10));

    axes.x_axis = make_object(make_axis(0));
    axes.x_axis.mesh.color = {1, 0, 0};
    axes.y_axis = make_object(make_axis(1));
    axes.y_axis.mesh.color = {0, 1, 0};
    axes.z_axis = make_object(make_axis(2));
    axes.z_axis.mesh.color = {0, 0, 1};

    cube = make_object(make_cube(3));
    cube.mesh.color = {0.3, 0.2, 0.5};

    glEnable(GL_DEPTH_TEST);
}

void update(float dt) {
    if (controls.move_right) {
        camera.move_horizontal(dt, controls.move_faster);
    } else if (controls.move_left) {
        camera.move_horizontal(-dt, controls.move_faster);
    }

    if (controls.move_backwards) {
        camera.move_towards(-dt, controls.move_faster);
    } else if (controls.move_forward) {
        camera.move_towards(dt, controls.move_faster);
    }

    if (controls.move_up) {
        camera.move_vertical(dt, controls.move_faster);
    } else if (controls.move_down) {
        camera.move_vertical(-dt, controls.move_faster);
    }

    if (controls.rotation.y() != 0 || controls.rotation.y() != 0) {
        camera.rotate_direction(controls.rotation.x(), controls.rotation.y());
        controls.rotation.reset();
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

    if (key == GLFW_KEY_LEFT_SHIFT) {
        if (action == GLFW_PRESS) {
            controls.move_faster = true;
        } else if (action == GLFW_RELEASE) {
            controls.move_faster = false;
        }
    }

    if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
        controls.wireframe = !controls.wireframe;
        if (controls.wireframe) {
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
    if (mouse_throttle.seconds_elapsed() < 0.016) return;
    mouse_throttle.tick();

    if (!mouse.already_moved) {
        mouse.already_moved = true;
        mouse.x = xpos;
        mouse.y = ypos;
    }

    controls.rotation.set(xpos - mouse.x, mouse.y - ypos);
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
