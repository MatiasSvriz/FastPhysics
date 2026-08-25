#pragma once

#include "fastphysics/vec3.hpp"

namespace fastphysics {

struct Particle {
    Vec3 position{};
    Vec3 velocity{};
    Vec3 acceleration{};
    double mass{};
};

}