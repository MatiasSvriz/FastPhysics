#include "fastphysics/convergence.hpp"

#include <cmath>
#include <stdexcept>

namespace fastphysics {

double relative_error(
    double value,
    double reference
)
{
    if (reference == 0.0) {
        throw std::invalid_argument(
            "Relative error requires a non-zero reference value"
        );
    }

    return (value - reference)
         / std::abs(reference);
}

double estimate_convergence_order(
    double coarse_step,
    double coarse_error,
    double fine_step,
    double fine_error
)
{
    if (
        coarse_step <= 0.0
        || fine_step <= 0.0
        || coarse_error <= 0.0
        || fine_error <= 0.0
    ) {
        throw std::invalid_argument(
            "Convergence order requires positive step sizes and errors"
        );
    }

    if (coarse_step == fine_step) {
        throw std::invalid_argument(
            "Convergence order requires different step sizes"
        );
    }

    return std::log(coarse_error / fine_error)
         / std::log(coarse_step / fine_step);
}

}