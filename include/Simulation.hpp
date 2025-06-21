#ifndef SIMULATION_HPP
#define SIMULATION_HPP
#include "types.hpp"
#include "forward.hpp"
#include "Galaxy.hpp"


struct Simulation {
    scalar_t time_step = 0.01;          // dt
    scalar_t G = 6.67430e-11;           // gravitational constant
    scalar_t softening = 1e-4;          // to avoid singularities in the force calculation
    scalar_t max_velocity = 1e5;        // vitesse maximale pour éviter les instabilités numériques
    scalar_t max_acceleration = 1e5;    // accélération maximale pour éviter les instabilités numériques
    scalar_t theta = 1;                 //parameter to control the accuracy of the simulation, if (size of the octree) / (distance between the particle and the octree) is less than theta, the force is calculated using the octree

    Galaxies galaxies;
    OctreePtr octreeRoot;

    Simulation(Galaxies& galaxies_, OctreePtr& octreeRoot_)
        : galaxies(galaxies_), octreeRoot(octreeRoot_) {}
    
    void run();

    void update();

    void updateParticuleAcceleration(ParticlePtr& particle, const OctreePtr& octreeRoot);
    
    
    
    

};

#endif