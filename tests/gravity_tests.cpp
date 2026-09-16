#include "fastphysics/gravity.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>

namespace {

bool almost_equal(
    double a,
    double b,
    double epsilon = 1e-12
)
{
    return std::abs(a - b) < epsilon;
}

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

    assert(almost_equal(
        particles[0].acceleration.x,
        0.75
    ));

    assert(almost_equal(
        particles[1].acceleration.x,
        -0.5
    ));

    assert(almost_equal(
        particles[0].acceleration.y,
        0.0
    ));

    assert(almost_equal(
        particles[0].acceleration.z,
        0.0
    ));
}

void test_pairwise_matches_reference_solver()
{
    // Three particles exercise multiple distinct pairs:
    //
    //     (0, 1)
    //     (0, 2)
    //     (1, 2)
    //
    // This is more useful than a two-particle system for checking that the
    // pairwise solver correctly accumulates several interactions.
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

    // Both solvers must start from exactly the same physical system.
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

    // The algorithms evaluate interactions in a different order, so small
    // floating-point differences are possible. We therefore compare using
    // a numerical tolerance instead of exact equality.
    for (std::size_t i = 0;
         i < reference_particles.size();
         ++i) {

        assert(almost_equal(
            reference_particles[i].acceleration.x,
            pairwise_particles[i].acceleration.x
        ));

        assert(almost_equal(
            reference_particles[i].acceleration.y,
            pairwise_particles[i].acceleration.y
        ));

        assert(almost_equal(
            reference_particles[i].acceleration.z,
            pairwise_particles[i].acceleration.z
        ));
    }
}

}

int main()
{
    test_two_body_reference_solver();
    test_pairwise_matches_reference_solver();

    return 0;
}