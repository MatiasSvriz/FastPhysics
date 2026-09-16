#pragma once

#include "fastphysics/particle.hpp"

#include <vector>

namespace fastphysics {

// Recomputes the gravitational acceleration of every particle.
// The collection is mutable because Particle::acceleration is overwritten.
void compute_accelerations(
    std::vector<Particle>& particles,
    double gravitational_constant,
    double softening
);

// Computes gravitational accelerations by evaluating each particle pair once.
// Uses interaction symmetry to avoid duplicate i-j and j-i calculations.
void compute_accelerations_pairwise(
    std::vector<Particle>& particles,
    double gravitational_constant,
    double softening
);

}