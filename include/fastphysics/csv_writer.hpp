#pragma once

#include "fastphysics/particle.hpp"

#include <iosfwd>
#include <vector>

namespace fastphysics {

// Writes the CSV column names used for trajectory output.
void write_csv_header(std::ostream& output);

// Writes one snapshot of the complete particle system.
void write_csv_snapshot(
    std::ostream& output,
    double time,
    const std::vector<Particle>& particles
);

}