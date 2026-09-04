#include "fastphysics/gravity.hpp"
#include "fastphysics/integrator.hpp"
#include "fastphysics/particle.hpp"

#include <cstddef>
#include <iostream>
#include <vector>

int main()
{
    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;
    constexpr double dt = 0.001;
    constexpr std::size_t number_of_steps = 1000;

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

    for (std::size_t step = 0; step < number_of_steps; ++step) {
        fastphysics::compute_accelerations(
            particles,
            gravitational_constant,
            softening
        );

        fastphysics::euler_step(
            particles,
            dt
        );
    }

    std::cout << "FastPhysics - N-body CPU reference\n\n";

    for (std::size_t i = 0; i < particles.size(); ++i) {
        const auto& position = particles[i].position;
        const auto& velocity = particles[i].velocity;

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