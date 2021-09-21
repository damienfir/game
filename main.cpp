#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
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

struct Tetra {
    Mesh mesh;
    SolidObjectProperties obj;
    BasicRenderingBuffer rendering;
};

struct Octa {
    Mesh mesh;
    SolidObjectProperties obj;
    BasicRenderingBuffer rendering;
};

Vec3 normal_for_face(Vec3 a, Vec3 b, Vec3 c) {
    Vec3 v = b - a;
    Vec3 w = c - a;
    Vec3 n = normalize(cross(v, w)); // verified correct order
    return n;
}

std::vector<Vec3> compute_normals(const std::vector<Vec3> &vertices) {
    // assuming the normals are packed by faces
    std::vector<Vec3> normals;
    normals.reserve(vertices.size());
    for (int i = 0; i < vertices.size(); i += 3) {
        Vec3 n = normal_for_face(vertices[i], vertices[i + 1], vertices[i + 2]);
        normals.push_back(n);
        normals.push_back(n);
        normals.push_back(n);
    }
    return normals;
}

Mesh tetra_mesh(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 v3) {
    Mesh mesh;
    mesh.vertices = {v0, v2, v3, v0, v1, v2, v0, v3, v1, v1, v3, v2};
    mesh.normals = compute_normals(mesh.vertices);
    return mesh;
}

Mesh tetra_from_face(Vec3 a, Vec3 b, Vec3 c) {
    Vec3 n = normal_for_face(a, b, c);
    Vec3 center = (a + b + c) / 3.f;
    Vec3 d = center + n * norm((c + (a - c) / 2.f - b));

    return tetra_mesh(a, c, b, d);
}

Tetra make_tetra(Mesh mesh) {
    Tetra t;
    t.mesh = mesh;
    t.obj.transform = eye();
    t.obj.color = {0.3, 0.3, 0.4};
    t.rendering = init_rendering(t.mesh);
    return t;
}

Tetra make_tetra() {
    Vec3 v0 = {1, 0, -1 / std::sqrt(2.f)};
    Vec3 v1 = {-1, 0, -1 / std::sqrt(2.f)};
    Vec3 v2 = {0, 1, 1 / std::sqrt(2.f)};
    Vec3 v3 = {0, -1, 1 / std::sqrt(2.f)};
    return make_tetra(tetra_from_face(v0, v1, v2));
}

Octa make_octa() {
    Vec3 top = {0, 1, 0};
    Vec3 bottom = {0, -1, 0};
    Vec3 front = {0, 0, 1};
    Vec3 back = {0, 0, -1};
    Vec3 left = {-1, 0, 0};
    Vec3 right = {1, 0, 0};

    Octa o;
    o.mesh.vertices = {right, top,    front, back, top,    right, left,   top,
                       back,  front,  top,   left, right,  front, bottom, back,
                       right, bottom, left,  back, bottom, front, left,   bottom};
    o.mesh.normals = compute_normals(o.mesh.vertices);
    //    for (Vec3 &v : o.mesh.vertices) {
    //        v *= 2.f / std::sqrt(2.f);
    //    }

    o.obj.transform = eye();
    o.obj.color = {0.3, 0.4, 0.3};
    o.rendering = init_rendering(o.mesh);

    return o;
}

Tetra tetra_from_octa_face(const Octa &octa, int face) {
    Vec3 a = octa.mesh.vertices[face * 3];
    Vec3 b = octa.mesh.vertices[face * 3 + 1];
    Vec3 c = octa.mesh.vertices[face * 3 + 2];
    return make_tetra(tetra_from_face(a, b, c));
}

struct SelectedFace {
    int target_index = -1;
    int face_index;
};

struct World {
    std::vector<Rectangle> rectangles;
    std::vector<Tetra> tetras;
    std::vector<Octa> octas;
    SelectedFace selected;
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

