#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <optional>
#include <utility>

#include "axes.h"
#include "buffer.h"
#include "logging.h"
#include "mesh2.h"
#include "physics.h"
#include "timer.h"

int window_width = 1920;
int window_height;
float window_ratio = 16.f / 9.f;

struct DebugControls {
    bool wireframe = false;
    bool draw_axes = false;
    bool show_normals = false;
};

struct Teleportation {
    bool visualize;
    Vec3 target;
};

struct Entity {
    Mesh mesh;
    BasicRenderingBuffer rendering;
    Vec3 color;
    Mat4 transform;
};

struct Editor {
    bool enabled = true;
    float mouse_pos_x;
    float mouse_pos_y;
    int selected = -1;
};

struct World {
    std::vector<Entity> entities;
    Teleportation teleportation;
    Camera camera;
    Axes axes;
    DebugControls debug_controls;
    Editor editor;
};

World world;

Entity make_floor() {
    Entity body;
    body.mesh = floor_mesh(100, 100);
    body.color = {0.2, 0.2, 0.2};
    body.transform = eye();
    body.rendering = init_rendering(body.mesh);
    return body;
}

Entity make_entity(Mesh mesh) {
    Entity body;
    body.mesh = mesh;
    body.color = {0.5, 0.2, 0.5};
    body.transform = eye();
    body.rendering = init_rendering(mesh);
    return body;
}

void initiate_teleportation() { world.teleportation.visualize = true; }

struct Ray {
    Vec3 origin;
    Vec3 direction;
};

struct IntersectInfo {
    Vec3 point;
    int entity_index;
};

template <typename T> bool contains(const std::vector<T> &v, const T &val) {
    return std::find(std::begin(v), std::end(v), val) != std::end(v);
}

std::optional<IntersectInfo> find_point_on_object(const Ray &ray,
                                                  const std::vector<int> &exclude = {}) {
    IntersectInfo info;
    float min_t = std::numeric_limits<float>::max();
    for (int i = 0; i < world.entities.size(); ++i) {
        if (contains(exclude, i))
            continue;

        Mat4 model_transform = world.entities[i].transform;
        Mat4 normal_transform = transpose(invert(model_transform));
        Mesh mesh = world.entities[i].mesh;
        int n_faces = mesh.vertices.size() / 3;
        for (int face_index = 0; face_index < n_faces; ++face_index) {
            // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
            Vec3 normal = normal_transform * mesh.normals[face_index * 3];
            if (dot(normal, ray.direction) >= 0) {
                // triangle is facing away from ray
                continue;
            }

            Vec3 v0 = model_transform * mesh.vertices[face_index * 3];
            Vec3 v1 = model_transform * mesh.vertices[face_index * 3 + 1];
            Vec3 v2 = model_transform * mesh.vertices[face_index * 3 + 2];

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
                    info = IntersectInfo{.point = point_intersect, .entity_index = i};
                }
            } else {
            }
        }
    }
    return info;
}

void update_teleportation() {
    if (world.teleportation.visualize) {
        //        world.teleportation.target = world.camera.position() + world.camera.direction() *
        //        10;
        Ray ray = {.origin = world.camera.position(), .direction = world.camera.direction()};
        if (auto intersection = find_point_on_object(ray)) {
            world.teleportation.target = intersection->point;
        } else {
            world.teleportation.target = {0, 0, 0};
        }
    }
}

void confirm_teleportation() {
    world.teleportation.visualize = false;
    world.camera.set_position(world.teleportation.target + Vec3{0, 1, 0});
}

void draw_teleportation() {
    if (world.teleportation.visualize) {
        //        log(string(world.teleportation.target));
    }
}

// void draw_floor() {
//     RenderingParameters param = {.color = world.floor.color,
//                                  .model_transform = world.floor.transform,
//                                  .view_transform = world.camera.view(),
//                                  .perspective_transform = world.camera.projection(),
//                                  .camera_position = world.camera.position()};
//     draw(world.floor.rendering, param);
// }

