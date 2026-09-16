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
    // Accelerations are recalculated from scratch every time.
    for (Particle& particle : particles) {
        particle.acceleration = {};
    }

    // Softening avoids very large forces when particles get too close.
    // We square it once here instead of doing it inside the O(N^2) loop.
    const double softening_squared = softening * softening;

    // Reference solver. For each particle i, we check every other particle j.
    // This evaluates both (i, j) and (j, i), giving N * (N - 1)
    // interactions and O(N^2) time complexity.
    for (std::size_t i = 0; i < particles.size(); ++i) {

        for (std::size_t j = 0; j < particles.size(); ++j) {

            // A particle should not interact with itself.
            if (i == j) {
                continue;
            }

            // Vector from particle i to particle j.
            const Vec3 displacement =
                particles[j].position - particles[i].position;

            // Softened squared distance: r^2 + epsilon^2.
            const double distance_squared =
                norm_squared(displacement) + softening_squared;

            // Compute 1 / distance once and reuse it below.
            const double inverse_distance =
                1.0 / std::sqrt(distance_squared);

            // Gives 1 / (r^2 + epsilon^2)^(3/2).
            const double inverse_distance_cubed =
                inverse_distance
                * inverse_distance
                * inverse_distance;

            // Acceleration of i caused by j, 
            // a_i = G * m_j * r_ij / (r^2 + epsilon^2)^(3/2)

            // The mass of particle i does not appear because it cancels
            // when going from force to acceleration.
            const double acceleration_scale =
                gravitational_constant
                * particles[j].mass
                * inverse_distance_cubed;

            particles[i].acceleration +=
                displacement * acceleration_scale;
        }
    }
}

void compute_accelerations_pairwise(
    std::vector<Particle>& particles,
    double gravitational_constant,
    double softening
)
{
    // Same as the reference solver: start with zero acceleration.
    for (Particle& particle : particles) {
        particle.acceleration = {};
    }

    const double softening_squared =
        softening * softening;

    // Each pair is evaluated only once.
    // Starting j at i + 1 avoids both self-interactions and duplicate pairs.

    // Reference: N * (N - 1) interactions
    // Pairwise:  N * (N - 1) / 2 pairs

    // It is still O(N^2), but we avoid repeating the expensive distance
    // and square-root calculations for both (i, j) and (j, i).
    for (std::size_t i = 0; i < particles.size(); ++i) {

        for (std::size_t j = i + 1; j < particles.size(); ++j) {

            // Vector from particle i to particle j.
            const Vec3 displacement =
                particles[j].position
                - particles[i].position;

            // Both particles share the same distance, so we compute it once.
            const double distance_squared =
                norm_squared(displacement)
                + softening_squared;

            const double inverse_distance =
                1.0 / std::sqrt(distance_squared);

            const double inverse_distance_cubed =
                inverse_distance
                * inverse_distance
                * inverse_distance;

            // G and the distance factor are shared by both particles.
            const double common_scale =
                gravitational_constant
                * inverse_distance_cubed;

            // Acceleration of i caused by j.
            particles[i].acceleration +=
                displacement
                * (common_scale * particles[j].mass);

            // Acceleration of j caused by i.
            // The direction is reversed, and the acceleration depends on
            // the mass of particle i.
            particles[j].acceleration +=
                displacement
                * (-common_scale * particles[i].mass);
        }
    }
}

}