    for (const auto &rect : world.rectangles) {
        draw(rect.rendering, rect.obj, camera);
    }

    for (int i = 0; i < world.tetras.size(); ++i) {
        auto obj = world.tetras[i].obj;
        if (i == world.selected.target_index) {
            obj.color = {1, 1, 1};
        }
        draw(world.tetras[i].rendering, obj, camera);
    }

    for (const auto &octa : world.octas) {
        draw(octa.rendering, octa.obj, camera);
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

struct Ray {
    Vec3 origin;
    Vec3 direction;
};

SelectedFace find_selected_face(const Ray &ray) {
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
    SelectedFace selected;
    float min_t = std::numeric_limits<float>::max();
    for (int i = 0; i < world.tetras.size(); ++i) {
        Mesh mesh = world.tetras[i].mesh;
        for (int face_index = 0; face_index < 4; ++face_index) {
            Vec3 normal = mesh.normals[face_index * 3];
            if (dot(normal, ray.direction) >= 0) {
                // triangle is facing away from ray
                continue;
            }

            Vec3 v0 = mesh.vertices[face_index * 3];
            Vec3 v1 = mesh.vertices[face_index * 3 + 1];
            Vec3 v2 = mesh.vertices[face_index * 3 + 2];

            auto plane_distance = dot(normal, v0);
            float t = -(dot(normal, ray.origin) - plane_distance) / dot(normal, ray.direction);
            if (t <= 0) {
                // triangle is behind the ray
                continue;
            }

            Vec3 point_intersect = ray.origin + ray.direction * t;
            Vec3 e0 = v1 - v0;
            Vec3 e1 = v2 - v1;
            Vec3 e2 = v0 - v2;
            Vec3 c0 = point_intersect - v0;
            Vec3 c1 = point_intersect - v1;
            Vec3 c2 = point_intersect - v2;
            if (dot(normal, cross(e0, c0)) > 0 && dot(normal, cross(e1, c1)) > 0 &&
                dot(normal, cross(e2, c2)) > 0) {
                // point is inside the triangle
                if (t < min_t) {
                    min_t = t;
                    selected.target_index = i;
                    selected.face_index = face_index;
                }
            }
        }
    }

    return selected;
}

void mouse_pick() {
    Ray ray = {.origin = camera.position(), .direction = camera.direction()};
    world.selected = find_selected_face(ray);
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
            // standard collision response
            velocity -= d.normal * std::min(0.f, dot(d.normal, velocity));
        }
    }

    camera.set_position(camera.position() + velocity * dt);
}

void update(float dt) { update(camera, camera_controls, dt); }

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

    //    world.tetras = {make_tetra()};
    world.octas = {make_octa()};
s
    axes = make_axes();
    glEnable(GL_DEPTH_TEST);
}

enum ObjectType { Tetrahedron, Octahedron };

void add_to_selected_face(ObjectType type) {
    if (world.selected.target_index >= 0) {
        int face_index = world.selected.face_index;
        Tetra tetra = world.tetras[world.selected.target_index];
        Vec3 v0 = tetra.mesh.vertices[face_index * 3];
        Vec3 v1 = tetra.mesh.vertices[face_index * 3 + 1];
        Vec3 v2 = tetra.mesh.vertices[face_index * 3 + 2];
        if (type == ObjectType::Tetrahedron) {
            world.tetras.push_back(make_tetra(tetra_from_face(v0, v1, v2)));
        } else if (type == ObjectType::Octahedron) {
            //            world.octas.push_back(make_octa(octa_from_face(v0, v1, v2)));
        }
    }
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

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        add_to_selected_face(ObjectType::Tetrahedron);
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        add_to_selected_face(ObjectType::Octahedron);
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

    float rx = xpos - mouse.x;
    float ry = mouse.y - ypos;
    mouse.x = xpos;
    mouse.y = ypos;

    camera.rotate_direction(rx, ry);
    mouse_pick();
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
