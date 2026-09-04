#pragma once

#include "fastphysics/particle.hpp"

#include <cstddef>
#include <vector>

namespace fastphysics {

struct SimulationConfig {
    double gravitational_constant{1.0};
    double softening{0.01};
    double dt{0.001};
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
    std::vector<Particle> particles_;
    SimulationConfig config_;
    double time_{};
};

}