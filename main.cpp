#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <utility>

#include "axes.h"
#include "buffer.h"
#include "logging.h"
#include "mesh.h"
#include "objects.h"
#include "timer.h"
#include "world.h"

template <typename A, typename B> struct Pair {
    A a;
    B b;
};

std::vector<Pair<Mesh, ObjectType>> ground_mesh() {
    Vec3 a0 = {0, 0, 0};
    Vec3 b0 = {1, 0, 0};
    Vec3 normal0 = {0, -1, 0};
    Face face = face_from_line(a0, b0, normal0);
    Mesh tetra0 = tetra_from_face(face.a, face.b, face.c);
    return {{tetra0, ObjectType::Tetrahedron}};
}

unsigned int add_ground() {
    PolyObject ground;
    auto meshs = ground_mesh();
    for (int i = 0; i < meshs.size(); ++i) {
        auto [mesh, type] = meshs[i];
        TetraOcta part = make_tetra_or_octa(mesh, type);
        world.tetraoctas.push_back(part);
        ground.parts.push_back(world.tetraoctas.size() - 1);
    }
    world.objects.push_back(ground);
    return world.objects.size() - 1;
}

void display() {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (world.render_controls.draw_axes) {
        draw(world.axes, world.camera);
    }

    //    for (const auto &rect : world.rectangles) {
    //        draw(rect.rendering, rect.obj, world.camera);
    //    }

    for (int i = 0; i < world.tetraoctas.size(); ++i) {
        SolidObjectProperties obj = world.tetraoctas[i].obj;
        if (world.editor && i == world.editor->selected.target_index) {
            obj.highlighted_face = world.editor->selected.face_index;
        }
        draw(world.tetraoctas[i].rendering, obj, world.camera);
    }

    if (world.editor && world.editor->phantom_object) {
        TetraOcta object = *world.editor->phantom_object;
        draw(object.rendering, object.obj, world.camera);
    }

    camera::draw();
}

void update(float dt) {
    update(world.camera, dt);
    if (world.editor) {
        editor::update(dt);
    }
    teleportation::update_target();
}

void init() {
    //    buffers = {make_surface(10, 10)};
    //
    //    for (int i = 0; i < 100; ++i) {
    //        float size = 5 * (rand() % 90) / 90.f;
    //        Rectangle cube = make_cube(size);
    //        float tx = (rand() % 300 - 150) / 10.f;
    //        float ty = (rand() % 300 - 150) / 10.f;
    //        float tz = -(rand() % 300) / 10.f;
    //        cube.obj.transform = translate(cube.obj.transform, {tx, ty, tz});
    //        float r = (rand() % 900 + 100) / 1000.f;
    //        float g = (rand() % 900 + 100) / 1000.f;
    //        float b = (rand() % 900 + 100) / 1000.f;
    //        cube.obj.color = {r, g, b};
    //
    //        world.rectangles.push_back(cube);
    //    }

    //    auto cube = make_cube(5);
    //    world.rectangles.push_back({cube});

    //    world.tetraoctas = {make_tetra()};
    //    world.tetraoctas = {make_octa()};
    add_ground();
    world.axes = make_axes();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }

    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            world.camera.controls.move_right = false;
            world.camera.controls.move_left = true;
        } else if (action == GLFW_RELEASE) {
            world.camera.controls.move_left = false;
        }
    }

    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            world.camera.controls.move_left = false;
            world.camera.controls.move_right = true;
        } else if (action == GLFW_RELEASE) {
            world.camera.controls.move_right = false;
        }
    }

    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            world.camera.controls.move_backwards = false;
            world.camera.controls.move_forward = true;
        } else if (action == GLFW_RELEASE) {
            world.camera.controls.move_forward = false;
        }
    }

    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            world.camera.controls.move_forward = false;
            world.camera.controls.move_backwards = true;
        } else if (action == GLFW_RELEASE) {
            world.camera.controls.move_backwards = false;
        }
    }

    if (key == GLFW_KEY_E) {
        if (action == GLFW_PRESS) {
            world.camera.controls.move_down = false;
            world.camera.controls.move_up = true;
        } else if (action == GLFW_RELEASE) {
            world.camera.controls.move_up = false;
        }
    }

    if (key == GLFW_KEY_Q) {
        if (action == GLFW_PRESS) {
            world.camera.controls.move_up = false;
            world.camera.controls.move_down = true;
        } else if (action == GLFW_RELEASE) {
            world.camera.controls.move_down = false;
        }
    }

    if (key == GLFW_KEY_LEFT_SHIFT) {
        if (action == GLFW_PRESS) {
            world.camera.controls.move_faster = true;
        } else if (action == GLFW_RELEASE) {
            world.camera.controls.move_faster = false;
        }
    }

    if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
        world.render_controls.wireframe = !world.render_controls.wireframe;
        if (world.render_controls.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
        world.render_controls.draw_axes = !world.render_controls.draw_axes;
    }

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        if (world.editor) {
            world.editor = std::nullopt;
        } else {
            world.editor = Editor{};
        }
    }

    if (world.editor) {
        editor::keyboard_input(key, action);
    }
}

struct MouseDelta {
  public:
    MouseDelta() : m_already_moved(false) {}

    std::pair<float, float> get_delta(float xpos, float ypos) {
        if (!m_already_moved) {
            m_already_moved = true;
            m_prev_x = xpos;
            m_prev_y = ypos;
        }
        float dx = xpos - m_prev_x;
        float dy = m_prev_y - ypos;
        m_prev_x = xpos;
        m_prev_y = ypos;
        return {dx, dy};
    }

  private:
    float m_prev_x;
    float m_prev_y;
    bool m_already_moved;
};

MouseDelta mouse_delta;

Timer mouse_throttle;

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    if (mouse_throttle.seconds_elapsed() < 0.016)
        return;
    mouse_throttle.tick();

    auto [dx, dy] = mouse_delta.get_delta(xpos, ypos);
    world.camera.controls.dx = dx;
    world.camera.controls.dy = dy;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (world.editor) {
        editor::mouse_button_input(button, action);
    } else {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                teleportation::initiate();
            } else if (action == GLFW_RELEASE) {
                teleportation::confirm();
            }
        }
    }
}

class FPSCounter {
  public:
    FPSCounter() : m_last_dt(1) {}

    void tick(float dt) { m_last_dt = dt; }

    float fps() const { return 1.f / m_last_dt; }

  private:
    float m_last_dt;
};

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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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
