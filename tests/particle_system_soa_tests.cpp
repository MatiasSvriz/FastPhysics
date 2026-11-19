
#include "fastphysics/particle_system_soa.hpp"
#include "test_helpers.hpp"

#include <vector>

int main()
{
    std::vector<fastphysics::Particle> particles{
        {
            {1.0, 2.0, 3.0},
            {4.0, 5.0, 6.0},
            {7.0, 8.0, 9.0},
            10.0
        },
        {
            {-1.0, -2.0, -3.0},
            {-4.0, -5.0, -6.0},
            {-7.0, -8.0, -9.0},
            20.0
        }
    };

    const fastphysics::ParticleSystemSoA system =
        fastphysics::make_particle_system_soa(
            particles
        );

    // --------------------------------------------------
    // Test 1: Particle count
    // --------------------------------------------------

    FP_CHECK(system.size() == 2);

    // --------------------------------------------------
    // Test 2: Particle 0
    // --------------------------------------------------

    // Position
    FP_CHECK(system.position_x[0] == 1.0);
    FP_CHECK(system.position_y[0] == 2.0);
    FP_CHECK(system.position_z[0] == 3.0);

    // Velocity
    FP_CHECK(system.velocity_x[0] == 4.0);
    FP_CHECK(system.velocity_y[0] == 5.0);
    FP_CHECK(system.velocity_z[0] == 6.0);

    // Acceleration
    FP_CHECK(system.acceleration_x[0] == 7.0);
    FP_CHECK(system.acceleration_y[0] == 8.0);
    FP_CHECK(system.acceleration_z[0] == 9.0);

    // Mass
    FP_CHECK(system.mass[0] == 10.0);

    // --------------------------------------------------
    // Test 3: Particle 1
    // --------------------------------------------------

    // Position
    FP_CHECK(system.position_x[1] == -1.0);
    FP_CHECK(system.position_y[1] == -2.0);
    FP_CHECK(system.position_z[1] == -3.0);

    // Velocity
    FP_CHECK(system.velocity_x[1] == -4.0);
    FP_CHECK(system.velocity_y[1] == -5.0);
    FP_CHECK(system.velocity_z[1] == -6.0);

    // Acceleration
    FP_CHECK(system.acceleration_x[1] == -7.0);
    FP_CHECK(system.acceleration_y[1] == -8.0);
    FP_CHECK(system.acceleration_z[1] == -9.0);

    // Mass
    FP_CHECK(system.mass[1] == 20.0);

    return 0;
}
