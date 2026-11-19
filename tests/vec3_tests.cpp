
#include "fastphysics/vec3.hpp"
#include "test_helpers.hpp"

int main()
{
    using fastphysics::Vec3;
    using fp_test::almost_equal;

    // --------------------------------------------------
    // Test 1: Vector addition
    // --------------------------------------------------

    {
        const Vec3 a{1.0, 2.0, 3.0};
        const Vec3 b{2.0, 3.0, 4.0};

        const Vec3 result = a + b;

        FP_CHECK(almost_equal(result.x, 3.0));
        FP_CHECK(almost_equal(result.y, 5.0));
        FP_CHECK(almost_equal(result.z, 7.0));
    }

    // --------------------------------------------------
    // Test 2: Vector subtraction
    // --------------------------------------------------

    {
        const Vec3 a{5.0, 5.0, 5.0};
        const Vec3 b{1.0, 2.0, 3.0};

        const Vec3 result = a - b;

        FP_CHECK(almost_equal(result.x, 4.0));
        FP_CHECK(almost_equal(result.y, 3.0));
        FP_CHECK(almost_equal(result.z, 2.0));
    }

    // --------------------------------------------------
    // Test 3: Scalar multiplication
    // --------------------------------------------------

    {
        const Vec3 v{1.0, 2.0, 3.0};

        const Vec3 result = v * 2.0;

        FP_CHECK(almost_equal(result.x, 2.0));
        FP_CHECK(almost_equal(result.y, 4.0));
        FP_CHECK(almost_equal(result.z, 6.0));
    }

    // --------------------------------------------------
    // Test 4: Addition assignment
    // --------------------------------------------------

    {
        Vec3 a{1.0, 2.0, 3.0};
        const Vec3 b{2.0, 3.0, 4.0};

        a += b;

        FP_CHECK(almost_equal(a.x, 3.0));
        FP_CHECK(almost_equal(a.y, 5.0));
        FP_CHECK(almost_equal(a.z, 7.0));
    }

    // --------------------------------------------------
    // Test 5: Vector norm and squared norm
    // --------------------------------------------------

    {
        const Vec3 v{3.0, 4.0, 0.0};

        FP_CHECK(almost_equal(
            fastphysics::norm_squared(v), 25.0
        ));

        FP_CHECK(almost_equal(
            fastphysics::norm(v), 5.0
        ));
    }

    return 0;
}
