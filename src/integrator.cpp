#include "fastphysics/integrator.hpp"

namespace fastphysics {

void euler_step(
    std::vector<Particle>& particles,
    double dt
)
{
    // Each particle is updated exactly once, so one integration step runs in O(N) time complexity.
    for (Particle& particle : particles) {
        particle.position += particle.velocity * dt;
        particle.velocity += particle.acceleration * dt;
    }
}

}