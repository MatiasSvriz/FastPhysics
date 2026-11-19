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

// Creates the cubic region for one of the parent's eight octants.
// The octant index must be between 0 and 7.
BarnesHutNode make_barnes_hut_child(
    const BarnesHutNode& parent,
    std::size_t octant
);

// Adds a child to nodes and stores its index in the parent.
// If that octant already has a child, returns its existing index.
// nodes must contain parent_index, and octant must be between 0 and 7.
int add_barnes_hut_child(
    std::vector<BarnesHutNode>& nodes,
    std::size_t parent_index,
    std::size_t octant
);

// Returns true when the node has no children.
bool barnes_hut_is_leaf(
    const BarnesHutNode& node
);

// Inserts one particle index into the octree.
// A full leaf is subdivided unless the maximum depth has been reached.
// Each particle index should be inserted once.
void insert_barnes_hut_particle(
    std::vector<BarnesHutNode>& nodes,
    const std::vector<Particle>& particles,
    std::size_t node_index,
    std::size_t particle_index
);

// Computes the total mass and center of mass of a node and its descendants.
// Uses postorder traversal, children are processed before their parent.
// Must be called after inserting all particles into the tree.
// Assumes nonnegative particle masses. Nodes with zero total mass
// receive a zero center-of-mass vector.
void update_barnes_hut_mass_properties(
    std::vector<BarnesHutNode>& nodes,
    const std::vector<Particle>& particles,
    std::size_t node_index
);

}