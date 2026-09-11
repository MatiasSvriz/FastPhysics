#include "fastphysics/energy.hpp"

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
    // -----------------------------------------------------------------------
    // Kinetic energy
    // -----------------------------------------------------------------------

    {
        const std::vector<fastphysics::Particle> particles{
            {
                {},
                {3.0, 4.0, 0.0},
                {},
                2.0
            }
        };

        assert(almost_equal(
            fastphysics::kinetic_energy(particles),
            25.0
        ));
    }

    // -----------------------------------------------------------------------
    // Potential energy
    // -----------------------------------------------------------------------

    {
        const std::vector<fastphysics::Particle> particles{
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

        assert(almost_equal(
            fastphysics::potential_energy(
                particles,
                1.0,
                0.0
            ),
            -3.0
        ));
    }

    // -----------------------------------------------------------------------
    // Total energy of the reference two-body system
    // -----------------------------------------------------------------------

    {
        const std::vector<fastphysics::Particle> particles{
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

        assert(almost_equal(
            fastphysics::total_energy(
                particles,
                1.0,
                0.0
            ),
            -0.25
        ));
    }
}