#include "fastphysics/csv_writer.hpp"

#include <cstddef>
#include <ostream>

namespace fastphysics {

void write_csv_header(
    std::ostream& output
)
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
    // Writing one trajectory snapshot visits each particle once so O(N).
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

void write_energy_csv_header(
    std::ostream& output
)
{
    output
        << "time,kinetic,potential,total,relative_error\n";
}

void write_energy_csv_snapshot(
    std::ostream& output,
    double time,
    double kinetic,
    double potential,
    double total,
    double relative_error
)
{
    output
        << time << ','
        << kinetic << ','
        << potential << ','
        << total << ','
        << relative_error << '\n';
}

}