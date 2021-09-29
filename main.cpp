#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <optional>
#include <unordered_map>
#include <utility>

#include "axes.h"
#include "buffer.h"
#include "logging.h"
#include "mesh2.h"
#include "physics.h"
#include "timer.h"

int window_width = 1024;
int window_height;
float window_ratio = 16.f / 9.f;

struct DebugControls {
    bool wireframe = false;
    bool draw_axes = true;
    bool show_normals = false;
};

struct Teleportation {
    int target;
};

struct Entity {
    Mesh mesh;
    BasicRenderingBuffer rendering;
    Vec3 color;
    Mat4 transform;
    Vec3 origin;
};

struct Editor {
    bool enabled = false;
    float mouse_pos_x;
    float mouse_pos_y;
    int selected = -1;
    Vec3 selected_point;
};

struct CellProperties {
    int axis;
};

struct Cell {
    enum Type {
        Start,
        End,
        Floor,
        Hole, // axis: X, Y or Z
        Wall,
        Mirror, // normal vector will define the orientation
        Hedge,
        Platform,
        RaisedPlatform
    };
    Type type = Type::Floor;
    CellProperties prop;
    Entity entity;
};

struct Grid {
    //    std::vector<Entity> entities;
    std::vector<Cell> cells;
    int rows;
    int cols;
    int start;
    int end;
};

struct World {
    //    std::vector<Entity> entities;
    Teleportation teleportation;
    Camera camera;
    Axes axes;
    DebugControls debug_controls;
    Editor editor;
    Grid grid;
};

World world;

Entity make_floor() {
    Entity body;
    body.mesh = floor_mesh(100, 100);
    body.color = {0.2, 0.2, 0.2};
    body.transform = eye();
    body.rendering = init_rendering(body.mesh);
    body.origin = {0, 0, 0};
    return body;
}

Entity make_entity(Mesh mesh, Vec3 color = {0.5, 0.5, 0.5}) {
    Entity body;
    body.mesh = mesh;
    body.color = color;
    body.transform = eye();
    body.rendering = init_rendering(mesh);
    body.origin = {0, 0, 0};
    return body;
}

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

