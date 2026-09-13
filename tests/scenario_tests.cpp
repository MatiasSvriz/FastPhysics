#include "fastphysics/scenarios.hpp"

#include <cassert>

int main()
{
    // -----------------------------------------------------------------------
    // Two-body validation scenario
    // -----------------------------------------------------------------------

    {
        const auto particles =
            fastphysics::make_two_body_system();

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

    // -----------------------------------------------------------------------
    // Benchmark scenario
    // -----------------------------------------------------------------------

    {
        const auto particles =
            fastphysics::make_benchmark_system(4);

        assert(particles.size() == 4);

        assert(particles[0].position.x == 0.0);
        assert(particles[1].position.x == 1.0);
        assert(particles[2].position.x == 2.0);
        assert(particles[3].position.x == 3.0);

        assert(particles[0].mass == 1.0);
        assert(particles[3].mass == 1.0);

        assert(particles[0].velocity.x == 0.0);
        assert(particles[3].acceleration.x == 0.0);
    }
}