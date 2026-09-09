#include "fastphysics/integrator.hpp"

#include <cassert>
#include <cmath>
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

}

int main()
{
    // -----------------------------------------------------------------------
    // Explicit Euler
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

        assert(almost_equal(
            particles[0].position.x,
            1.0
        ));

        assert(almost_equal(
            particles[0].velocity.x,
            2.5
        ));

        assert(almost_equal(
            particles[0].position.y,
            0.0
        ));

        assert(almost_equal(
            particles[0].velocity.y,
            0.0
        ));
    }

    // -----------------------------------------------------------------------
    // Velocity Verlet
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
        // v_half = 2 + 0.5 * 1 * 0.5
        //        = 2.25
        assert(almost_equal(
            particles[0].velocity.x,
            2.25
        ));

        // Drift:
        //
        // x_new = 0 + 2.25 * 0.5
        //       = 1.125
        assert(almost_equal(
            particles[0].position.x,
            1.125
        ));

        // Simulate the acceleration recomputation performed by Simulation
        // after particles have moved to their new positions.
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
        // v_new = 2.25 + 0.5 * 3 * 0.5
        //       = 3.0
        assert(almost_equal(
            particles[0].velocity.x,
            3.0
        ));

        assert(almost_equal(
            particles[0].velocity.y,
            0.0
        ));

        assert(almost_equal(
            particles[0].position.y,
            0.0
        ));
    }
}