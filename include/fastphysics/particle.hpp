#pragma once

#include "fastphysics/vec3.hpp"

#include <vector>

namespace fastphysics {
    
// AoS
// A single particle ocupies more than one chache line of 64 bytes. 
// Vec3 ~ 24 bytes and double ~ 8 bytes, so 80 bytes.
struct Particle {
    Vec3 position{};
    Vec3 velocity{};
    Vec3 acceleration{};
    double mass{};
};

}