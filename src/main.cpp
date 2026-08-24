#include "fastphysics/vec3.hpp"

#include <iostream>

int main()
{
    const fastphysics::Vec3 position{1.0, 2.0, 3.0};
    const fastphysics::Vec3 velocity{0.5, 1.0, -0.5};

    const fastphysics::Vec3 result = position + velocity;

    std::cout
        << result.x << ' '
        << result.y << ' '
        << result.z << '\n';

    std::cout
        << "Norm: "
        << fastphysics::norm(result)
        << '\n';
}