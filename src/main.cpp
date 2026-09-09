#include "fastphysics/csv_writer.hpp"
#include "fastphysics/scenarios.hpp"
#include "fastphysics/simulation.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>

namespace {

bool run_simulation(
    fastphysics::IntegrationMethod integration_method,
    const char* output_path
)
{
    constexpr std::size_t number_of_steps = 100000;
    constexpr std::size_t sample_interval = 100;

    const fastphysics::SimulationConfig config{
        .gravitational_constant = 1.0,
        .softening = 0.01,
        .dt = 0.001,
        .integration_method = integration_method
    };

    fastphysics::Simulation simulation{
        fastphysics::make_two_body_system(),
        config
    };

    std::ofstream output{
        output_path
    };

    if (!output) {
        std::cerr
            << "Failed to open output file: "
            << output_path
            << '\n';

        return false;
    }

    fastphysics::write_csv_header(output);

    // Store the initial state at t = 0.
    fastphysics::write_csv_snapshot(
        output,
        simulation.time(),
        simulation.particles()
    );

    for (std::size_t step = 1;
         step <= number_of_steps;
         ++step) {

        simulation.step();

        if (step % sample_interval == 0) {
            fastphysics::write_csv_snapshot(
                output,
                simulation.time(),
                simulation.particles()
            );
        }
    }

    std::cout
        << "  Simulation time: "
        << simulation.time()
        << '\n'
        << "  Results: "
        << output_path
        << "\n\n";

    return true;
}

}

int main()
{
    std::cout
        << "FastPhysics - Integrator comparison\n\n";

    std::cout
        << "Euler\n";

    if (!run_simulation(
            fastphysics::IntegrationMethod::Euler,
            "results/two_body_euler.csv"
        )) {

        return 1;
    }

    std::cout
        << "Velocity Verlet\n";

    if (!run_simulation(
            fastphysics::IntegrationMethod::VelocityVerlet,
            "results/two_body_verlet.csv"
        )) {

        return 1;
    }

    std::cout
        << "Integrator comparison completed.\n";

    return 0;
}