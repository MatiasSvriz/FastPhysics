
#include "fastphysics/convergence.hpp"
#include "test_helpers.hpp"

int main()
{
    using fp_test::almost_equal;

    // -----------------------------------------------------------------------
    // Test 1: Relative error
    // -----------------------------------------------------------------------

    {
        const double error =
            fastphysics::relative_error(
                105.0,
                100.0
            );

        // Relative error:
        //
        // |105 - 100| / |100| = 0.05

        FP_CHECK(almost_equal(
            error,
            0.05
        ));
    }

    // -----------------------------------------------------------------------
    // Test 2: First-order convergence
    // -----------------------------------------------------------------------

    {
        // If halving the step size halves the error:
        //
        // error ~ h
        //
        // The observed convergence order must be p = 1.

        const double order =
            fastphysics::estimate_convergence_order(
                0.1,
                0.02,
                0.05,
                0.01
            );

        FP_CHECK(almost_equal(
            order,
            1.0
        ));
    }

    // -----------------------------------------------------------------------
    // Test 3: Second-order convergence
    // -----------------------------------------------------------------------

    {
        // If halving the step size reduces the error by four:
        //
        // error ~ h^2
        //
        // The observed convergence order must be p = 2.

        const double order =
            fastphysics::estimate_convergence_order(
                0.1,
                0.04,
                0.05,
                0.01
            );

        FP_CHECK(almost_equal(
            order,
            2.0
        ));
    }

    // -----------------------------------------------------------------------
    // Test 4: Arbitrary step-size ratio
    // -----------------------------------------------------------------------

    {
        // The formula must also work when the step size
        // is not reduced exactly by a factor of two.

        const double order =
            fastphysics::estimate_convergence_order(
                0.1,
                0.01,
                0.025,
                0.000625
            );

        FP_CHECK(almost_equal(
            order,
            2.0
        ));
    }

    return 0;
}
