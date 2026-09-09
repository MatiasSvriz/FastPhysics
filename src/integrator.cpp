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

void velocity_verlet_begin_step(
    std::vector<Particle>& particles,
    double dt
)
{
    const double half_dt = 0.5 * dt;

    // Each particle is updated once, O(N) time complexity and O(1) extra memory.
    for (Particle& particle : particles) {
        particle.velocity +=
            particle.acceleration * half_dt;

        particle.position +=
            particle.velocity * dt;
    }
}

void velocity_verlet_end_step(
    std::vector<Particle>& particles,
    double dt
)
{
    const double half_dt = 0.5 * dt;

    // Complete the velocity update using the acceleration at t + dt.
    for (Particle& particle : particles) {
        particle.velocity +=
            particle.acceleration * half_dt;
    }
}

}

