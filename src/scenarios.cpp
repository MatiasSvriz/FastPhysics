#include "fastphysics/scenarios.hpp"

#include <cstddef>
#include <vector>

namespace fastphysics {

std::vector<Particle> make_two_body_system()
{
    return {
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
}

std::vector<Particle> make_benchmark_system(
    std::size_t particle_count
)
{
    std::vector<Particle> particles;

    particles.reserve(
        particle_count
    );

    for (std::size_t i = 0; i < particle_count; ++i) {

        particles.push_back({
            {
                static_cast<double>(i),
                0.0,
                0.0
            },
            {},
            {},
            1.0
        });
    }

    return particles;
}

}