void draw_entities() {
    for (Entity &entity : world.entities) {
        RenderingParameters param = {.color = entity.color,
                                     .model_transform = entity.transform,
                                     .view_transform = world.camera.view(),
                                     .perspective_transform = world.camera.projection(),
                                     .camera_position = world.camera.position(),
                                     .show_normals = world.debug_controls.show_normals,
                                     .teleportation_target = world.teleportation.target,
                                     .show_teleportation = world.teleportation.visualize};
        //        if (world.editor.enabled) {
        //            param.teleportation_target = world.editor.selected_point;
        //            param.show_teleportation = true;
        //        }
        draw(entity.rendering, param);
    }
}

void draw_middle_point() {
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3d(1, 1, 1);
    glVertex3d(0, 0, 0);
    glEnd();
}

void update_camera_position(Camera &camera, float dt) {
    Vec3 velocity;

    if (camera.controls.move_right) {
        velocity = camera.move_horizontal();
    } else if (camera.controls.move_left) {
        velocity = 0 - camera.move_horizontal();
    }

    if (camera.controls.move_backwards) {
        velocity -= camera.move_towards_restricted();
    } else if (camera.controls.move_forward) {
        velocity += camera.move_towards_restricted();
    }

    if (camera.controls.move_up) {
        velocity += camera.move_vertical();
    } else if (camera.controls.move_down) {
        velocity -= camera.move_vertical();
    }

    if (camera.controls.move_faster)
        velocity *= 2.f;

    //    velocity.y -= 100 * dt; // Fake gravity, should accumulate
    velocity *= dt;

    float radius = 0.3;
    if (norm(velocity) > 0) {
        for (int entity_index = 0; entity_index < world.entities.size(); ++entity_index) {
            const Mesh &mesh = world.entities[entity_index].mesh;
            Mat4 model_transform = world.entities[entity_index].transform;
            Mat4 normal_transform = transpose(invert(model_transform));
            for (int face_index = 0; face_index < mesh.vertices.size() / 3; ++face_index) {
                Vec3 position = camera.position() + velocity;
                Vec3 v0 = model_transform * mesh.vertices[face_index * 3];
                Vec3 v1 = model_transform * mesh.vertices[face_index * 3 + 1];
                Vec3 v2 = model_transform * mesh.vertices[face_index * 3 + 2];
                Vec3 n = normal_transform * mesh.normals[face_index * 3];

                Vec3 face_center = centroid(v0, v1, v2);
                float circumsphere_radius = norm(v0 - face_center);
                if (norm(position - face_center) < circumsphere_radius) {
                    Vec3 p0 = position - v0;
                    Vec3 p1 = position - v1;
                    Vec3 p2 = position - v2;
                    if (dot(p0, v1 - v0) > 0 && dot(p1, v2 - v1) > 0 && dot(p2, v0 - v2) > 0) {
                        float distance = dot(p0, n);
                        if (distance - radius < 0 &&
                            distance > 0 // Assumption that we will never be inside an object...
                        ) {
                            velocity -= n * (distance - radius);
                        }
                    }
                }
            }
        }
    }

    camera.set_position(camera.position() + velocity);
}

void toggle_editor() {
    if (world.editor.enabled) {
        world.debug_controls.draw_axes = false;
        world.editor.enabled = false;
    } else {
        world.debug_controls.draw_axes = true;
        world.editor.enabled = true;
    }
}

std::pair<float, float> screen_to_clip(float x, float y) {
    float xd = (x / window_width - 0.5f) * 2;
    float yd = 1 - (y / window_height - 0.5f) * 2;
    return {xd, yd};
}

