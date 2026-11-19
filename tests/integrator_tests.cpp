
#include "fastphysics/integrator.hpp"
#include "test_helpers.hpp"

#include <vector>

int main()
{
    using fp_test::almost_equal;

    // -----------------------------------------------------------------------
    // Test 1: Explicit Euler
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::Particle> particles{
            {
                {0.0, 0.0, 0.0},
                {2.0, 0.0, 0.0},
                {1.0, 0.0, 0.0},
                1.0
            }
        };

        fastphysics::euler_step(
            particles,
            0.5
        );

        // Euler:
        // x_new = x + v * dt = 0 + 2 * 0.5 = 1.0

        FP_CHECK(almost_equal(
            particles[0].position.x,
            1.0
        ));

        // v_new = v + a * dt = 2 + 1 * 0.5 = 2.5

        FP_CHECK(almost_equal(
            particles[0].velocity.x,
            2.5
        ));

        // No movement along the y-axis.

        FP_CHECK(almost_equal(
            particles[0].position.y,
            0.0
        ));

        FP_CHECK(almost_equal(
            particles[0].velocity.y,
            0.0
        ));
    }

    // -----------------------------------------------------------------------
    // Test 2: Velocity Verlet
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::Particle> particles{
            {
                {0.0, 0.0, 0.0},
                {2.0, 0.0, 0.0},
                {1.0, 0.0, 0.0},
                1.0
            }
        };

        fastphysics::velocity_verlet_begin_step(
            particles,
            0.5
        );

        // First half kick:
        //
        // v_half = v + (a * dt / 2)
        //        = 2 + (1 * 0.5 / 2)
        //        = 2.25

        FP_CHECK(almost_equal(
            particles[0].velocity.x,
            2.25
        ));

        // Drift:
        //
        // x_new = x + v_half * dt
        //       = 0 + 2.25 * 0.5
        //       = 1.125

        FP_CHECK(almost_equal(
            particles[0].position.x,
            1.125
        ));

        // Simulate the acceleration recomputation
        // performed by Simulation after updating positions.

        particles[0].acceleration = {
            3.0,
            0.0,
            0.0
        };

        fastphysics::velocity_verlet_end_step(
            particles,
            0.5
        );

        // Second half kick:
        //
        // v_new = v_half + (a_new * dt / 2)
        //       = 2.25 + (3 * 0.5 / 2)
        //       = 3.0

        FP_CHECK(almost_equal(
            particles[0].velocity.x,
            3.0
        ));

        // No movement along the y-axis.

        FP_CHECK(almost_equal(
            particles[0].velocity.y,
            0.0
        ));

        FP_CHECK(almost_equal(
            particles[0].position.y,
            0.0
        ));
    }

    return 0;
}
