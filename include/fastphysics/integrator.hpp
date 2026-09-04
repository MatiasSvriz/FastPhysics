#pragma once

#include "fastphysics/particle.hpp"

#include <vector>

namespace fastphysics {

// Advances all particles by one time step using the explicit Euler method.
// Accelerations are assumed to have been computed for the current state.
void euler_step(
    std::vector<Particle>& particles,
    double dt
);

}