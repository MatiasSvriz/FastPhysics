#include "fastphysics/barnes_hut.hpp"

#include <cassert>
#include <vector>

int main()
{
    // The default node is centered at the origin.
    const fastphysics::BarnesHutNode node{};

    // Each positive axis contributes to the octant index:
    // x contributes 1, y contributes 2, and z contributes 4.
    assert(fastphysics::barnes_hut_octant(node, {-1.0, -1.0, -1.0}) == 0);
    assert(fastphysics::barnes_hut_octant(node, { 1.0, -1.0, -1.0}) == 1);
    assert(fastphysics::barnes_hut_octant(node, {-1.0,  1.0, -1.0}) == 2);
    assert(fastphysics::barnes_hut_octant(node, {-1.0, -1.0,  1.0}) == 4);
    assert(fastphysics::barnes_hut_octant(node, { 1.0,  1.0,  1.0}) == 7);

    // A coordinate equal to the center belongs to the positive half.
    assert(fastphysics::barnes_hut_octant(node, {0.0, 0.0, 0.0}) == 7);

    // Root cube

    std::vector<fastphysics::Particle> particles(2);

    particles[0].position = {-2.0, 1.0, 0.0};
    particles[1].position = { 4.0, 3.0, 2.0};

    const fastphysics::BarnesHutNode root =
        fastphysics::make_barnes_hut_root(particles);

    // The midpoint of the particle bounds is (1, 2, 1).
    assert(root.center.x == 1.0);
    assert(root.center.y == 2.0);
    assert(root.center.z == 1.0);

    // The x range [-2, 4] is the widest: the cube has side length 6.
    assert(root.half_size == 3.0);
}