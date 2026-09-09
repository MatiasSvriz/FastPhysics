#include "fastphysics/scenarios.hpp"

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

}