std::optional<IntersectInfo> find_point_on_grid(const Ray &ray) {
    std::optional<IntersectInfo> info;
    float min_t = std::numeric_limits<float>::max();
    for (int i = 0; i < world.grid.cells.size(); ++i) {
        const Cell &cell = world.grid.cells[i];
        Mat4 model_transform = cell.entity.transform;
        Mat4 normal_transform = transpose(invert(model_transform));
        Mesh mesh = cell.entity.mesh;
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

// void confirm_teleportation() {
//     world.camera.set_position(world.teleportation.target + Vec3{0, 1, 0});
// }

// void draw_entities() {
//     for (Entity &entity : world.entities) {
//         RenderingParameters param = {.color = entity.color,
//                                      .model_transform = entity.transform,
//                                      .view_transform = world.camera.view(),
//                                      .perspective_transform = world.camera.projection(),
//                                      .camera_position = world.camera.position(),
//                                      .show_normals = world.debug_controls.show_normals,
//                                      .teleportation_target = world.teleportation.target,
//                                      .show_teleportation = world.teleportation.visualize};
//         //        if (world.editor.enabled) {
//         //            param.teleportation_target = world.editor.selected_point;
//         //            param.show_teleportation = true;
//         //        }
//         draw(entity.rendering, param);
//     }
// }

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

    //    float radius = 0.3;
    //    if (norm(velocity) > 0) {
    //        for (int entity_index = 0; entity_index < world.entities.size(); ++entity_index) {
    //            const Mesh &mesh = world.entities[entity_index].mesh;
    //            Mat4 model_transform = world.entities[entity_index].transform;
    //            Mat4 normal_transform = transpose(invert(model_transform));
    //            for (int face_index = 0; face_index < mesh.vertices.size() / 3; ++face_index) {
    //                Vec3 position = camera.position() + velocity;
    //                Vec3 v0 = model_transform * mesh.vertices[face_index * 3];
    //                Vec3 v1 = model_transform * mesh.vertices[face_index * 3 + 1];
    //                Vec3 v2 = model_transform * mesh.vertices[face_index * 3 + 2];
    //                Vec3 n = normal_transform * mesh.normals[face_index * 3];
    //
    //                Vec3 face_center = centroid(v0, v1, v2);
    //                float circumsphere_radius = norm(v0 - face_center);
    //                if (norm(position - face_center) < circumsphere_radius) {
    //                    Vec3 p0 = position - v0;
    //                    Vec3 p1 = position - v1;
    //                    Vec3 p2 = position - v2;
    //                    if (dot(p0, v1 - v0) > 0 && dot(p1, v2 - v1) > 0 && dot(p2, v0 - v2) > 0)
    //                    {
    //                        float distance = dot(p0, n);
    //                        if (distance - radius < 0 &&
    //                            distance > 0 // Assumption that we will never be inside an
    //                            object...
    //                        ) {
    //                            velocity -= n * (distance - radius);
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }

    camera.set_position(camera.position() + velocity);
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

void toggle_editor() {
    if (world.editor.enabled) {
        world.debug_controls.draw_axes = false;
        world.editor.enabled = false;
    } else {
        world.debug_controls.draw_axes = true;
        world.editor.enabled = true;
    }
}

// void editor_update_selected() {
//     int selected = world.editor.selected;
//     if (selected >= 0) {
//         Entity entity = world.entities[selected];
//         auto intersection = find_point_on_object(ray_from_camera(), {selected});
//         if (intersection) {
//             Vec3 target = intersection->point;
//             world.entities[selected].transform = translate(eye(), target);
//         }
//     }
// }
//
// void editor_initiate_move() {
//     auto intersection = find_point_on_object(ray_from_camera());
//     if (intersection) {
//         world.editor.selected = intersection->entity_index;
//         world.editor.selected_point = intersection->point;
//     } else {
//         world.editor.selected = -1;
//     }
// }

void editor_confirm_move() { world.editor.selected = -1; }

void update_fpv_view(Camera &camera) {
    camera.rotate_direction(camera.controls.dx, camera.controls.dy);
    camera.controls.dx = 0;
    camera.controls.dy = 0;
}

std::optional<Vec3> find_point_on_plane(Ray ray) {
    Vec3 normal = {0, 1, 0};
    if (dot(ray.direction, normal) >= 0) {
        return std::nullopt;
    }

    float plane_distance = 0;
    float t = -(dot(normal, ray.origin) - plane_distance) / dot(normal, ray.direction);
    return ray.origin + ray.direction * t;
}

Vec3 coord_at(const Grid &grid, int index) {
    float row = std::floor(index / grid.cols);
    float col = index % grid.cols;
    Vec3 cell_origin = {0.5f, 0.f, -0.5f};
    return Vec3{col, 0.f, -row} + cell_origin;
}

int index_at(const Grid &grid, int row, int col) { return row * grid.cols + col; }

Vec3 coord_at(const Grid &grid, int row, int col) {
    return coord_at(grid, index_at(grid, row, col));
}

int index_at(const Grid &grid, Vec3 coord) {
    int col = std::floor(coord.x);
    int row = std::floor(-coord.z);
    return index_at(grid, row, col);
}

bool can_teleport_here(Cell::Type type) {
    return (type == Cell::Type::Floor) || (type == Cell::Type::Start) || (type == Cell::Type::End);
}

void update_teleportation() {
    Ray ray = {.origin = world.camera.position(), .direction = world.camera.direction()};
    auto point = find_point_on_grid(ray);
    if (point && can_teleport_here(world.grid.cells[point->entity_index].type)) {
        world.teleportation.target = point->entity_index;
    } else {
        world.teleportation.target = -1;
    }
}

void confirm_teleportation() {
    if (world.teleportation.target >= 0) {
        log(world.teleportation.target);
        world.camera.set_position(coord_at(world.grid, world.teleportation.target));
    }
}

RenderingParameters render_params(const Entity &entity) {
    return {.color = entity.color,
            .model_transform = entity.transform,
            .view_transform = world.camera.view(),
            .perspective_transform = world.camera.projection(),
            .camera_position = world.camera.position(),
            .show_normals = world.debug_controls.show_normals};
}

void draw_grid() {
    for (int i = 0; i < world.grid.cells.size(); ++i) {
        const Entity &entity = world.grid.cells[i].entity;
        RenderingParameters params = render_params(entity);
        if (world.teleportation.target == i) {
            params.color = {1, 1, 1};
        }
        draw(entity.rendering, params);
    }
}

void display() {
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (world.debug_controls.draw_axes) {
        draw(world.axes, world.camera);
    }

    draw_grid();

    if (world.editor.enabled) {
        glPointSize(5);
        glBegin(GL_POINTS);
        glColor3d(1, 1, 1);
        auto [xd, yd] = screen_to_clip(world.editor.mouse_pos_x, world.editor.mouse_pos_y);
        glVertex3d(xd, yd, 0);
        glEnd();
    } else {
        //        draw_teleportation();
    }

    draw_middle_point();
}

void update(float dt) {
    if (!world.editor.enabled) {
        //        update_camera_position(world.camera, dt);
        update_fpv_view(world.camera);
        update_teleportation();
    }
}

Entity make_entity_from_cell(Cell::Type type, CellProperties prop) {
    switch (type) {
    case Cell::Type::Floor:
    case Cell::Type::Start:
        return make_entity(floor_tile_mesh(1, 1), {0.1, 0.8, 0.1});
    case Cell::Type::End:
        return make_entity(floor_tile_mesh(1, 1), {0.1, 0.4, 0.5});
    case Cell::Type::Wall: {
        if (prop.axis == 0) {
            return make_entity(rectangle_mesh(1, 5, 0.2));
        } else {
            return make_entity(rectangle_mesh(0.2, 5, 1));
        }
    }
    case Cell::Type::Hedge: {
        if (prop.axis == 0) {
            return make_entity(rectangle_mesh(1, 0.5, 0.2), {0.1, 0.5, 0.1});
        } else if (prop.axis == 2) {
            return make_entity(rectangle_mesh(0.2, 0.5, 1), {0.1, 0.5, 0.1});
        }
    }
    case Cell::Type::Platform: {
        return make_entity(rectangle_mesh(1, 0.5, 1), {0.1, 0.1, 0.8});
    }
    case Cell::Type::RaisedPlatform: {
        return make_entity(rectangle_mesh(1, 2, 1), {0.1, 0.1, 0.8});
    }
    }
}

void add_cell(Grid &grid, int row, int col, Cell::Type type, CellProperties prop = {}) {
    Entity entity = make_entity_from_cell(type, prop);
    entity.transform = translate(eye(), coord_at(grid, row, col));
    Cell cell = {
        .type = type,
        .prop = prop,
        .entity = entity,
    };
    grid.cells[index_at(grid, row, col)] = cell;
}

Grid make_grid_from_definition(std::string def, int rows, int cols) {
    Grid grid;
    grid.rows = rows;
    grid.cols = cols;
    grid.cells.resize(rows * cols);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int index = row * cols + col;
            std::string s = def.substr(index * 2, 2);

            if (s == "  ") {
                add_cell(grid, row, col, Cell::Type::Floor);
            } else if (s == "==") {
                add_cell(grid, row, col, Cell::Type::Wall, {.axis = 0});
            } else if (s == "||") {
                add_cell(grid, row, col, Cell::Type::Wall, {.axis = 2});
            } else if (s == "--") {
                add_cell(grid, row, col, Cell::Type::Hedge, {.axis = 0});
            } else if (s == "| ") {
                add_cell(grid, row, col, Cell::Type::Hedge, {.axis = 2});
            } else if (s == "TT") {
                add_cell(grid, row, col, Cell::Type::RaisedPlatform);
            } else if (s == "__") {
                add_cell(grid, row, col, Cell::Type::Platform);
            } else if (s == "a ") {
                add_cell(grid, row, col, Cell::Type::Start);
                grid.start = index;
            } else if (s == "z ") {
                add_cell(grid, row, col, Cell::Type::End);
                grid.end = index;
            } else {
                throw std::runtime_error("Unknown cell: '" + s + "' at location (" +
                                         std::to_string(row) + ", " + std::to_string(col) + ")");
            }
        }
    }
    return grid;
}

Grid make_grid1() {
    std::string def = "||============||"
                      "||a   ||      ||"
                      "||    ||      z "
                      "||    |       ||"
                      "||============||";
    return make_grid_from_definition(def, 5, 8);
}

void init() {
    world.grid = make_grid1();
    world.camera.set_position(coord_at(world.grid, world.grid.start));
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
        //        game_key_callback(key, action);
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
        //        editor_update_selected();
    } else {
        world.camera.controls.dx = dx;
        world.camera.controls.dy = dy;
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (world.editor.enabled) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                //                editor_initiate_move();
            } else {
                //                editor_confirm_move();
            }
        }
    } else {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                confirm_teleportation();
            } else if (action == GLFW_RELEASE) {
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
