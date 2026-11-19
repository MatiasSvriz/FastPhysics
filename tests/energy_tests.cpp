
#include "fastphysics/energy.hpp"
#include "test_helpers.hpp"

#include <vector>

int main()
{
    using fp_test::almost_equal;

    // -----------------------------------------------------------------------
    // Test 1: Kinetic energy
    // -----------------------------------------------------------------------

    {
        const std::vector<fastphysics::Particle> particles{
            {
                {},
                {3.0, 4.0, 0.0},
                {},
                2.0
            }
        };

        // K = 1/2 * m * v^2
        // v^2 = 3^2 + 4^2 = 25
        // K = 1/2 * 2 * 25 = 25

        FP_CHECK(almost_equal(
            fastphysics::kinetic_energy(particles),
            25.0
        ));
    }

    // -----------------------------------------------------------------------
    // Test 2: Potential energy
    // -----------------------------------------------------------------------

    {
        const std::vector<fastphysics::Particle> particles{
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

        // U = -G * m1 * m2 / r
        // U = -1 * 2 * 3 / 2 = -3

        FP_CHECK(almost_equal(
            fastphysics::potential_energy(
                particles,
                1.0,
                0.0
            ),
            -3.0
        ));
    }

    // -----------------------------------------------------------------------
    // Test 3: Total energy of the reference two-body system
    // -----------------------------------------------------------------------

    {
        const std::vector<fastphysics::Particle> particles{
            {
                {-1.0, 0.0, 0.0},
                {0.0, -0.5, 0.0},
                {},
                1.0
            },
            {
                {1.0, 0.0, 0.0},
                {0.0, 0.5, 0.0},
                {},
                1.0
            }
        };

        // Total energy:
        //
        // E = K + U
        //
        // K = 2 * (1/2 * 1 * 0.5^2) = 0.25
        // U = -1 * 1 * 1 / 2 = -0.5
        //
        // E = 0.25 - 0.5 = -0.25

        FP_CHECK(almost_equal(
            fastphysics::total_energy(
                particles,
                1.0,
                0.0
            ),
            -0.25
        ));
    }

    return 0;
}
