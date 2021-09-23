#pragma once

#include "camera.h"
#include "editor.h"
#include "axes.h"
#include "objects.h"
#include "teleportation.h"

struct RenderControls {
    bool wireframe = false;
    bool draw_axes = true;
};

struct World {
//    std::vector<Rectangle> rectangles;
    std::vector<TetraOcta> tetraoctas;
    editor::SelectedFace selected;
    Camera camera;
    Axes axes;
    RenderControls render_controls;
    Teleportation teleportation;
};

extern World world;