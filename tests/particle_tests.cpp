#include "fastphysics/particle.hpp"

#include <cassert>

int main()
{
    const fastphysics::Particle particle{
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0},
        10.0
    };

    assert(particle.position.x == 1.0);
    assert(particle.position.y == 2.0);
    assert(particle.position.z == 3.0);

    assert(particle.velocity.x == 4.0);
    assert(particle.acceleration.z == 9.0);

    assert(particle.mass == 10.0);
}