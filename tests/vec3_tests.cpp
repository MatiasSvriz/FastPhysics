#include "fastphysics/vec3.hpp"

#include <cassert>
#include <cmath>

namespace {

bool almost_equal(double a, double b, double epsilon = 1e-12)
{
    return std::abs(a - b) < epsilon;
}

}

int main()
{
    using fastphysics::Vec3;

    {
        const Vec3 a{1.0, 2.0, 3.0};
        const Vec3 b{2.0, 3.0, 4.0};

        const Vec3 result = a + b;

        assert(almost_equal(result.x, 3.0));
        assert(almost_equal(result.y, 5.0));
        assert(almost_equal(result.z, 7.0));
    }

    {
        const Vec3 a{5.0, 5.0, 5.0};
        const Vec3 b{1.0, 2.0, 3.0};

        const Vec3 result = a - b;

        assert(almost_equal(result.x, 4.0));
        assert(almost_equal(result.y, 3.0));
        assert(almost_equal(result.z, 2.0));
    }

    {
        const Vec3 v{1.0, 2.0, 3.0};

        const Vec3 result = v * 2.0;

        assert(almost_equal(result.x, 2.0));
        assert(almost_equal(result.y, 4.0));
        assert(almost_equal(result.z, 6.0));
    }

    {
        Vec3 a{1.0, 2.0, 3.0};
        const Vec3 b{2.0, 3.0, 4.0};

        a += b;

        assert(almost_equal(a.x, 3.0));
        assert(almost_equal(a.y, 5.0));
        assert(almost_equal(a.z, 7.0));
    }

    {
        const Vec3 v{3.0, 4.0, 0.0};

        assert(almost_equal(fastphysics::norm_squared(v), 25.0));
        assert(almost_equal(fastphysics::norm(v), 5.0));
    }
}