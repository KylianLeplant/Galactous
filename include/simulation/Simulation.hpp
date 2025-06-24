#ifndef SIMULATION_HPP
#define SIMULATION_HPP
#include "types.hpp"
#include "forward.hpp"
#include "Galaxy.hpp"


struct Simulation {
    scalar_t time_step = 1000; //0.01;          // dt
    scalar_t G = 6.67430e-11;           // gravitational constant
    scalar_t softening = 1e-4;          // to avoid singularities in the force calculation
    scalar_t max_velocity = 1e5;        // vitesse maximale pour éviter les instabilités numériques
    scalar_t max_acceleration = 1e5;    // accélération maximale pour éviter les instabilités numériques
    scalar_t theta = 1;                 //parameter to control the accuracy of the simulation, if (size of the octree) / (distance between the particle and the octree) is less than theta, the force is calculated using the octree

    Galaxies galaxies;
    OctreePtr octreeRoot;

    Simulation()
        : galaxies(), octreeRoot(std::make_shared<Octree>(Vec3(0,0,0),1000.0f)) {}
    Simulation(scalar_t width)
        : galaxies(), octreeRoot(std::make_shared<Octree>(Vec3(0,0,0),width)) {}
    Simulation(OctreePtr& octreeRoot_)
        : galaxies(), octreeRoot(octreeRoot_) {}
    Simulation(Galaxies& galaxies_, OctreePtr& octreeRoot_)
        : galaxies(galaxies_), octreeRoot(octreeRoot_) {}
    
    void run();

    void update();

    // Function to update the acceleration of a particle, if the particle is in the octree, the acceleration is calculated using the octree, if the particle is not in the octree, the acceleration is calculated using the particles in the octree.
    void updateAcceleration(ParticlePtr& particle, const OctreePtr& octreeRoot);

    // Function to update the position of a particle, if the particle leave its parent octree, it is migrated up to the parent node, if the particle is in the octree, the octree is updated. Returns true if the particle is in a new octree, false otherwise.
    bool updatePosition(ParticlePtr& particle);
   

};

using SimulationPtr = std::shared_ptr<Simulation>;
using SimulationWeakPtr = std::weak_ptr<Simulation>;

#endif