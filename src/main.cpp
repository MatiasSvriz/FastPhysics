#include "fastphysics/gravity.hpp"
#include "fastphysics/particle.hpp"

#include <iostream>
#include <vector>

int main()
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

    std::cout
        << "FastPhysics - N-body CPU reference\n\n";

    for (std::size_t i = 0; i < particles.size(); ++i) {
        const auto& acceleration = particles[i].acceleration;

        std::cout
            << "Particle " << i
            << " acceleration: ("
            << acceleration.x << ", "
            << acceleration.y << ", "
            << acceleration.z << ")\n";
    }
}