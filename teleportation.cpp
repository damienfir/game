#include "teleportation.h"
#include "logging.h"
#include "world.h"

namespace teleportation {

void initiate() { world.teleportation.visualize = true; }

void update_target() {
    if (world.teleportation.visualize) {
        world.teleportation.target = world.camera.position() + world.camera.direction() * 10;
    }
}

void confirm() {
    world.teleportation.visualize = false;
    world.camera.set_position(world.teleportation.target);
}

} // namespace teleportation
