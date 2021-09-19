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
    bool draw_axes = true;
};

struct Axes {
    Axis x_axis;
    Axis y_axis;
    Axis z_axis;
};

struct World {
    std::vector<Rectangle> rectangles;
};

World world;
Camera camera;
Axes axes;
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

    if (render_controls.draw_axes) {
        draw(axes, camera);
    }

    for (const auto &buffer : world.rectangles) {
        draw(buffer, camera);
    }
}
struct IntersectData {
    bool intersected = false;
    Vec3 normal;
};

struct Sphere {
    Vec3 pos;
    float radius;
};

IntersectData intersect(const Rectangle &r, const Sphere &sphere) {
    IntersectData d;
    Vec3 pos = r.transform * r.center;

    if (norm(sphere.pos - pos) - sphere.radius >
        norm({r.width / 2.f, r.height / 2.f, r.depth / 2.f})) {
        return d;
    }

    std::string faces[] = {"left", "right", "top", "bottom", "front", "back"};

    float distances[] = {
        pos.x - r.width / 2.f - sphere.pos.x,    // left
        sphere.pos.x - (pos.x + r.width / 2.f),  // right
        sphere.pos.y - (pos.y + r.height / 2.f), // top
        pos.y - r.height / 2.f - sphere.pos.y,   // bottom
        sphere.pos.z - (pos.z + r.depth / 2.f),  // front
        pos.z - r.depth / 2.f - sphere.pos.z,    // back
    };
    int index_smallest_distance = -1;
    bool intersected = false;
    for (int i = 0; i < 6; ++i) {
        if (distances[i] >= 0 && distances[i] < sphere.radius) {
            if (index_smallest_distance == -1 ||
                distances[i] < distances[index_smallest_distance]) {
                index_smallest_distance = i;
            }
            intersected = true;
        }
    }
    Vec3 normals[] = {{-1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
    if (intersected) {
        d.intersected = true;
        d.normal = normals[index_smallest_distance];
        log(faces[index_smallest_distance]);
        log(distances[index_smallest_distance]);
        log(string(d.normal));
    }
    return d;
}

void update(Camera &camera, const CameraControls &controls, float dt) {
    Vec3 velocity;

    if (controls.move_right) {
        velocity = camera.move_horizontal();
    } else if (controls.move_left) {
        velocity = 0 - camera.move_horizontal();
    }

    if (controls.move_backwards) {
        velocity -= camera.move_towards();
    } else if (controls.move_forward) {
        velocity += camera.move_towards();
    }

    if (controls.move_up) {
        velocity += camera.move_vertical();
    } else if (controls.move_down) {
        velocity -= camera.move_vertical();
    }

    if (controls.move_faster)
        velocity *= 2.f;

    Sphere sphere = {.pos = camera.position(), .radius = 0.3};
    for (const auto &other : world.rectangles) {
        IntersectData d = intersect(other, sphere);
        if (d.intersected) {
            log("hit rect: " + string(other.transform * other.center));
            log(other.width);
            log(string(other.color));
            velocity -= d.normal * std::min(0.f, dot(d.normal, velocity));
            break;
        }
    }

    camera.set_position(camera.position() + velocity * dt);

    if (controls.rotation_x != 0 || controls.rotation_y != 0) {
        camera.rotate_direction(controls.rotation_x, controls.rotation_y);
    }
}

void update(float dt) {
    update(camera, camera_controls, dt);
    camera_controls.rotation_x = 0;
    camera_controls.rotation_y = 0;
}

void init() {
    //    buffers = {make_surface(10, 10)};

    for (int i = 0; i < 100; ++i) {
        float size = 5 * (rand() % 90) / 90.f;
        Rectangle cube = make_cube(size);
        float tx = (rand() % 300 - 150) / 10.f;
        float ty = (rand() % 300 - 150) / 10.f;
        float tz = -(rand() % 300) / 10.f;
        cube.transform = translate(cube.transform, {tx, ty, tz});
        float r = (rand() % 900 + 100) / 1000.f;
        float g = (rand() % 900 + 100) / 1000.f;
        float b = (rand() % 900 + 100) / 1000.f;
        cube.color = {r, g, b};

        world.rectangles.push_back(cube);
    }

//    auto cube = make_cube(5);
//    world.rectangles.push_back({cube});

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

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        render_controls.draw_axes = !render_controls.draw_axes;
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
