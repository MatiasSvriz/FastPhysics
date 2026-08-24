#include "fastphysics/vec3.hpp"

#include <cmath>

namespace fastphysics {

Vec3 Vec3::operator+(const Vec3& other) const
{
    return {
        x + other.x,
        y + other.y,
        z + other.z
    };
}

Vec3 Vec3::operator-(const Vec3& other) const
{
    return {
        x - other.x,
        y - other.y,
        z - other.z
    };
}

Vec3 Vec3::operator*(double scalar) const
{
    return {
        x * scalar,
        y * scalar,
        z * scalar
    };
}

Vec3& Vec3::operator+=(const Vec3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

double norm_squared(const Vec3& v)
{
    // Avoid sqrt when only the squared magnitude is required.
    // This is cheaper and will be useful for distance comparisons.
    return v.x * v.x
         + v.y * v.y
         + v.z * v.z;
}

double norm(const Vec3& v)
{
    return std::sqrt(norm_squared(v));
}

}