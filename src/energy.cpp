#include "fastphysics/energy.hpp"

#include "fastphysics/vec3.hpp"

#include <cmath>
#include <cstddef>

namespace fastphysics {

double kinetic_energy(
    const std::vector<Particle>& particles
)
{
    double energy = 0.0;

    // Each particle contributes once, giving O(N) time complexity.
    for (const Particle& particle : particles) {
        energy +=
            0.5
            * particle.mass
            * norm_squared(particle.velocity);
    }

    return energy;
}

double potential_energy(
    const std::vector<Particle>& particles,
    double gravitational_constant,
    double softening
)
{
    double energy = 0.0;

    const double softening_squared =
        softening * softening;

    // Each unique particle pair is evaluated once, giving O(N^2) time.
    for (std::size_t i = 0; i < particles.size(); ++i) {

        for (std::size_t j = i + 1;
             j < particles.size();
             ++j) {

            const Vec3 displacement =
                particles[j].position
                - particles[i].position;

            const double distance =
                std::sqrt(
                    norm_squared(displacement)
                    + softening_squared
                );

            energy -=
                gravitational_constant
                * particles[i].mass
                * particles[j].mass
                / distance;
        }
    }

    return energy;
}

double total_energy(
    const std::vector<Particle>& particles,
    double gravitational_constant,
    double softening
)
{
    return kinetic_energy(particles)
         + potential_energy(
               particles,
               gravitational_constant,
               softening
           );
}

}