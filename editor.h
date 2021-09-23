#pragma once

#include "objects.h"
#include <optional>

class World;

struct SelectedFace {
    int target_index = -1;
    int face_index;
};

struct Editor {
    std::optional<TetraOcta> phantom_object;
    SelectedFace selected;
    ObjectType target_type;
};

namespace editor {

void mouse_pick();
void add_to_selected_face(ObjectType type);
void redo();
void remove_selected_object();
void undo();
void keyboard_input(int key, int action);
void mouse_button_input(int button, int action);

} // namespace editor
