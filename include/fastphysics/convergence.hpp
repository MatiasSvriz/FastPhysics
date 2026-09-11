#pragma once

namespace fastphysics {

// Computes the relative error of a value with respect to a reference:
//
//     (value - reference) / |reference|
//
// The reference value must be non-zero.
double relative_error(
    double value,
    double reference
);

// Estimates the observed convergence order assuming:
//
//     error ≈ C * h^p
//
// where h represents the numerical resolution, such as the time step.
//
// Comparing two resolutions gives:
//
//     p = log(error_coarse / error_fine)
//       / log(h_coarse / h_fine)
//
// Step sizes and errors must be positive.
double estimate_convergence_order(
    double coarse_step,
    double coarse_error,
    double fine_step,
    double fine_error
);

}