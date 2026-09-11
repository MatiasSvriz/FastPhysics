#include "fastphysics/convergence.hpp"

#include <cassert>
#include <cmath>

namespace {

bool almost_equal(
    double a,
    double b,
    double epsilon = 1e-12
)
{
    return std::abs(a - b) < epsilon;
}

}

int main()
{
    // -----------------------------------------------------------------------
    // Relative error
    // -----------------------------------------------------------------------

    {
        const double error =
            fastphysics::relative_error(
                105.0,
                100.0
            );

        assert(almost_equal(
            error,
            0.05
        ));
    }

    // -----------------------------------------------------------------------
    // First-order convergence
    // -----------------------------------------------------------------------

    {
        // If halving the step size halves the error:
        //
        // error ~ h
        //
        // the observed order must be p = 1.
        const double order =
            fastphysics::estimate_convergence_order(
                0.1,
                0.02,
                0.05,
                0.01
            );

        assert(almost_equal(
            order,
            1.0
        ));
    }

    // -----------------------------------------------------------------------
    // Second-order convergence
    // -----------------------------------------------------------------------

    {
        // If halving the step size reduces the error by four:
        //
        // error ~ h^2
        //
        // the observed order must be p = 2.
        const double order =
            fastphysics::estimate_convergence_order(
                0.1,
                0.04,
                0.05,
                0.01
            );

        assert(almost_equal(
            order,
            2.0
        ));
    }

    // -----------------------------------------------------------------------
    // Arbitrary step-size ratio
    // -----------------------------------------------------------------------

    {
        // The general formula must also work when the step size
        // is not reduced exactly by a factor of two.
        const double order =
            fastphysics::estimate_convergence_order(
                0.1,
                0.01,
                0.025,
                0.000625
            );

        assert(almost_equal(
            order,
            2.0
        ));
    }
}