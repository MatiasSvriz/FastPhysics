#include "fastphysics/csv_writer.hpp"

#include <cstddef>
#include <ostream>

namespace fastphysics {

void write_csv_header(std::ostream& output)
{
    output
        << "time,particle,x,y,z,vx,vy,vz\n";
}

void write_csv_snapshot(
    std::ostream& output,
    double time,
    const std::vector<Particle>& particles
)
{
    // Writing one snapshot visits each particle once, so O(N) time complexity.
    for (std::size_t i = 0; i < particles.size(); ++i) {
        const Particle& particle = particles[i];

        output
            << time << ','
            << i << ','
            << particle.position.x << ','
            << particle.position.y << ','
            << particle.position.z << ','
            << particle.velocity.x << ','
            << particle.velocity.y << ','
            << particle.velocity.z << '\n';
    }
}

}