#pragma once

#include "axes.h"
#include "camera.h"
#include "editor.h"
#include "objects.h"
#include "teleportation.h"

struct RenderControls {
    bool wireframe = false;
    bool draw_axes = true;
};

struct World {
    //    std::vector<Rectangle> rectangles;
    std::vector<TetraOcta> tetraoctas;
    std::vector<PolyObject> objects;
    Teleportation teleportation;
    std::optional<Editor> editor = Editor{};
    Camera camera;
    Axes axes;
    RenderControls render_controls;
};

extern World world;