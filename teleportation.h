#pragma once

#include "maths.h"

struct Teleportation {
    bool visualize;
    Vec3 target;
};

namespace teleportation {
void initiate();
void update_target();
void confirm();
}
