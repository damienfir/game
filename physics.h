#pragma once

#include "maths.h"

struct Sphere {
    Vec3 pos;
    float radius;
};

struct IntersectData {
    bool intersected = false;
    Vec3 normal;
};

//IntersectData intersect_tetras_point(Sphere sphere);