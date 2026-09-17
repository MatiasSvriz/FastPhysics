#pragma once

#include "fastphysics/particle.hpp"

#include <cstddef>
#include <vector>

namespace fastphysics {

// Stores particle data in separate arrays instead of grouping all the
// properties of one particle together.
//
// All values with the same meaning are contiguous in memory:
//
//     position_x = [x0, x1, x2, ...]
//     position_y = [y0, y1, y2, ...]
//     mass       = [m0, m1, m2, ...]
//
// Index i still refers to the same physical particle in every array.
struct ParticleSystemSoA {
    std::vector<double> position_x;
    std::vector<double> position_y;
    std::vector<double> position_z;

    std::vector<double> velocity_x;
    std::vector<double> velocity_y;
    std::vector<double> velocity_z;

    std::vector<double> acceleration_x;
    std::vector<double> acceleration_y;
    std::vector<double> acceleration_z;

    std::vector<double> mass;

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return mass.size();
    }
};

// Converts the current Array of Structures representation into a
// Structure of Arrays representation without changing the particle data.
ParticleSystemSoA make_particle_system_soa(
    const std::vector<Particle>& particles
);

}