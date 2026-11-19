
#include "fastphysics/scenarios.hpp"
#include "test_helpers.hpp"

int main()
{
    // -----------------------------------------------------------------------
    // Test 1: Two-body validation scenario
    // -----------------------------------------------------------------------

    {
        const auto particles =
            fastphysics::make_two_body_system();

        // Verify the number of particles.
        FP_CHECK(particles.size() == 2);

        // Verify initial positions.
        FP_CHECK(particles[0].position.x == -1.0);
        FP_CHECK(particles[1].position.x == 1.0);

        // Verify particle masses.
        FP_CHECK(particles[0].mass == 1.0);
        FP_CHECK(particles[1].mass == 1.0);

        // The two particles must have opposite
        // velocities along the y-axis.
        FP_CHECK(
            particles[0].velocity.y
            == -particles[1].velocity.y
        );
    }

    // -----------------------------------------------------------------------
    // Test 2: Benchmark scenario
    // -----------------------------------------------------------------------

    {
        const auto particles =
            fastphysics::make_benchmark_system(4);

        // Verify the number of particles.
        FP_CHECK(particles.size() == 4);

        // Verify initial positions.
        FP_CHECK(particles[0].position.x == 0.0);
        FP_CHECK(particles[1].position.x == 1.0);
        FP_CHECK(particles[2].position.x == 2.0);
        FP_CHECK(particles[3].position.x == 3.0);

        // Verify particle masses.
        FP_CHECK(particles[0].mass == 1.0);
        FP_CHECK(particles[3].mass == 1.0);

        // Verify initial velocity and acceleration.
        FP_CHECK(particles[0].velocity.x == 0.0);
        FP_CHECK(particles[3].acceleration.x == 0.0);
    }

    return 0;
}
