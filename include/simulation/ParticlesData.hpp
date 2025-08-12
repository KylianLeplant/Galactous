#ifndef PARTICLES_DATA_HPP
#define PARTICLES_DATA_HPP

#include <vector>
#include "types.hpp"


// struct containing the data of particles necessary for simulation in GPU mode
struct ParticlesData {
    std::vector<Vec3> positions; // Positions of the particles
    std::vector<float> masses; // Masses of the particles
    std::vector<Vec3> velocities; // Velocities of the particles
};

using ParticlesDataPtr = std::shared_ptr<ParticlesData>;
using ParticlesDataWeakPtr = std::weak_ptr<ParticlesData>;

#endif 