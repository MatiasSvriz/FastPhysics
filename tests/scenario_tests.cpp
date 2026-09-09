#include "fastphysics/scenarios.hpp"

#include <cassert>

int main()
{
    const auto particles = fastphysics::make_two_body_system();

    assert(particles.size() == 2);

    assert(particles[0].position.x == -1.0);
    assert(particles[1].position.x == 1.0);

    assert(particles[0].mass == 1.0);
    assert(particles[1].mass == 1.0);

    assert(
        particles[0].velocity.y
        == -particles[1].velocity.y
    );
}