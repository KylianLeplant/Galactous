#ifndef PARTICLES_DATA_HPP
#define PARTICLES_DATA_HPP

#include <vector>
#include "types.hpp"



struct ParticlesData {
    std::vector<Vec3> positions; // Positions of the particles
    std::vector<float> masses; // Masses of the particles
    std::vector<Vec3> accelerations; // Accelerations of the particles
};

using ParticlesDataPtr = std::shared_ptr<ParticlesData>;
using ParticlesDataWeakPtr = std::weak_ptr<ParticlesData>;

#endif 