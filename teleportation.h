#pragma once

#include "maths.h"

struct Teleportation {
    bool visualize;
    Vec3 target;
};

void update_teleportation() {
    if (world.teleportation.visualize) {
        world.teleportation.target = world.camera.position() + world.camera.direction() * 10;
    }
}

void confirm_teleportation() {
    world.teleportation.visualize = false;
    world.camera.set_position(world.teleportation.target);
}

namespace teleportation {
void initiate();
void update_target();
void confirm();
}
