#include "fastphysics/gravity.hpp"

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

    assert(almost_equal(
        particles[0].acceleration.x,
        0.75
    ));

    assert(almost_equal(
        particles[1].acceleration.x,
        -0.5
    ));

    assert(almost_equal(
        particles[0].acceleration.y,
        0.0
    ));

    assert(almost_equal(
        particles[0].acceleration.z,
        0.0
    ));
}