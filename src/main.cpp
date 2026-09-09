#include "fastphysics/csv_writer.hpp"
#include "fastphysics/scenarios.hpp"
#include "fastphysics/simulation.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <utility>

int main()
{
    constexpr std::size_t number_of_steps = 1000;
    constexpr std::size_t sample_interval = 10;

    const fastphysics::SimulationConfig config{
        .gravitational_constant = 1.0,
        .softening = 0.01,
        .dt = 0.001
    };

    fastphysics::Simulation simulation{
        fastphysics::make_two_body_system(),
        config
    };

    std::ofstream output{
        "results/two_body_euler.csv" // output file stream.
    };

    if (!output) {
        std::cerr
            << "Failed to open output file\n";

        return 1;
    }

    fastphysics::write_csv_header(output);

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
        << "FastPhysics - N-body CPU reference\n"
        << "Simulation time: "
        << simulation.time()
        << '\n'
        << "Results: results/two_body_euler.csv\n";
}