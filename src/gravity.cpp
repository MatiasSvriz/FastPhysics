#include "fastphysics/gravity.hpp"

#include "fastphysics/vec3.hpp"

#include <cmath>
#include <cstddef>

namespace fastphysics {

void compute_accelerations(
    std::vector<Particle>& particles,
    double gravitational_constant,
    double softening
)
{
    // Accelerations are recomputed from scratch for the current system state.
    for (Particle& particle : particles) {
        particle.acceleration = {};
    }

    // Squaring once avoids repeating the operation inside the O(N^2) loop.
    const double softening_squared = softening * softening;

    // Direct N-body evaluation checks every particle pair, resulting in
    // approximately N * (N - 1) interactions and O(N^2) time complexity.
    for (std::size_t i = 0; i < particles.size(); ++i) {

        for (std::size_t j = 0; j < particles.size(); ++j) {

            // A particle does not gravitationally interact with itself.
            if (i == j) {
                continue;
            }

            const Vec3 displacement =
                particles[j].position - particles[i].position;

            // Softening prevents the gravitational interaction from becoming
            // singular when two particles are extremely close together.
            const double distance_squared =
                norm_squared(displacement) + softening_squared;

            const double inverse_distance =
                1.0 / std::sqrt(distance_squared);

            // The vector formulation requires r / |r|^3 so that the
            // resulting acceleration magnitude follows the inverse-square law.
            const double inverse_distance_cubed =
                inverse_distance
                * inverse_distance
                * inverse_distance;

            const double acceleration_scale =
                gravitational_constant
                * particles[j].mass
                * inverse_distance_cubed;

            particles[i].acceleration +=
                displacement * acceleration_scale;
        }
    }
}

}