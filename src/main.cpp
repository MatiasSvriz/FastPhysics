#include "fastphysics/particle.hpp"
#include "fastphysics/simulation.hpp"

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

int main()
{
    std::vector<fastphysics::Particle> particles{
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

    const fastphysics::SimulationConfig config{
        .gravitational_constant = 1.0,
        .softening = 0.01,
        .dt = 0.001
    };

    fastphysics::Simulation simulation{
        std::move(particles),
        config
    };

    constexpr std::size_t number_of_steps = 1000;

    simulation.run(number_of_steps);

    std::cout
        << "FastPhysics - N-body CPU reference\n"
        << "Simulation time: "
        << simulation.time()
        << "\n\n";

    const auto& final_particles = simulation.particles();

    for (std::size_t i = 0; i < final_particles.size(); ++i) {
        const auto& position = final_particles[i].position;
        const auto& velocity = final_particles[i].velocity;

        std::cout
            << "Particle " << i << '\n'
            << "  position: ("
            << position.x << ", "
            << position.y << ", "
            << position.z << ")\n"
            << "  velocity: ("
            << velocity.x << ", "
            << velocity.y << ", "
            << velocity.z << ")\n";
    }
}