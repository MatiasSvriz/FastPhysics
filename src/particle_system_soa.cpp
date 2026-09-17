#include "fastphysics/particle_system_soa.hpp"

namespace fastphysics {

ParticleSystemSoA make_particle_system_soa(
    const std::vector<Particle>& particles
)
{
    ParticleSystemSoA system;

    // We already know how many particles will be stored, so reserve the
    // required capacity once instead of letting the vectors grow repeatedly.
    const std::size_t particle_count = particles.size();

    system.position_x.reserve(particle_count);
    system.position_y.reserve(particle_count);
    system.position_z.reserve(particle_count);

    system.velocity_x.reserve(particle_count);
    system.velocity_y.reserve(particle_count);
    system.velocity_z.reserve(particle_count);

    system.acceleration_x.reserve(particle_count);
    system.acceleration_y.reserve(particle_count);
    system.acceleration_z.reserve(particle_count);

    system.mass.reserve(particle_count);

    // Split each Particle into the corresponding SoA arrays.
    // The same index is kept across every array.
    for (const Particle& particle : particles) {

        system.position_x.push_back(particle.position.x);
        system.position_y.push_back(particle.position.y);
        system.position_z.push_back(particle.position.z);

        system.velocity_x.push_back(particle.velocity.x);
        system.velocity_y.push_back(particle.velocity.y);
        system.velocity_z.push_back(particle.velocity.z);

        system.acceleration_x.push_back(particle.acceleration.x);
        system.acceleration_y.push_back(particle.acceleration.y);
        system.acceleration_z.push_back(particle.acceleration.z);

        system.mass.push_back(particle.mass);
    }

    return system;
}

}