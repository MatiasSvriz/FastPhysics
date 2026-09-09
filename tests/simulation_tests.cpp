#include "fastphysics/simulation.hpp"

#include <cassert>
#include <cmath>
#include <utility>
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
    // Euler simulation step
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

        // A single particle experiences no gravitational force,
        // so it should move with constant velocity.
        assert(almost_equal(
            particle.position.x,
            1.0
        ));

        assert(almost_equal(
            particle.velocity.x,
            2.0
        ));

        assert(almost_equal(
            particle.acceleration.x,
            0.0
        ));

        assert(almost_equal(
            simulation.time(),
            0.5
        ));
    }

    // -----------------------------------------------------------------------
    // Velocity Verlet simulation step
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

        // With zero acceleration, Velocity Verlet must also reproduce
        // uniform linear motion exactly.
        assert(almost_equal(
            particle.position.x,
            1.0
        ));

        assert(almost_equal(
            particle.velocity.x,
            2.0
        ));

        assert(almost_equal(
            particle.acceleration.x,
            0.0
        ));

        assert(almost_equal(
            simulation.time(),
            0.5
        ));
    }

    // -----------------------------------------------------------------------
    // Multiple simulation steps
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

        simulation.run(10);

        const auto& particle =
            simulation.particles()[0];

        assert(almost_equal(
            simulation.time(),
            1.0
        ));

        assert(almost_equal(
            particle.position.x,
            1.0
        ));

        assert(almost_equal(
            particle.velocity.x,
            1.0
        ));
    }
}