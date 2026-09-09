#include "fastphysics/simulation.hpp"

#include "fastphysics/gravity.hpp"
#include "fastphysics/integrator.hpp"

#include <utility>

namespace fastphysics {

Simulation::Simulation(
    std::vector<Particle> particles,
    SimulationConfig config
)
    : particles_(std::move(particles)),
      config_(config)
{
    // Ensure accelerations correspond to the initial particle positions.
    update_accelerations();
}

void Simulation::step()
{
    switch (config_.integration_method) {

        case IntegrationMethod::Euler:
            euler_step(
                particles_,
                config_.dt
            );

            // Keep accelerations synchronized with the new positions.
            update_accelerations();

            break;

        case IntegrationMethod::VelocityVerlet:
            // Half kick -> drift -> recompute gravity -> half kick.
            velocity_verlet_begin_step(
                particles_,
                config_.dt
            );

            // Velocity Verlet needs the acceleration at the new positions
            // before completing the second half of the velocity update.
            update_accelerations();

            velocity_verlet_end_step(
                particles_,
                config_.dt
            );

            break;
    }

    time_ += config_.dt;
}

void Simulation::run(std::size_t number_of_steps)
{
    // With the direct O(N^2) gravity solver, S simulation steps
    // require O(S * N^2) time.
    for (std::size_t step_index = 0;
         step_index < number_of_steps;
         ++step_index) {

        step();
    }
}

const std::vector<Particle>& Simulation::particles() const
{
    return particles_;
}

double Simulation::time() const
{
    return time_;
}

void Simulation::update_accelerations()
{
    compute_accelerations(
        particles_,
        config_.gravitational_constant,
        config_.softening
    );
}

}