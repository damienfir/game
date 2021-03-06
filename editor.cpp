#include "editor.h"

#include <GLFW/glfw3.h>
#include <optional>

#include "maths.h"
#include "mesh2.h"
#include "objects.h"
#include "undoredo.h"
#include "world.h"
#include "logging.h"

namespace editor {

void select_face(SelectedFace selected) { world.editor->selected = selected; }

void unselected_face() { world.editor->selected.target_index = -1; }

int add_object(TetraOcta object, World &world) {
    world.tetraoctas.push_back(object);
    return world.tetraoctas.size() - 1;
}

void remove_object(int index, World &world) {
    world.tetraoctas.erase(std::begin(world.tetraoctas) + index);
}

// FIXME: use a better mechanism than changing the state of actions (in this case, to set the index
// after adding the object)
struct AddObjectAction {
    TetraOcta object;
    std::optional<int> index;
};

struct RemoveObjectAction {
    std::optional<TetraOcta> object;
    int index{-1};
};

using ActionType = std::variant<AddObjectAction, RemoveObjectAction>;

struct ActionApplyVisitor {
    void operator()(AddObjectAction &action) { action.index = add_object(action.object, world); }

    void operator()(RemoveObjectAction &action) {
        action.object = world.tetraoctas[action.index];
        remove_object(action.index, world);
    }
};

struct ActionUndoVisitor {
    void operator()(AddObjectAction &action) { remove_object(*action.index, world); }

    void operator()(RemoveObjectAction &action) {
        action.index = add_object(*action.object, world);
    }
};

UndoRedo<ActionType, ActionApplyVisitor, ActionUndoVisitor> action_system;

void emit(ActionType action) {
    action_system.add(action);
    action_system.apply_all_unapplied();
}

struct Ray {
    Vec3 origin;
    Vec3 direction;
};

SelectedFace find_selected_face(const Ray &ray) {
    SelectedFace selected;
    float min_t = std::numeric_limits<float>::max();
    for (int i = 0; i < world.tetraoctas.size(); ++i) {
        Mesh mesh = world.tetraoctas[i].mesh;
        int n_faces = mesh.vertices.size() / 3.f;
        for (int face_index = 0; face_index < n_faces; ++face_index) {
            // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
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

TetraOcta object_from_face(const TetraOcta &obj, int face_index, ObjectType type) {
    Vec3 v0 = obj.mesh.vertices[face_index * 3];
    Vec3 v2 = obj.mesh.vertices[face_index * 3 + 1];
    Vec3 v1 = obj.mesh.vertices[face_index * 3 + 2];
    Mesh mesh =
        type == ObjectType::Tetrahedron ? tetra_from_face(v0, v1, v2) : octa_from_face(v0, v1, v2);
    return make_tetra_or_octa(mesh, type);
}

void mouse_pick() {
    if (!world.camera.controls.move_around) {
        Ray ray = {.origin = world.camera.position(), .direction = world.camera.direction()};
        select_face(find_selected_face(ray));

        if (world.editor->selected.target_index != -1) {
            log(world.editor->selected.face_index);
            TetraOcta obj =
                object_from_face(world.tetraoctas[world.editor->selected.target_index],
                                 world.editor->selected.face_index, world.editor->target_type);
            obj.obj.alpha = 0.3;
            obj.obj.color = {0, 1, 0};
            world.editor->phantom_object = obj;
        } else {
            world.editor->phantom_object = std::nullopt;
        }
    }
}

void update(float dt) { mouse_pick(); }

void undo() { action_system.undo(); }

void redo() { action_system.redo(); }

void add_to_selected_face(ObjectType type) {
    if (world.editor->selected.target_index >= 0) {
        int face_index = world.editor->selected.face_index;
        TetraOcta obj = world.tetraoctas[world.editor->selected.target_index];
        TetraOcta new_obj = object_from_face(obj, face_index, type);
        emit(AddObjectAction{.object = new_obj});
        unselected_face();
    }
}

void remove_selected_object() {
    if (world.editor->selected.target_index >= 0 && world.tetraoctas.size() > 1) {
        emit(RemoveObjectAction{.index = world.editor->selected.target_index});
        unselected_face();
    }
}

void keyboard_input(int key, int action) {
    if (key == GLFW_KEY_LEFT_CONTROL) {
        if (action == GLFW_PRESS) {
            world.camera.controls.move_around = true;
        } else if (action == GLFW_RELEASE) {
            world.camera.controls.move_around = false;
        }
    }

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        world.editor->target_type = world.editor->target_type == ObjectType::Tetrahedron
                                        ? ObjectType::Octahedron
                                        : ObjectType::Tetrahedron;
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        remove_selected_object();
    }

    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
        undo();
    }

    if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
        redo();
    }
}

void mouse_button_input(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        add_to_selected_face(world.editor->target_type);
    }
}

}; // namespace editor
