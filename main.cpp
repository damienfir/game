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
        if (i == world.editor.selected.target_index) {
            obj.highlighted_face = world.editor.selected.face_index;
        }
        draw(world.tetraoctas[i].rendering, obj, world.camera);
    }

    if (world.editor.phantom_object) {
        TetraOcta object = *world.editor.phantom_object;
        draw(object.rendering, object.obj, world.camera);
    }

    camera::draw();
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
    Vec3 pos = r.obj.transform * r.center;

    if (norm(sphere.pos - pos) - sphere.radius >
        norm({r.width / 2.f, r.height / 2.f, r.depth / 2.f})) {
        return d;
    }

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
    }
    return d;
}

void update(float dt) {
    update(world.camera, dt);
    editor::mouse_pick();
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

    world.tetraoctas = {make_tetra()};
    //    world.tetraoctas = {make_octa()};
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

    if (key == GLFW_KEY_LEFT_CONTROL) {
        if (action == GLFW_PRESS) {
            world.camera.controls.move_around = true;
        } else if (action == GLFW_RELEASE) {
            world.camera.controls.move_around = false;
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

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        editor::add_to_selected_face(editor::ObjectType::Tetrahedron);
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        editor::add_to_selected_face(editor::ObjectType::Octahedron);
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        editor::remove_selected_object();
    }

    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
        editor::undo();
    }

    if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
        editor::redo();
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
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            teleportation::initiate();
        } else if (action == GLFW_RELEASE) {
            teleportation::confirm();
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
    int width = 1024;

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
