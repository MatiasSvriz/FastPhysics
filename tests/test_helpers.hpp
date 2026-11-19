
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace fp_test {

    inline bool almost_equal(
        double actual,
        double expected,
        double abs_tol = 1e-12,
        double rel_tol = 1e-12
    ) {
        if (!std::isfinite(actual) ||
            !std::isfinite(expected)) {
            return false;
        }

        return std::abs(actual - expected)
            <= abs_tol + rel_tol *
                std::max(std::abs(actual), std::abs(expected));
    }

    [[noreturn]] inline void fail(
        const char* expression,
        const char* file,
        int line
    ) {
        std::cerr << file << ":" << line
                  << " - Test failed: "
                  << expression << "\n";

        std::exit(EXIT_FAILURE);
    }

}

#define FP_CHECK(condition)                     \
    do {                                        \
        if (!(condition)) {                     \
            fp_test::fail(                      \
                #condition, __FILE__, __LINE__  \
            );                                  \
        }                                       \
    } while (false)
