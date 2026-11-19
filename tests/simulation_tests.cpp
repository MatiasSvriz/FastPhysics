
#include "fastphysics/simulation.hpp"
#include "test_helpers.hpp"

#include <utility>
#include <vector>

int main()
{
    using fp_test::almost_equal;

    // -----------------------------------------------------------------------
    // Test 1: Euler simulation step
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::Particle> particles{
            {
                {0.0, 0.0, 0.0},
                {2.0, 0.0, 0.0},
                {},
                1.0
            }
        };

        const fastphysics::SimulationConfig config{
            .gravitational_constant = 1.0,
            .softening = 0.01,
            .dt = 0.5,
            .integration_method =
                fastphysics::IntegrationMethod::Euler
        };

        fastphysics::Simulation simulation{
            std::move(particles),
            config
        };

        simulation.step();

        const auto& particle =
            simulation.particles()[0];

        // A single particle experiences no gravitational force.
        // Therefore, its velocity must remain constant.

        FP_CHECK(almost_equal(
            particle.position.x,
            1.0
        ));

        FP_CHECK(almost_equal(
            particle.velocity.x,
            2.0
        ));

        FP_CHECK(almost_equal(
            particle.acceleration.x,
            0.0
        ));

        FP_CHECK(almost_equal(
            simulation.time(),
            0.5
        ));
    }

    // -----------------------------------------------------------------------
    // Test 2: Velocity Verlet simulation step
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::Particle> particles{
            {
                {0.0, 0.0, 0.0},
                {2.0, 0.0, 0.0},
                {},
                1.0
            }
        };

        const fastphysics::SimulationConfig config{
            .gravitational_constant = 1.0,
            .softening = 0.01,
            .dt = 0.5,
            .integration_method =
                fastphysics::IntegrationMethod::VelocityVerlet
        };

        fastphysics::Simulation simulation{
            std::move(particles),
            config
        };

        simulation.step();

        const auto& particle =
            simulation.particles()[0];

        // With zero acceleration, Velocity Verlet must
        // reproduce uniform linear motion.

        FP_CHECK(almost_equal(
            particle.position.x,
            1.0
        ));

        FP_CHECK(almost_equal(
            particle.velocity.x,
            2.0
        ));

        FP_CHECK(almost_equal(
            particle.acceleration.x,
            0.0
        ));

        FP_CHECK(almost_equal(
            simulation.time(),
            0.5
        ));
    }

    // -----------------------------------------------------------------------
    // Test 3: Multiple simulation steps
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::Particle> particles{
            {
                {},
                {1.0, 0.0, 0.0},
                {},
                1.0
            }
        };

        const fastphysics::SimulationConfig config{
            .gravitational_constant = 1.0,
            .softening = 0.01,
            .dt = 0.1,
            .integration_method =
                fastphysics::IntegrationMethod::VelocityVerlet
        };

        fastphysics::Simulation simulation{
            std::move(particles),
            config
        };

        // Execute 10 steps of dt = 0.1.
        // Expected total time: t = 1.0.

        simulation.run(10);

        const auto& particle =
            simulation.particles()[0];

        FP_CHECK(almost_equal(
            simulation.time(),
            1.0
        ));

        // With constant velocity v = 1.0:
        // x = x0 + v * t = 0 + 1 * 1 = 1.0

        FP_CHECK(almost_equal(
            particle.position.x,
            1.0
        ));

        FP_CHECK(almost_equal(
            particle.velocity.x,
            1.0
        ));
    }

    return 0;
}
