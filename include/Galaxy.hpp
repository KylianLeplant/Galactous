#ifndef GALAXY_HPP
#define GALAXY_HPP
#include "Particle.hpp"
#include <stdlib.h>
#include <iostream>
#include <time.h>

struct Galaxy{
    Vec3 massCenter; //center of mass of the galaxy
    Particles particles; //list of particles in the galaxy
    scalar_t mass;  //total mass of the galaxy

    Galaxy(size_t nbParticules, scalar_t mass_,scalar_t radius);

    void printParticles();
};

using GalaxyPtr = std::shared_ptr<Galaxy>;
using GalaxyWeakPtr = std::weak_ptr<Galaxy>;
using Galaxies = std::vector<GalaxyPtr>;

#endif