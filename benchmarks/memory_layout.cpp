#include "fastphysics/particle.hpp"
#include "fastphysics/vec3.hpp"

#include <cstddef>
#include <iostream>
#include <type_traits>

int main()
{
    using fastphysics::Particle;
    using fastphysics::Vec3;

    // offsetof is well-defined for standard-layout types.
    static_assert(std::is_standard_layout_v<Vec3>);
    static_assert(std::is_standard_layout_v<Particle>);

    std::cout
        << "\n"
        << "========================================\n"
        << " FastPhysics - Memory Layout\n"
        << "========================================\n\n";

    std::cout
        << "Primitive types\n"
        << "  sizeof(double)   : "
        << sizeof(double)
        << " bytes\n\n";

    std::cout
        << "Vec3\n"
        << "  sizeof(Vec3)     : "
        << sizeof(Vec3)
        << " bytes\n"
        << "  alignof(Vec3)    : "
        << alignof(Vec3)
        << " bytes\n\n";

    std::cout
        << "Particle\n"
        << "  sizeof(Particle) : "
        << sizeof(Particle)
        << " bytes\n"
        << "  alignof(Particle): "
        << alignof(Particle)
        << " bytes\n\n";

    std::cout
        << "Particle member offsets\n"
        << "  position         : "
        << offsetof(Particle, position)
        << " bytes\n"
        << "  velocity         : "
        << offsetof(Particle, velocity)
        << " bytes\n"
        << "  acceleration     : "
        << offsetof(Particle, acceleration)
        << " bytes\n"
        << "  mass             : "
        << offsetof(Particle, mass)
        << " bytes\n\n";

    return 0;
}