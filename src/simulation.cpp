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
}

void Simulation::step()
{
    // Gravity dominates the step with O(N^2) work-
    // Euler integration and the remaining state updates are O(N) or O(1).
    compute_accelerations(
        particles_,
        config_.gravitational_constant,
        config_.softening
    );

    euler_step(
        particles_,
        config_.dt
    );

    time_ += config_.dt;
}

void Simulation::run(std::size_t number_of_steps)
{
    // Running S steps with the direct gravity solver costs O(S * N^2).
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

}