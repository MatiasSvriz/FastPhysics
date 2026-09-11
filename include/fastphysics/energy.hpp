#pragma once

#include "fastphysics/particle.hpp"

#include <vector>

namespace fastphysics {

double kinetic_energy(
    const std::vector<Particle>& particles
);

double potential_energy(
    const std::vector<Particle>& particles,
    double gravitational_constant,
    double softening
);

double total_energy(
    const std::vector<Particle>& particles,
    double gravitational_constant,
    double softening
);

}