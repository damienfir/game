#pragma once

class World;

namespace editor {

struct SelectedFace {
    int target_index = -1;
    int face_index;
};

enum ObjectType { Tetrahedron, Octahedron };

void mouse_pick();
void add_to_selected_face(ObjectType type);
void redo();
void remove_selected_object();
void undo();

} // namespace editor
