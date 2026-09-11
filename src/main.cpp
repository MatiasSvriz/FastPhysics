#include "fastphysics/convergence.hpp"
#include "fastphysics/energy.hpp"
#include "fastphysics/scenarios.hpp"
#include "fastphysics/simulation.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace {

struct SimulationResult {
    std::size_t number_of_steps{};
    double final_relative_energy_error{};
    double maximum_relative_energy_error{};
};

SimulationResult run_experiment(
    fastphysics::IntegrationMethod integration_method,
    double dt,
    double final_time
)
{
    const auto number_of_steps =
        static_cast<std::size_t>(
            std::llround(final_time / dt)
        );

    const fastphysics::SimulationConfig config{
        .gravitational_constant = 1.0,
        .softening = 0.01,
        .dt = dt,
        .integration_method = integration_method
    };

    fastphysics::Simulation simulation{
        fastphysics::make_two_body_system(),
        config
    };

    const double initial_energy =
        fastphysics::total_energy(
            simulation.particles(),
            config.gravitational_constant,
            config.softening
        );

    double maximum_relative_energy_error = 0.0;
    double final_relative_energy_error = 0.0;

    for (std::size_t step = 0;
         step < number_of_steps;
         ++step) {

        simulation.step();

        const double current_energy =
            fastphysics::total_energy(
                simulation.particles(),
                config.gravitational_constant,
                config.softening
            );

        final_relative_energy_error =
            fastphysics::relative_error(
                current_energy,
                initial_energy
            );

        maximum_relative_energy_error =
            std::max(
                maximum_relative_energy_error,
                std::abs(final_relative_energy_error)
            );
    }

    return {
        number_of_steps,
        final_relative_energy_error,
        maximum_relative_energy_error
    };
}

void run_convergence_study(
    const char* name,
    fastphysics::IntegrationMethod integration_method,
    int theoretical_order,
    std::ofstream& output
)
{
    constexpr double final_time = 100.0;

    constexpr double time_steps[]{
        0.1,
        0.05,
        0.025,
        0.0125,
        0.00625,
        0.003125,
        0.0015625,
        0.00078125
    };

    std::cout
        << name
        << "  [theoretical order: "
        << theoretical_order
        << "]\n";

    std::cout
        << std::left
        << std::setw(12) << "dt"
        << std::setw(12) << "steps"
        << std::setw(18) << "final error"
        << std::setw(18) << "max error"
        << "observed p\n";

    std::cout
        << "-------------------------------------------------------------\n";

    double previous_dt = 0.0;
    double previous_error = 0.0;
    bool has_previous_result = false;

    for (const double dt : time_steps) {

        const SimulationResult result =
            run_experiment(
                integration_method,
                dt,
                final_time
            );

        std::cout
            << std::scientific
            << std::setprecision(4)
            << std::left
            << std::setw(12) << dt
            << std::setw(12) << result.number_of_steps
            << std::setw(18)
            << result.final_relative_energy_error
            << std::setw(18)
            << result.maximum_relative_energy_error;

        output
            << name << ','
            << dt << ','
            << result.number_of_steps << ','
            << result.final_relative_energy_error << ','
            << result.maximum_relative_energy_error << ',';

        if (has_previous_result) {
            const double observed_order =
                fastphysics::estimate_convergence_order(
                    previous_dt,
                    previous_error,
                    dt,
                    result.maximum_relative_energy_error
                );

            std::cout
                << std::fixed
                << std::setprecision(3)
                << observed_order;

            output
                << observed_order;
        } else {
            std::cout << '-';
        }

        std::cout << '\n';
        output << '\n';

        previous_dt = dt;
        previous_error =
            result.maximum_relative_energy_error;

        has_previous_result = true;
    }

    std::cout
        << "\n"
        << "Note: observed p is estimated from maximum relative energy error.\n"
        << "It is a numerical diagnostic and does not necessarily equal the\n"
        << "formal convergence order for every tested time step.\n\n";
}

}

int main()
{
    std::ofstream output{
        "results/convergence.csv"
    };

    if (!output) {
        std::cerr
            << "Failed to open results/convergence.csv\n";

        return 1;
    }

    output
        << "integrator,dt,steps,final_error,max_error,observed_order\n";

    std::cout
        << "\n"
        << "=============================================================\n"
        << " FastPhysics - N-body Time Step Convergence Study\n"
        << "=============================================================\n\n"
        << "Scenario    : symmetric two-body gravitational orbit\n"
        << "Final time  : 100 normalized time units\n"
        << "Diagnostic  : relative total-energy error\n"
        << "Refinement  : dt -> dt / 2\n\n";

    run_convergence_study(
        "Euler",
        fastphysics::IntegrationMethod::Euler,
        1,
        output
    );

    run_convergence_study(
        "Velocity Verlet",
        fastphysics::IntegrationMethod::VelocityVerlet,
        2,
        output
    );

    std::cout
        << "=============================================================\n"
        << " Study completed\n"
        << " Results: results/convergence.csv\n"
        << "=============================================================\n\n";

    return 0;
}