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

BarnesHutNode make_barnes_hut_child(
    const BarnesHutNode& parent,
    std::size_t octant
)
{
    BarnesHutNode child;

    // A child has half the side length of its parent.
    child.half_size = parent.half_size / 2.0;

    // Each bit selects the negative or positive half of one axis.
    child.center = {
        parent.center.x
            + ((octant & 1) == 0 ? -child.half_size : child.half_size),

        parent.center.y
            + ((octant & 2) == 0 ? -child.half_size : child.half_size),

        parent.center.z
            + ((octant & 4) == 0 ? -child.half_size : child.half_size)
    };

    return child;
}

int add_barnes_hut_child(
    std::vector<BarnesHutNode>& nodes,
    std::size_t parent_index,
    std::size_t octant
)
{
    const int existing_child =
        nodes[parent_index].children[octant];

    if (existing_child != -1) {
        return existing_child;
    }

    BarnesHutNode child =
        make_barnes_hut_child(nodes[parent_index], octant);

    const int child_index =
        static_cast<int>(nodes.size());

    // push_back may reallocate the vector and move its nodes in memory.
    // Access the parent by index again after adding the child.
    nodes.push_back(child);

    nodes[parent_index].children[octant] = child_index;

    return child_index;
}

bool barnes_hut_is_leaf(
    const BarnesHutNode& node
)
{
    for (int child_index : node.children) {
        if (child_index != -1) {
            return false;
        }
    }

    return true;
}

namespace {

constexpr std::size_t leaf_capacity = 4;
constexpr std::size_t maximum_depth = 32;

void insert_particle_at_depth(
    std::vector<BarnesHutNode>& nodes,
    const std::vector<Particle>& particles,
    std::size_t node_index,
    std::size_t particle_index,
    std::size_t depth
)
{
    if (barnes_hut_is_leaf(nodes[node_index])) {

        if (
            nodes[node_index].particle_indices.size() < leaf_capacity
            || depth >= maximum_depth
        ) {
            nodes[node_index].particle_indices.push_back(particle_index);
            return;
        }

        // Copy the indices before adding children: push_back may move nodes.
        const std::vector<std::size_t> previous_indices =
            nodes[node_index].particle_indices;

        nodes[node_index].particle_indices.clear();

        // Redistribute the particles previously stored in this leaf.
        for (std::size_t previous_index : previous_indices) {

            const std::size_t octant = barnes_hut_octant(
                nodes[node_index],
                particles[previous_index].position
            );

            const int child_index =
                add_barnes_hut_child(nodes, node_index, octant);

            insert_particle_at_depth(
                nodes,
                particles,
                static_cast<std::size_t>(child_index),
                previous_index,
                depth + 1
            );
        }
    }

    // Descend with the new particle after any required subdivision.
    const std::size_t octant = barnes_hut_octant(
        nodes[node_index],
        particles[particle_index].position
    );

    const int child_index =
        add_barnes_hut_child(nodes, node_index, octant);

    insert_particle_at_depth(
        nodes,
        particles,
        static_cast<std::size_t>(child_index),
        particle_index,
        depth + 1
    );
}

}

void insert_barnes_hut_particle(
    std::vector<BarnesHutNode>& nodes,
    const std::vector<Particle>& particles,
    std::size_t node_index,
    std::size_t particle_index
)
{
    insert_particle_at_depth(
        nodes,
        particles,
        node_index,
        particle_index,
        0
    );
}

// Computes the physical properties of a node and its descendants.
void update_barnes_hut_mass_properties(
    std::vector<BarnesHutNode>& nodes,
    const std::vector<Particle>& particles,
    std::size_t node_index
)
{
    BarnesHutNode& node = nodes[node_index];

    double total_mass = 0.0;
    Vec3 weighted_position{};

    if (barnes_hut_is_leaf(node)) {

        // Leaf node. Accumulate the mass and weighted position
        // of every particle stored in this leaf.
        for (std::size_t particle_index : node.particle_indices) {

            const Particle& particle = particles[particle_index];

            total_mass += particle.mass;

            weighted_position +=
                particle.position * particle.mass;
        }

    } else {

        // Internal node. Recursively compute the properties
        // of each child before aggregating them.
        for (int child_index : node.children) {

            if (child_index == -1) {
                continue;
            }

            update_barnes_hut_mass_properties(
                nodes,
                particles,
                static_cast<std::size_t>(child_index)
            );

            const BarnesHutNode& child = nodes[child_index];

            total_mass += child.total_mass;

            weighted_position +=
                child.center_of_mass * child.total_mass;
        }
    }

    // Store the total mass of this node and its descendants.
    node.total_mass = total_mass;

    // The center of mass is the mass-weighted average position.
    // An empty or zero-mass node has no defined center of mass,
    // so we store a zero vector to avoid division by zero.
    if (total_mass > 0.0) {

        node.center_of_mass = {
            weighted_position.x / total_mass,
            weighted_position.y / total_mass,
            weighted_position.z / total_mass
        };

    } else {

        node.center_of_mass = {};
    }
}

}