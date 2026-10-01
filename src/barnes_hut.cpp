#include "fastphysics/barnes_hut.hpp"

#include <algorithm>

namespace fastphysics {

BarnesHutNode make_barnes_hut_root(
    const std::vector<Particle>& particles
)
{
    if (particles.empty()) {
        return {};
    }

    Vec3 minimum = particles[0].position;
    Vec3 maximum = particles[0].position;

    for (const Particle& particle : particles) {
        minimum.x = std::min(minimum.x, particle.position.x);
        minimum.y = std::min(minimum.y, particle.position.y);
        minimum.z = std::min(minimum.z, particle.position.z);

        maximum.x = std::max(maximum.x, particle.position.x);
        maximum.y = std::max(maximum.y, particle.position.y);
        maximum.z = std::max(maximum.z, particle.position.z);
    }

    const double size_x = maximum.x - minimum.x;
    const double size_y = maximum.y - minimum.y;
    const double size_z = maximum.z - minimum.z;

    // Use the widest coordinate range for all three sides.
    // Shorter dimensions leave empty space but all particles fit in one cube.
    const double largest_size = std::max({
        size_x,
        size_y,
        size_z
    });

    BarnesHutNode root;

    root.center = {
        minimum.x + size_x / 2.0,
        minimum.y + size_y / 2.0,
        minimum.z + size_z / 2.0
    };

    root.half_size = largest_size / 2.0;

    // Coincident particles produce a cube of zero size. Give the root
    // a nonzero size so it can be subdivided in a later step.
    if (root.half_size == 0.0) {
        root.half_size = 1.0;
    }

    return root;
}

std::size_t barnes_hut_octant(
    const BarnesHutNode& node,
    const Vec3& position
)
{
    // Each axis contributes one bit to the child index.
    // Points on a central plane belong to its positive half.
    std::size_t octant = 0;

    if (position.x >= node.center.x) {
        octant += 1;
    }

    if (position.y >= node.center.y) {
        octant += 2;
    }

    if (position.z >= node.center.z) {
        octant += 4;
    }

    return octant;
}

}