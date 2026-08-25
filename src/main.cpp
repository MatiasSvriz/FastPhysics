#include "fastphysics/particle.hpp"

#include <iostream>
#include <vector>

int main()
{
    std::vector<fastphysics::Particle> particles;

    particles.reserve(3);

    particles.push_back({
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {},
        10.0
    });

    particles.push_back({
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {},
        1.0
    });

    particles.push_back({
        {-1.0, 0.0, 0.0},
        {0.0, -1.0, 0.0},
        {},
        1.0
    });

    std::cout
        << "FastPhysics\n"
        << "Particles: "
        << particles.size()
        << '\n';
}