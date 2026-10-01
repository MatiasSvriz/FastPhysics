#pragma once

#include "fastphysics/vec3.hpp"

#include "fastphysics/particle.hpp"

#include <array>
#include <cstddef>
#include <vector>

namespace fastphysics {

// A Barnes-Hut octree divides a three-dimensional region into eight
// smaller cubes at each level. Each node represents one cube and stores
// the total mass and center of mass of the particles inside it.
// This lets the gravity solver approximate distant groups of particles
// instead of evaluating every interaction individually.
struct BarnesHutNode {
    // Geometric center and half the side length of this cubic region.
    // These values determine the node's eight octants.
    Vec3 center{};
    double half_size{};

    // Child indices use the bits x=1, y=2, z=4.
    // -1 means that the child does not exist.
    std::array<int, 8> children{
        -1, -1, -1, -1,
        -1, -1, -1, -1
    };

    // A leaf can hold multiple particles, including coincident ones.
    std::vector<std::size_t> particle_indices;

    // Physical properties of all particles contained in this region.
    // The center of mass is generally different from the cube's center.
    double total_mass{};
    Vec3 center_of_mass{};
};

// Creates a cubic root region containing all particle positions.
// Returns an empty node when there are no particles.
BarnesHutNode make_barnes_hut_root(
    const std::vector<Particle>& particles
);

// Returns the octant containing position within node.
// A coordinate equal to the center belongs to the positive half.
std::size_t barnes_hut_octant(
    const BarnesHutNode& node,
    const Vec3& position
);

}