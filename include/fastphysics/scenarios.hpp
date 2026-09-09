#pragma once

#include "fastphysics/particle.hpp"

#include <vector>

namespace fastphysics {

// Creates a deterministic two-body system with equal masses and symmetric initial conditions around the origin.
std::vector<Particle> make_two_body_system();

}