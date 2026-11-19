
#include "fastphysics/gravity.hpp"
#include "test_helpers.hpp"

#include <cstddef>
#include <vector>

namespace {

using fp_test::almost_equal;

// --------------------------------------------------
// Test 1: Two-body reference solver
// --------------------------------------------------

void test_two_body_reference_solver()
{
    std::vector<fastphysics::Particle> particles{
        {
            {0.0, 0.0, 0.0},
            {},
            {},
            2.0
        },
        {
            {2.0, 0.0, 0.0},
            {},
            {},
            3.0
        }
    };

    fastphysics::compute_accelerations(
        particles,
        1.0,
        0.0
    );

    FP_CHECK(almost_equal(
        particles[0].acceleration.x,
        0.75
    ));

    FP_CHECK(almost_equal(
        particles[1].acceleration.x,
        -0.5
    ));

    FP_CHECK(almost_equal(
        particles[0].acceleration.y,
        0.0
    ));

    FP_CHECK(almost_equal(
        particles[0].acceleration.z,
        0.0
    ));
}

// --------------------------------------------------
// Test 2: Pairwise matches reference solver
// --------------------------------------------------

void test_pairwise_matches_reference_solver()
{
    // Three particles exercise multiple distinct pairs:
    //
    // (0, 1)
    // (0, 2)
    // (1, 2)

    std::vector<fastphysics::Particle> reference_particles{
        {
            {0.0, 0.0, 0.0},
            {},
            {},
            2.0
        },
        {
            {2.0, 1.0, 0.0},
            {},
            {},
            3.0
        },
        {
            {-1.0, 3.0, 2.0},
            {},
            {},
            4.0
        }
    };

    // Both solvers start from identical initial conditions.
    auto pairwise_particles = reference_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    fastphysics::compute_accelerations(
        reference_particles,
        gravitational_constant,
        softening
    );

    fastphysics::compute_accelerations_pairwise(
        pairwise_particles,
        gravitational_constant,
        softening
    );

    // Compare accelerations using numerical tolerances.
    for (std::size_t i = 0;
         i < reference_particles.size();
         ++i) {

        FP_CHECK(almost_equal(
            reference_particles[i].acceleration.x,
            pairwise_particles[i].acceleration.x
        ));

        FP_CHECK(almost_equal(
            reference_particles[i].acceleration.y,
            pairwise_particles[i].acceleration.y
        ));

        FP_CHECK(almost_equal(
            reference_particles[i].acceleration.z,
            pairwise_particles[i].acceleration.z
        ));
    }
}

// --------------------------------------------------
// Test 3: SoA matches reference solver
// --------------------------------------------------

void test_soa_matches_reference_solver()
{
    std::vector<fastphysics::Particle> particles{
        {
            {0.0, 0.0, 0.0},
            {},
            {},
            2.0
        },
        {
            {2.0, 1.0, 0.0},
            {},
            {},
            3.0
        },
        {
            {-1.0, 3.0, 2.0},
            {},
            {},
            4.0
        }
    };

    auto soa_particles =
        fastphysics::make_particle_system_soa(
            particles
        );

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    fastphysics::compute_accelerations(
        particles,
        gravitational_constant,
        softening
    );

    fastphysics::compute_accelerations_soa(
        soa_particles,
        gravitational_constant,
        softening
    );

    for (std::size_t i = 0; i < particles.size(); ++i) {

        FP_CHECK(almost_equal(
            particles[i].acceleration.x,
            soa_particles.acceleration_x[i]
        ));

        FP_CHECK(almost_equal(
            particles[i].acceleration.y,
            soa_particles.acceleration_y[i]
        ));

        FP_CHECK(almost_equal(
            particles[i].acceleration.z,
            soa_particles.acceleration_z[i]
        ));
    }
}

// --------------------------------------------------
// Test 4: Pairwise SoA matches reference solver
// --------------------------------------------------

void test_pairwise_soa_matches_reference_solver()
{
    std::vector<fastphysics::Particle> particles{
        {
            {0.0, 0.0, 0.0},
            {},
            {},
            2.0
        },
        {
            {2.0, 1.0, 0.0},
            {},
            {},
            3.0
        },
        {
            {-1.0, 3.0, 2.0},
            {},
            {},
            4.0
        }
    };

    auto pairwise_soa_particles =
        fastphysics::make_particle_system_soa(
            particles
        );

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    fastphysics::compute_accelerations(
        particles,
        gravitational_constant,
        softening
    );

    fastphysics::compute_accelerations_pairwise_soa(
        pairwise_soa_particles,
        gravitational_constant,
        softening
    );

    for (std::size_t i = 0; i < particles.size(); ++i) {

        FP_CHECK(almost_equal(
            particles[i].acceleration.x,
            pairwise_soa_particles.acceleration_x[i]
        ));

        FP_CHECK(almost_equal(
            particles[i].acceleration.y,
            pairwise_soa_particles.acceleration_y[i]
        ));

        FP_CHECK(almost_equal(
            particles[i].acceleration.z,
            pairwise_soa_particles.acceleration_z[i]
        ));
    }
}

// --------------------------------------------------
// Test 5: Newton's third law
// --------------------------------------------------

void test_total_internal_force_is_zero()
{
    // Three particles with different positions and masses.
    std::vector<fastphysics::Particle> particles{
        {
            {0.0, 0.0, 0.0},
            {},
            {},
            2.0
        },
        {
            {2.0, 1.0, 0.0},
            {},
            {},
            3.0
        },
        {
            {-1.0, 3.0, 2.0},
            {},
            {},
            4.0
        }
    };

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Calculate gravitational accelerations.
    fastphysics::compute_accelerations(
        particles,
        gravitational_constant,
        softening
    );

    // Calculate the total internal force:
    //
    // F_total = sum(m_i * a_i)

    fastphysics::Vec3 total_force{};

    double force_scale = 0.0;

    for (const auto& particle : particles) {

        const fastphysics::Vec3 force{
            particle.mass * particle.acceleration.x,
            particle.mass * particle.acceleration.y,
            particle.mass * particle.acceleration.z
        };

        total_force += force;

        // Accumulate force magnitudes for a relative tolerance.
        force_scale += fastphysics::norm(force);
    }

    // Newton's third law:
    //
    // F_ij = -F_ji
    //
    // Therefore:
    //
    // sum(m_i * a_i) = 0
    //
    // Allow a small floating-point rounding error.

    constexpr double relative_tolerance = 1e-12;
    constexpr double absolute_tolerance = 1e-12;

    const double tolerance =
        absolute_tolerance +
        relative_tolerance * force_scale;

    FP_CHECK(almost_equal(
        total_force.x, 0.0, tolerance, 0.0
    ));

    FP_CHECK(almost_equal(
        total_force.y, 0.0, tolerance, 0.0
    ));

    FP_CHECK(almost_equal(
        total_force.z, 0.0, tolerance, 0.0
    ));
}

} // namespace

int main()
{
    test_two_body_reference_solver();

    test_pairwise_matches_reference_solver();

    test_soa_matches_reference_solver();

    test_pairwise_soa_matches_reference_solver();

    test_total_internal_force_is_zero();

    return 0;
}
