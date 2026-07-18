#ifndef GALAXYFACTORY_HPP
#define GALAXYFACTORY_HPP

#include "Galaxy.hpp"
#include "Simulation.hpp"

class GalaxyFactory
{
    private:
        SimulationPtr simulation;
    public:
        GalaxyFactory(SimulationPtr simulation) : simulation(simulation) {}
        // Generate a galaxy with the given number of particles, mass, radius, thickness and starSpeed.
        Galaxy generateGalaxy(size_t nbParticles, scalar_t mass_, scalar_t radius, scalar_t thickness, scalar_t starSpeed = 0);
};

using GalaxyFactoryPtr = std::shared_ptr<GalaxyFactory>;
using GalaxyFactoryWeakPtr = std::weak_ptr<GalaxyFactory>;

#endif