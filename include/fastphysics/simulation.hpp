#pragma once

#include "fastphysics/particle.hpp"

#include <cstddef>
#include <vector>

namespace fastphysics {

enum class IntegrationMethod {
    Euler,
    VelocityVerlet
};

struct SimulationConfig {
    double gravitational_constant{1.0};
    double softening{0.01};
    double dt{0.001};

    IntegrationMethod integration_method{
        IntegrationMethod::Euler
    };
};

class Simulation {
public:
    Simulation(
        std::vector<Particle> particles,
        SimulationConfig config
    );

    void step();
    void run(std::size_t number_of_steps);

    const std::vector<Particle>& particles() const;
    double time() const;

private:
    void update_accelerations();

    std::vector<Particle> particles_;
    SimulationConfig config_;
    double time_{};
};

}