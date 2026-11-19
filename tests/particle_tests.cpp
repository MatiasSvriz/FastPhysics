
#include "fastphysics/particle.hpp"
#include "test_helpers.hpp"

int main()
{
    const fastphysics::Particle particle{
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0},
        10.0
    };

    // --------------------------------------------------
    // Test 1: Position
    // --------------------------------------------------

    FP_CHECK(particle.position.x == 1.0);
    FP_CHECK(particle.position.y == 2.0);
    FP_CHECK(particle.position.z == 3.0);

    // --------------------------------------------------
    // Test 2: Velocity and acceleration
    // --------------------------------------------------

    FP_CHECK(particle.velocity.x == 4.0);
    FP_CHECK(particle.acceleration.z == 9.0);

    // --------------------------------------------------
    // Test 3: Mass
    // --------------------------------------------------

    FP_CHECK(particle.mass == 10.0);

    return 0;
}
