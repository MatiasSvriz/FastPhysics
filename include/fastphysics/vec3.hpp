#pragma once

namespace fastphysics {

struct Vec3 {
    double x{};
    double y{};
    double z{};

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(double scalar) const;

    Vec3& operator+=(const Vec3& other);
};

double norm_squared(const Vec3& v);
double norm(const Vec3& v);

}