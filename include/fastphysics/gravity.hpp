#pragma once

#include "fastphysics/particle.hpp"

#include <vector>

namespace fastphysics {

// Recomputes the gravitational acceleration of every particle in the system.
// The particle collection is mutable because each Particle::acceleration is overwritten with the acceleration produced by the current system state.
void compute_accelerations(
    std::vector<Particle>& particles,
    double gravitational_constant,
    double softening
);

}