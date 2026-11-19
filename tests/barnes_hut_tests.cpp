
#include "fastphysics/barnes_hut.hpp"
#include "test_helpers.hpp"

#include <cstddef>
#include <vector>

int main()
{
    using fp_test::almost_equal;

    // -----------------------------------------------------------------------
    // Octant selection
    // -----------------------------------------------------------------------

    // The default node is centered at the origin.
    const fastphysics::BarnesHutNode node{};

    // Each positive axis contributes to the octant index:
    // x contributes 1, y contributes 2, and z contributes 4.
    FP_CHECK(fastphysics::barnes_hut_octant(node, {-1.0, -1.0, -1.0}) == 0);
    FP_CHECK(fastphysics::barnes_hut_octant(node, { 1.0, -1.0, -1.0}) == 1);
    FP_CHECK(fastphysics::barnes_hut_octant(node, {-1.0,  1.0, -1.0}) == 2);
    FP_CHECK(fastphysics::barnes_hut_octant(node, {-1.0, -1.0,  1.0}) == 4);
    FP_CHECK(fastphysics::barnes_hut_octant(node, { 1.0,  1.0,  1.0}) == 7);

    // A coordinate equal to the center belongs to the positive half.
    FP_CHECK(fastphysics::barnes_hut_octant(node, {0.0, 0.0, 0.0}) == 7);

    // -----------------------------------------------------------------------
    // Root cube
    // -----------------------------------------------------------------------

    std::vector<fastphysics::Particle> particles(2);

    particles[0].position = {-2.0, 1.0, 0.0};
    particles[1].position = { 4.0, 3.0, 2.0};

    const fastphysics::BarnesHutNode root =
        fastphysics::make_barnes_hut_root(particles);

    // The midpoint of the particle bounds is (1, 2, 1).
    FP_CHECK(almost_equal(root.center.x, 1.0));
    FP_CHECK(almost_equal(root.center.y, 2.0));
    FP_CHECK(almost_equal(root.center.z, 1.0));

    // The x range [-2, 4] is the widest: the cube has side length 6.
    FP_CHECK(almost_equal(root.half_size, 3.0));

    // -----------------------------------------------------------------------
    // Child cube geometry
    // -----------------------------------------------------------------------

    const fastphysics::BarnesHutNode child_0 =
        fastphysics::make_barnes_hut_child(root, 0);

    FP_CHECK(almost_equal(child_0.half_size, 1.5));
    FP_CHECK(almost_equal(child_0.center.x, -0.5));
    FP_CHECK(almost_equal(child_0.center.y, 0.5));
    FP_CHECK(almost_equal(child_0.center.z, -0.5));

    const fastphysics::BarnesHutNode child_7 =
        fastphysics::make_barnes_hut_child(root, 7);

    FP_CHECK(almost_equal(child_7.half_size, 1.5));
    FP_CHECK(almost_equal(child_7.center.x, 2.5));
    FP_CHECK(almost_equal(child_7.center.y, 3.5));
    FP_CHECK(almost_equal(child_7.center.z, 2.5));

    // -----------------------------------------------------------------------
    // Child storage and parent link
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::BarnesHutNode> nodes{root};

        FP_CHECK(fastphysics::barnes_hut_is_leaf(nodes[0]));

        const int child_index =
            fastphysics::add_barnes_hut_child(nodes, 0, 7);

        FP_CHECK(child_index == 1);
        FP_CHECK(nodes.size() == 2);
        FP_CHECK(nodes[0].children[7] == child_index);

        FP_CHECK(!fastphysics::barnes_hut_is_leaf(nodes[0]));
        FP_CHECK(fastphysics::barnes_hut_is_leaf(nodes[child_index]));

        FP_CHECK(almost_equal(nodes[child_index].center.x, 2.5));
        FP_CHECK(almost_equal(nodes[child_index].center.y, 3.5));
        FP_CHECK(almost_equal(nodes[child_index].center.z, 2.5));
        FP_CHECK(almost_equal(nodes[child_index].half_size, 1.5));

        // Asking for the same octant again must not create a duplicate node.
        const int same_child_index =
            fastphysics::add_barnes_hut_child(nodes, 0, 7);

        FP_CHECK(same_child_index == child_index);
        FP_CHECK(nodes.size() == 2);
    }

    // -----------------------------------------------------------------------
    // Particle insertion through existing children
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::BarnesHutNode> tree_nodes{root};

        // Make the root an internal node before inserting particles.
        fastphysics::add_barnes_hut_child(tree_nodes, 0, 7);

        fastphysics::insert_barnes_hut_particle(
            tree_nodes,
            particles,
            0,
            0
        );

        fastphysics::insert_barnes_hut_particle(
            tree_nodes,
            particles,
            0,
            1
        );

        const int child_0_index = tree_nodes[0].children[0];
        const int child_7_index = tree_nodes[0].children[7];

        FP_CHECK(child_0_index != -1);
        FP_CHECK(child_7_index != -1);
        FP_CHECK(tree_nodes[0].particle_indices.empty());

        FP_CHECK(tree_nodes[child_0_index].particle_indices.size() == 1);
        FP_CHECK(tree_nodes[child_0_index].particle_indices[0] == 0);

        FP_CHECK(tree_nodes[child_7_index].particle_indices.size() == 1);
        FP_CHECK(tree_nodes[child_7_index].particle_indices[0] == 1);
    }

    // -----------------------------------------------------------------------
    // Leaf subdivision
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::Particle> split_particles(5);

        split_particles[0].position = {-1.0, -1.0, -1.0};
        split_particles[1].position = { 1.0, -1.0, -1.0};
        split_particles[2].position = {-1.0,  1.0, -1.0};
        split_particles[3].position = {-1.0, -1.0,  1.0};
        split_particles[4].position = { 1.0,  1.0,  1.0};

        std::vector<fastphysics::BarnesHutNode> tree_nodes{
            fastphysics::make_barnes_hut_root(split_particles)
        };

        for (std::size_t i = 0; i < split_particles.size(); ++i) {
            fastphysics::insert_barnes_hut_particle(
                tree_nodes,
                split_particles,
                0,
                i
            );
        }

        // The fifth particle causes the full root leaf to subdivide.
        FP_CHECK(!fastphysics::barnes_hut_is_leaf(tree_nodes[0]));
        FP_CHECK(tree_nodes[0].particle_indices.empty());

        // Each original particle index must occur in exactly one leaf.
        std::vector<int> occurrences(split_particles.size(), 0);

        for (const auto& tree_node : tree_nodes) {
            if (fastphysics::barnes_hut_is_leaf(tree_node)) {
                for (std::size_t index : tree_node.particle_indices) {
                    FP_CHECK(index < occurrences.size());
                    ++occurrences[index];
                }
            }
        }

        for (int count : occurrences) {
            FP_CHECK(count == 1);
        }
    }

    // -----------------------------------------------------------------------
    // Empty particle collection
    // -----------------------------------------------------------------------

    {
        const std::vector<fastphysics::Particle> empty_particles;

        const fastphysics::BarnesHutNode empty_root =
            fastphysics::make_barnes_hut_root(empty_particles);

        // An empty collection has no spatial region to subdivide.
        FP_CHECK(almost_equal(empty_root.half_size, 0.0));
        FP_CHECK(empty_root.particle_indices.empty());
        FP_CHECK(fastphysics::barnes_hut_is_leaf(empty_root));
    }

    // -----------------------------------------------------------------------
    // Single particle
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::Particle> single_particle(1);
        single_particle[0].position = {3.0, -2.0, 4.0};

        std::vector<fastphysics::BarnesHutNode> tree_nodes{
            fastphysics::make_barnes_hut_root(single_particle)
        };

        fastphysics::insert_barnes_hut_particle(
            tree_nodes,
            single_particle,
            0,
            0
        );

        // A single particle stays in the root leaf.
        FP_CHECK(tree_nodes.size() == 1);
        FP_CHECK(fastphysics::barnes_hut_is_leaf(tree_nodes[0]));
        FP_CHECK(tree_nodes[0].particle_indices.size() == 1);
        FP_CHECK(tree_nodes[0].particle_indices[0] == 0);
    }

    // -----------------------------------------------------------------------
    // Coincident particles
    // -----------------------------------------------------------------------

    {
        std::vector<fastphysics::Particle> coincident_particles(6);

        for (auto& particle : coincident_particles) {
            particle.position = {0.0, 0.0, 0.0};
        }

        std::vector<fastphysics::BarnesHutNode> tree_nodes{
            fastphysics::make_barnes_hut_root(coincident_particles)
        };

        for (std::size_t i = 0; i < coincident_particles.size(); ++i) {
            fastphysics::insert_barnes_hut_particle(
                tree_nodes,
                coincident_particles,
                0,
                i
            );
        }

        // Coincident positions always choose the same octant.
        // The depth limit must stop subdivision and retain every index.
        FP_CHECK(tree_nodes.size() == 33); // Root plus 32 child levels.

        std::vector<int> occurrences(coincident_particles.size(), 0);
        std::size_t nonempty_leaves = 0;

        for (const auto& tree_node : tree_nodes) {
            if (
                fastphysics::barnes_hut_is_leaf(tree_node)
                && !tree_node.particle_indices.empty()
            ) {
                ++nonempty_leaves;

                for (std::size_t index : tree_node.particle_indices) {
                    FP_CHECK(index < occurrences.size());
                    ++occurrences[index];
                }
            }
        }

        FP_CHECK(nonempty_leaves == 1);

        for (int count : occurrences) {
            FP_CHECK(count == 1);
        }
    }

    // -----------------------------------------------------------------------
    // Mass properties of a leaf
    // -----------------------------------------------------------------------

    {
        // Two particles with different masses and positions.
        // The root remains a leaf because its capacity is four.
        std::vector<fastphysics::Particle> mass_particles{
            {
                {-2.0, 1.0, 0.0},
                {},
                {},
                2.0
            },
            {
                {4.0, 3.0, 2.0},
                {},
                {},
                3.0
            }
        };

        std::vector<fastphysics::BarnesHutNode> tree_nodes{
            fastphysics::make_barnes_hut_root(mass_particles)
        };

        for (std::size_t i = 0; i < mass_particles.size(); ++i) {
            fastphysics::insert_barnes_hut_particle(
                tree_nodes,
                mass_particles,
                0,
                i
            );
        }

        // Calculate the physical properties of the root.
        fastphysics::update_barnes_hut_mass_properties(
            tree_nodes,
            mass_particles,
            0
        );

        FP_CHECK(fastphysics::barnes_hut_is_leaf(tree_nodes[0]));

        // Total mass = 2 + 3 = 5.
        FP_CHECK(almost_equal(tree_nodes[0].total_mass, 5.0));

        // Center of mass:
        // x = (2 * -2 + 3 * 4) / 5 = 1.6
        // y = (2 *  1 + 3 * 3) / 5 = 2.2
        // z = (2 *  0 + 3 * 2) / 5 = 1.2
        FP_CHECK(almost_equal(tree_nodes[0].center_of_mass.x, 1.6));
        FP_CHECK(almost_equal(tree_nodes[0].center_of_mass.y, 2.2));
        FP_CHECK(almost_equal(tree_nodes[0].center_of_mass.z, 1.2));

        // Recomputing the properties must not duplicate mass.
        fastphysics::update_barnes_hut_mass_properties(
            tree_nodes,
            mass_particles,
            0
        );

        FP_CHECK(almost_equal(tree_nodes[0].total_mass, 5.0));
    }

    // -----------------------------------------------------------------------
    // Mass properties after subdivision
    // -----------------------------------------------------------------------

    {
        // Five particles force the root to subdivide.
        // Each particle has a different mass.
        std::vector<fastphysics::Particle> mass_particles{
            {{-1.0, -1.0, -1.0}, {}, {}, 1.0},
            {{ 1.0, -1.0, -1.0}, {}, {}, 2.0},
            {{-1.0,  1.0, -1.0}, {}, {}, 3.0},
            {{-1.0, -1.0,  1.0}, {}, {}, 4.0},
            {{ 1.0,  1.0,  1.0}, {}, {}, 5.0}
        };

        std::vector<fastphysics::BarnesHutNode> tree_nodes{
            fastphysics::make_barnes_hut_root(mass_particles)
        };

        for (std::size_t i = 0; i < mass_particles.size(); ++i) {
            fastphysics::insert_barnes_hut_particle(
                tree_nodes,
                mass_particles,
                0,
                i
            );
        }

        // Verify that the root was subdivided.
        FP_CHECK(!fastphysics::barnes_hut_is_leaf(tree_nodes[0]));
        FP_CHECK(tree_nodes[0].particle_indices.empty());

        // Recursively calculate mass properties of all nodes.
        fastphysics::update_barnes_hut_mass_properties(
            tree_nodes,
            mass_particles,
            0
        );

        // Total mass = 1 + 2 + 3 + 4 + 5 = 15.
        FP_CHECK(almost_equal(tree_nodes[0].total_mass, 15.0));

        // Mass-weighted coordinates:
        // x = -1 + 2 - 3 - 4 + 5 = -1
        // y = -1 - 2 + 3 - 4 + 5 =  1
        // z = -1 - 2 - 3 + 4 + 5 =  3
        //
        // Divide each component by the total mass.
        FP_CHECK(almost_equal(
            tree_nodes[0].center_of_mass.x,
            -1.0 / 15.0
        ));

        FP_CHECK(almost_equal(
            tree_nodes[0].center_of_mass.y,
            1.0 / 15.0
        ));

        FP_CHECK(almost_equal(
            tree_nodes[0].center_of_mass.z,
            3.0 / 15.0
        ));

        // The sum of the masses of the direct children
        // must equal the mass stored in the parent.
        double children_mass = 0.0;

        for (int child_index : tree_nodes[0].children) {

            if (child_index != -1) {
                children_mass += tree_nodes[child_index].total_mass;
            }
        }

        FP_CHECK(almost_equal(
            children_mass,
            tree_nodes[0].total_mass
        ));
    }

    return 0;
}
