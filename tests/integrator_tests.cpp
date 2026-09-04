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