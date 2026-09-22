#include "fastphysics/gravity.hpp"

#include "fastphysics/vec3.hpp"

#include <cmath>
#include <cstddef>

namespace fastphysics {

namespace {

// Adds the four double values stored inside a 256-bit AVX register.
// [a, b, c, d] -> a + b + c + d
double horizontal_sum(
    __m256d values
)
{
    // Split the 256-bit register into two 128-bit halves.
    const __m128d lower =
        _mm256_castpd256_pd128(values);

    const __m128d upper =
        _mm256_extractf128_pd(
            values,
            1
        );

    // [a, b] + [c, d] = [a + c, b + d]
    const __m128d pair_sum =
        _mm_add_pd(
            lower,
            upper
        );

    // Move the second value into the lower position.
    const __m128d high_value =
        _mm_unpackhi_pd(
            pair_sum,
            pair_sum
        );

    // (a + c) + (b + d)
    const __m128d total =
        _mm_add_sd(
            pair_sum,
            high_value
        );

    return _mm_cvtsd_f64(total);
}

}

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

void compute_accelerations_soa_restructured(
    ParticleSystemSoA& particles,
    double gravitational_constant,
    double softening
)
{
    const std::size_t particle_count = particles.size();

    const double softening_squared =
        softening * softening;

    // This version keeps the same N * (N - 1) interaction count as the
    // reference SoA solver, but restructures the inner loop to make it
    // friendlier to compiler auto-vectorization.
    for (std::size_t i = 0; i < particle_count; ++i) {

        // Particle i remains constant during the whole inner loop.
        // Load its position once instead of reading it repeatedly.
        const double position_i_x = particles.position_x[i];
        const double position_i_y = particles.position_y[i];
        const double position_i_z = particles.position_z[i];

        // Accumulate the acceleration locally.
        // This avoids repeatedly writing to acceleration_x/y/z[i]
        // inside the inner loop.
        double acceleration_x = 0.0;
        double acceleration_y = 0.0;
        double acceleration_z = 0.0;

        // Interactions with particles before i.
        for (std::size_t j = 0; j < i; ++j) {

            const double displacement_x =
                particles.position_x[j] - position_i_x;

            const double displacement_y =
                particles.position_y[j] - position_i_y;

            const double displacement_z =
                particles.position_z[j] - position_i_z;

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

            acceleration_x +=
                displacement_x * acceleration_scale;

            acceleration_y +=
                displacement_y * acceleration_scale;

            acceleration_z +=
                displacement_z * acceleration_scale;
        }

        // Skip j == i completely and continue with particles after i.
        // Splitting the range removes the branch if (i == j) from the hot loop.
        for (std::size_t j = i + 1; j < particle_count; ++j) {

            const double displacement_x =
                particles.position_x[j] - position_i_x;

            const double displacement_y =
                particles.position_y[j] - position_i_y;

            const double displacement_z =
                particles.position_z[j] - position_i_z;

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

            acceleration_x +=
                displacement_x * acceleration_scale;

            acceleration_y +=
                displacement_y * acceleration_scale;

            acceleration_z +=
                displacement_z * acceleration_scale;
        }

        // Only one write per acceleration component and particle.
        particles.acceleration_x[i] = acceleration_x;
        particles.acceleration_y[i] = acceleration_y;
        particles.acceleration_z[i] = acceleration_z;
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
    // Starting j at i + 1 avoids self-interactions and prevents us from calculating both (i, j) and (j, i).
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

void compute_accelerations_soa_avx(
    ParticleSystemSoA& particles,
    double gravitational_constant,
    double softening
)
{
    const std::size_t particle_count =
        particles.size();

    const double softening_squared =
        softening * softening;

    // AVX uses 256-bit registers.
    // Since one double uses 64 bits, a __m256d can hold four doubles [value0, value1, value2, value3]
    // This allows us to calculate four particle interactions at the same time.
    // _mm256_set1_pd(value) copies the same scalar value into all four lanes.
    const __m256d one_vector =
        _mm256_set1_pd(1.0);

    const __m256d gravitational_constant_vector =
        _mm256_set1_pd(
            gravitational_constant
        );

    const __m256d softening_squared_vector =
        _mm256_set1_pd(
            softening_squared
        );

    // We still calculate the acceleration of one particle i at a time.
    // AVX is used to process four different particles j simultaneously.
    for (std::size_t i = 0; i < particle_count; ++i) {

        // Particle i stays fixed while all the other particles interact with it.
        // Keep its position in local variables so we only read it once.
        const double position_i_x =
            particles.position_x[i];

        const double position_i_y =
            particles.position_y[i];

        const double position_i_z =
            particles.position_z[i];

        // Copy each coordinate of particle i into all four SIMD lanes.
        // For example, position_i_x = 5 becomes [5, 5, 5, 5]
        // We can then subtract this from four different x_j values at once.
        const __m256d position_i_x_vector =
            _mm256_set1_pd(
                position_i_x
            );

        const __m256d position_i_y_vector =
            _mm256_set1_pd(
                position_i_y
            );

        const __m256d position_i_z_vector =
            _mm256_set1_pd(
                position_i_z
            );

        // Each SIMD accumulator stores four partial sums.
        __m256d acceleration_x_vector = _mm256_setzero_pd();

        __m256d acceleration_y_vector = _mm256_setzero_pd();

        __m256d acceleration_z_vector = _mm256_setzero_pd();

        // If the number of remaining particles is not a multiple of four,
        // the leftover interactions are calculated normally with doubles.
        double acceleration_x = 0.0;
        double acceleration_y = 0.0;
        double acceleration_z = 0.0;

        // Particles before i

        // We split the interactions into [0, i) and (i, N).
        // This avoids putting an if (i == j) branch inside the SIMD loop.

        std::size_t j = 0;

        // Process four particles at a time.
        for (; j + 3 < i; j += 4) {
            // Load four consecutive coordinates from memory.
            // For example, [x_j, x_j+1, x_j+2, x_j+3]
            // SoA makes this convenient because all x coordinates are
            // stored next to each other in memory.
            const __m256d position_j_x =
                _mm256_loadu_pd(
                    &particles.position_x[j]
                );

            const __m256d position_j_y =
                _mm256_loadu_pd(
                    &particles.position_y[j]
                );

            const __m256d position_j_z =
                _mm256_loadu_pd(
                    &particles.position_z[j]
                );

             // AVX arithmetic works lane by lane.
            // For example, [x0, x1, x2, x3] - [xi, xi, xi, xi] gives [dx0, dx1, dx2, dx3]
            // We therefore calculate four displacement vectors at once.
            const __m256d displacement_x =
                _mm256_sub_pd(
                    position_j_x,
                    position_i_x_vector
                );

            const __m256d displacement_y =
                _mm256_sub_pd(
                    position_j_y,
                    position_i_y_vector
                );

            const __m256d displacement_z =
                _mm256_sub_pd(
                    position_j_z,
                    position_i_z_vector
                );

            // distance_squared = dx^2 + dy^2 + dz^2 + softening^2
            // _mm256_mul_pd multiplies the four lanes independently.
            __m256d distance_squared =
                _mm256_mul_pd(
                    displacement_x,
                    displacement_x
                );
            
            // FMA means a * b + c in one operation per lane.
            distance_squared =
                _mm256_fmadd_pd(
                    displacement_y,
                    displacement_y,
                    distance_squared
                );

            distance_squared =
                _mm256_fmadd_pd(
                    displacement_z,
                    displacement_z,
                    distance_squared
                );

            distance_squared =
                _mm256_add_pd(
                    distance_squared,
                    softening_squared_vector
                );

            // Calculate four square roots at once [sqrt(r0²), sqrt(r1²), sqrt(r2²), sqrt(r3²)]
            const __m256d distance =
                _mm256_sqrt_pd(
                    distance_squared
                );

            // Divide lane by lane to obtain [1/r0, 1/r1, 1/r2, 1/r3]
            const __m256d inverse_distance =
                _mm256_div_pd(
                    one_vector,
                    distance
                );

            // Build 1/r³ for all four interactions.
            const __m256d inverse_distance_squared =
                _mm256_mul_pd(
                    inverse_distance,
                    inverse_distance
                );

            const __m256d inverse_distance_cubed =
                _mm256_mul_pd(
                    inverse_distance_squared,
                    inverse_distance
                );

            // Load four masses [m_j, m_j+1, m_j+2, m_j+3]
            const __m256d mass =
                _mm256_loadu_pd(
                    &particles.mass[j]
                );
            
            // Calculate G * m_j for all four particles at once.
            const __m256d gravitational_mass =
                _mm256_mul_pd(
                    gravitational_constant_vector,
                    mass
                );

            // Complete the scalar part of the gravity equation scale = G * m_j / r³ producing four scale values.
            const __m256d acceleration_scale =
                _mm256_mul_pd(
                    gravitational_mass,
                    inverse_distance_cubed
                );

            // Add the four new acceleration contributions to the four partial sums already stored in the SIMD registers.
            // FMA performs: displacement * scale + previous_sum independently in every lane.
            acceleration_x_vector =
                _mm256_fmadd_pd(
                    displacement_x,
                    acceleration_scale,
                    acceleration_x_vector
                );

            acceleration_y_vector =
                _mm256_fmadd_pd(
                    displacement_y,
                    acceleration_scale,
                    acceleration_y_vector
                );

            acceleration_z_vector =
                _mm256_fmadd_pd(
                    displacement_z,
                    acceleration_scale,
                    acceleration_z_vector
                );
        }

        // The SIMD loop only processes complete groups of four.
        // If one, two, or three particles remain before i, calculate those
        // interactions normally. This leftover section is the scalar tail.
        for (; j < i; ++j) {

            const double displacement_x =
                particles.position_x[j]
                - position_i_x;

            const double displacement_y =
                particles.position_y[j]
                - position_i_y;

            const double displacement_z =
                particles.position_z[j]
                - position_i_z;

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

            acceleration_x +=
                displacement_x
                * acceleration_scale;

            acceleration_y +=
                displacement_y
                * acceleration_scale;

            acceleration_z +=
                displacement_z
                * acceleration_scale;
        }

        // Particles after i

        // Skip particle i itself and continue immediately after it.
        j = i + 1;

        // The same SIMD process is repeated for particles after i:
        //     load 4 particles
        //     calculate 4 displacements
        //     calculate 4 distances
        //     calculate 4 acceleration contributions
        //     accumulate them in the four SIMD lanes
        for (; j + 3 < particle_count; j += 4) {

            const __m256d position_j_x =
                _mm256_loadu_pd(
                    &particles.position_x[j]
                );

            const __m256d position_j_y =
                _mm256_loadu_pd(
                    &particles.position_y[j]
                );

            const __m256d position_j_z =
                _mm256_loadu_pd(
                    &particles.position_z[j]
                );

            const __m256d displacement_x =
                _mm256_sub_pd(
                    position_j_x,
                    position_i_x_vector
                );

            const __m256d displacement_y =
                _mm256_sub_pd(
                    position_j_y,
                    position_i_y_vector
                );

            const __m256d displacement_z =
                _mm256_sub_pd(
                    position_j_z,
                    position_i_z_vector
                );

            __m256d distance_squared =
                _mm256_mul_pd(
                    displacement_x,
                    displacement_x
                );

            distance_squared =
                _mm256_fmadd_pd(
                    displacement_y,
                    displacement_y,
                    distance_squared
                );

            distance_squared =
                _mm256_fmadd_pd(
                    displacement_z,
                    displacement_z,
                    distance_squared
                );

            distance_squared =
                _mm256_add_pd(
                    distance_squared,
                    softening_squared_vector
                );

            const __m256d distance =
                _mm256_sqrt_pd(
                    distance_squared
                );

            const __m256d inverse_distance =
                _mm256_div_pd(
                    one_vector,
                    distance
                );

            const __m256d inverse_distance_squared =
                _mm256_mul_pd(
                    inverse_distance,
                    inverse_distance
                );

            const __m256d inverse_distance_cubed =
                _mm256_mul_pd(
                    inverse_distance_squared,
                    inverse_distance
                );

            const __m256d mass =
                _mm256_loadu_pd(
                    &particles.mass[j]
                );

            const __m256d gravitational_mass =
                _mm256_mul_pd(
                    gravitational_constant_vector,
                    mass
                );

            const __m256d acceleration_scale =
                _mm256_mul_pd(
                    gravitational_mass,
                    inverse_distance_cubed
                );

            acceleration_x_vector =
                _mm256_fmadd_pd(
                    displacement_x,
                    acceleration_scale,
                    acceleration_x_vector
                );

            acceleration_y_vector =
                _mm256_fmadd_pd(
                    displacement_y,
                    acceleration_scale,
                    acceleration_y_vector
                );

            acceleration_z_vector =
                _mm256_fmadd_pd(
                    displacement_z,
                    acceleration_scale,
                    acceleration_z_vector
                );
        }

        // Process the final one, two, or three particles that do not fit inside a complete four-value AVX register.
        for (; j < particle_count; ++j) {

            const double displacement_x =
                particles.position_x[j]
                - position_i_x;

            const double displacement_y =
                particles.position_y[j]
                - position_i_y;

            const double displacement_z =
                particles.position_z[j]
                - position_i_z;

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

            acceleration_x +=
                displacement_x
                * acceleration_scale;

            acceleration_y +=
                displacement_y
                * acceleration_scale;

            acceleration_z +=
                displacement_z
                * acceleration_scale;
        }

        // The SIMD registers now contain four partial sums [sum0, sum1, sum2, sum3]
        // horizontal_sum() reduces those four lanes into one value, sum0 + sum1 + sum2 + sum3
        // We then add any contributions calculated by the scalar tails.
        acceleration_x +=
            horizontal_sum(
                acceleration_x_vector
            );

        acceleration_y +=
            horizontal_sum(
                acceleration_y_vector
            );

        acceleration_z +=
            horizontal_sum(
                acceleration_z_vector
            );

        // Store the final acceleration of particle i.
        particles.acceleration_x[i] =
            acceleration_x;

        particles.acceleration_y[i] =
            acceleration_y;

        particles.acceleration_z[i] =
            acceleration_z;
    }
}

}