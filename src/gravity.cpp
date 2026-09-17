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

void compute_accelerations_soa(
    ParticleSystemSoA& particles,
    double gravitational_constant,
    double softening
)
{
    // Accelerations are recalculated from scratch every time.
    for (std::size_t i = 0; i < particles.size(); ++i) {
        particles.acceleration_x[i] = 0.0;
        particles.acceleration_y[i] = 0.0;
        particles.acceleration_z[i] = 0.0;
    }

    // Softening is constant, so calculate its square once outside the loop.
    const double softening_squared = softening * softening;

    // This is intentionally the same O(N^2) algorithm as the reference AoS
    // solver. The only difference is how particle data is stored in memory.
    for (std::size_t i = 0; i < particles.size(); ++i) {

        for (std::size_t j = 0; j < particles.size(); ++j) {

            if (i == j) {
                continue;
            }

            const double displacement_x =
                particles.position_x[j]
                - particles.position_x[i];

            const double displacement_y =
                particles.position_y[j]
                - particles.position_y[i];

            const double displacement_z =
                particles.position_z[j]
                - particles.position_z[i];

            const double distance_squared =
                displacement_x * displacement_x
                + displacement_y * displacement_y
                + displacement_z * displacement_z
                + softening_squared;

            const double inverse_distance =
                1.0 / std::sqrt(distance_squared);

            const double inverse_distance_cubed =
                inverse_distance
                * inverse_distance
                * inverse_distance;

            const double acceleration_scale =
                gravitational_constant
                * particles.mass[j]
                * inverse_distance_cubed;

            particles.acceleration_x[i] +=
                displacement_x * acceleration_scale;

            particles.acceleration_y[i] +=
                displacement_y * acceleration_scale;

            particles.acceleration_z[i] +=
                displacement_z * acceleration_scale;
        }
    }
}

void compute_accelerations_pairwise_soa(
    ParticleSystemSoA& particles,
    double gravitational_constant,
    double softening
)
{
    // Accelerations are recalculated from scratch every time.
    for (std::size_t i = 0; i < particles.size(); ++i) {
        particles.acceleration_x[i] = 0.0;
        particles.acceleration_y[i] = 0.0;
        particles.acceleration_z[i] = 0.0;
    }

    const double softening_squared =
        softening * softening;

    // Each unique pair is evaluated once.
    // Starting j at i + 1 avoids self-interactions and prevents us from
    // calculating both (i, j) and (j, i).
    for (std::size_t i = 0; i < particles.size(); ++i) {

        for (std::size_t j = i + 1; j < particles.size(); ++j) {

            const double displacement_x =
                particles.position_x[j]
                - particles.position_x[i];

            const double displacement_y =
                particles.position_y[j]
                - particles.position_y[i];

            const double displacement_z =
                particles.position_z[j]
                - particles.position_z[i];

            const double distance_squared =
                displacement_x * displacement_x
                + displacement_y * displacement_y
                + displacement_z * displacement_z
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
            const double scale_i =
                common_scale
                * particles.mass[j];

            particles.acceleration_x[i] +=
                displacement_x * scale_i;

            particles.acceleration_y[i] +=
                displacement_y * scale_i;

            particles.acceleration_z[i] +=
                displacement_z * scale_i;

            // Acceleration of j caused by i.
            // The displacement direction is reversed.
            const double scale_j =
                -common_scale
                * particles.mass[i];

            particles.acceleration_x[j] +=
                displacement_x * scale_j;

            particles.acceleration_y[j] +=
                displacement_y * scale_j;

            particles.acceleration_z[j] +=
                displacement_z * scale_j;
        }
    }
}

}