Ray ray_from_camera() {
    auto [xd, yd] = screen_to_clip(world.editor.mouse_pos_x, world.editor.mouse_pos_y);

    // https://antongerdelan.net/opengl/raycasting.html
    Vec4 ray_clip = {xd, yd, -1.f, 1.f};
    Vec4 ray_eye = invert(world.camera.projection()) * ray_clip;
    Vec4 ray_world = invert(world.camera.view()) * Vec4{ray_eye.x, ray_eye.y, -1.f, 0.f};
    Vec3 ray_dir = normalize({ray_world.x, ray_world.y, ray_world.z});

    Ray ray = {.origin = world.camera.position(), .direction = ray_dir};
    return ray;
}

void editor_update_selected() {
    int selected = world.editor.selected;
    if (selected >= 0) {
        auto intersection = find_point_on_object(ray_from_camera(), {selected});
        if (intersection) {
            Vec3 target = intersection->point;
            world.entities[selected].transform = translate(eye(), {target});
        }
    }
}

void editor_initiate_move() {
    auto intersection = find_point_on_object(ray_from_camera());
    if (intersection) {
        world.editor.selected = intersection->entity_index;
    } else {
        world.editor.selected = -1;
    }
}

void editor_confirm_move() { world.editor.selected = -1; }

void update_fpv_view(Camera &camera) {
    camera.rotate_direction(camera.controls.dx, camera.controls.dy);
    camera.controls.dx = 0;
    camera.controls.dy = 0;
}

void display() {
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //    draw_floor();

    if (world.debug_controls.draw_axes) {
        draw(world.axes, world.camera);
    }

    draw_entities();

    if (world.editor.enabled) {
        glPointSize(5);
        glBegin(GL_POINTS);
        glColor3d(1, 1, 1);
        auto [xd, yd] = screen_to_clip(world.editor.mouse_pos_x, world.editor.mouse_pos_y);
        glVertex3d(xd, yd, 0);
        glEnd();
    } else {
        draw_teleportation();
    }

    //    draw_middle_point();
}

void update(float dt) {
    if (!world.editor.enabled) {
        update_camera_position(world.camera, dt);
        update_fpv_view(world.camera);
        update_teleportation();
    }
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

    world.entities.push_back(make_floor());

    Entity rect1 = make_entity(rectangle_mesh(2, 1, 2));
    rect1.color = {0.1, 0.7, 0.2};
    rect1.transform = translate(eye(), {3, 0, 0});
    world.entities.push_back(rect1);

    Entity rect2 = make_entity(rectangle_mesh(3, 1, 1));
    rect2.color = {0.1, 0.2, 0.7};
    rect2.transform = translate(eye(), {-3, 0, 0});
    world.entities.push_back(rect2);


    //    world.floor = make_floor();
    world.axes = make_axes();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void game_key_callback(int key, int action) {
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
}

void editor_key_callback(int key, int action) {}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }

    if (world.editor.enabled) {
        editor_key_callback(key, action);
    } else {
        game_key_callback(key, action);
    }

    if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
        world.debug_controls.wireframe = !world.debug_controls.wireframe;
        if (world.debug_controls.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
        world.debug_controls.draw_axes = !world.debug_controls.draw_axes;
    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        world.debug_controls.show_normals = !world.debug_controls.show_normals;
    }

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        toggle_editor();
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

    if (world.editor.enabled) {
        //        float speed = 3;
        // FIXME: add acceleration (to move pixel-by-pixel when slow)
        world.editor.mouse_pos_x = xpos;
        world.editor.mouse_pos_y = ypos;
        editor_update_selected();
    } else {
        world.camera.controls.dx = dx;
        world.camera.controls.dy = dy;
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (world.editor.enabled) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                editor_initiate_move();
            } else {
                editor_confirm_move();
            }
        }
    } else {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                initiate_teleportation();
            } else if (action == GLFW_RELEASE) {
                confirm_teleportation();
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
    window_height = window_width / window_ratio;

    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(window_width, window_height, "Game", NULL, NULL);
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
