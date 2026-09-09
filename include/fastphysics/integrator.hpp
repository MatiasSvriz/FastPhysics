#pragma once

#include "fastphysics/particle.hpp"

#include <vector>

namespace fastphysics {

// Advances all particles by one time step using the explicit Euler method.
// Accelerations are assumed to correspond to the current particle positions.
void euler_step(
    std::vector<Particle>& particles,
    double dt
);

// Velocity Verlet advances the system using a kick-drift-kick scheme:
// 1. Update velocity by half a time step using the current acceleration.
// 2. Advance positions using the intermediate velocity.
// 3. Recompute accelerations at the new positions.
// 4. Complete the velocity update using the new acceleration.

// Applies the first half velocity update and advances positions.
// Accelerations must correspond to the current particle positions.
void velocity_verlet_begin_step(
    std::vector<Particle>& particles,
    double dt
);

// Completes the velocity update after accelerations have been recomputed for the new particle positions.
void velocity_verlet_end_step(
    std::vector<Particle>& particles,
    double dt
);

}