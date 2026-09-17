#pragma once

#include "fastphysics/particle.hpp"
#include "fastphysics/particle_system_soa.hpp"

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

// Reference gravity solver using Structure of Arrays.
// It performs the same N * (N - 1) interactions as compute_accelerations(),
// but particle data is stored in separate contiguous arrays.
void compute_accelerations_soa(
    ParticleSystemSoA& particles,
    double gravitational_constant,
    double softening
);

// Pairwise gravity solver using Structure of Arrays.
// Each unique particle pair is evaluated once, reducing the number of pair
// evaluations to N * (N - 1) / 2 while keeping particle data in separate contiguous arrays.
void compute_accelerations_pairwise_soa(
    ParticleSystemSoA& particles,
    double gravitational_constant,
    double softening
);

}