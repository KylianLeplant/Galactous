#ifndef SIMULATION_HPP
#define SIMULATION_HPP
#include "types.hpp"
#include "forward.hpp"
#include "Galaxy.hpp"
#include "ComputeShader.hpp"
#include "FlattenedOctree.hpp"
#include "ParticlesData.hpp"
#include <queue>
#include <unordered_set>
#include <thread>
#include <atomic>


struct Simulation {
    scalar_t time_step = 10; //0.01;          // dt
    scalar_t G = 6.67430e-11;           // gravitational constant
    scalar_t softening = 1e-2;          // to avoid singularities in the force calculation
    scalar_t max_velocity = 1e5;        // vitesse maximale pour éviter les instabilités numériques
    scalar_t max_acceleration = 1e5;    // accélération maximale pour éviter les instabilités numériques
    scalar_t theta = 1;                 //parameter to control the accuracy of the simulation, if (size of the octree) / (distance between the particle and the octree) is less than theta, the force is calculated using the octree
    std::queue<ParticlesDataPtr> particlesDataQueue; // stack to store the particles data for each step of the simulation
    std::queue<FlattenedOctreePtr> flattenedOctreeQueue; // stack to store the flattened octree for each step of the simulation
    Galaxies galaxies;
    OctreePtr octreeRoot;
    std::thread threadUpdateParticlesData;
    std::thread threadUpdateAcceleration;
    std::atomic<bool> stopFlag;

    Simulation()
        : galaxies(), octreeRoot(std::make_shared<Octree>(Vec3(0,0,0),1000.0f)), particlesDataQueue(), stopFlag(false) {Octree::root = octreeRoot;std::cout<<"Octree root width : "<<octreeRoot->width<<std::endl;}
    Simulation(scalar_t width)
        : galaxies(), octreeRoot(std::make_shared<Octree>(Vec3(0,0,0),width)), particlesDataQueue(), stopFlag(false) {Octree::root = octreeRoot;}
    Simulation(OctreePtr& octreeRoot_)
        : galaxies(), octreeRoot(octreeRoot_), particlesDataQueue(), stopFlag(false) {Octree::root = octreeRoot;}
    Simulation(Galaxies& galaxies_, OctreePtr& octreeRoot_)
        : galaxies(galaxies_), octreeRoot(octreeRoot_), particlesDataQueue(), stopFlag(false) {Octree::root = octreeRoot;}

    ~Simulation(){stop();}

    void run(bool withGPU=false);

    void stop();

    void update();

    void updateAccelerationWithGPU();

    void updateParticlesData();

    void updateFlattenedOctree();

    void updateAccelerationParticle(ParticlePtr particle, OctreePtr octreeRoot);

    // Function to update the position of a particle, if the particle leave its parent octree, it is migrated up to the parent node, if the particle is in the octree, the octree is updated. Returns true if the particle is in a new octree, false otherwise.
    bool updatePosition(ParticlePtr& particle);
   
    void updateAcceleration();

    void migrateParticleUp(ParticlePtr& particle,OctreePtr& octree);

    void cleanOctree(OctreePtr& octree);

    //std::vector<GPUOctreePtr> getFlattenedOctree(OctreePtr& octree, unsigned int& index);

};

using SimulationPtr = std::shared_ptr<Simulation>;
using SimulationWeakPtr = std::weak_ptr<Simulation>;

#endif