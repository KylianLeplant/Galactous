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
#include <memory>


struct Simulation {
    scalar_t time_step = 10; //0.01;    // dt
    scalar_t G = 6.67430e-11;           // gravitational constant
    scalar_t softening = 1e-2;          // to avoid singularities in the force calculation
    scalar_t theta = 1;                 // parameter to control the accuracy of the simulation, if (size of the octree) / (distance between the particle and the octree) is less than theta, the force is calculated using the octree
    std::atomic<ParticlesDataPtr> currentParticlesData; // atomic shared_ptr for thread-safe access without mutex
    std::atomic<FlattenedOctreePtr> currentFlattenedOctree; // atomic shared_ptr for thread-safe access without mutex
    Galaxies galaxies;                  // galaxies of the simulation
    OctreePtr octreeRoot;               // main node of the simulation
    std::thread threadUpdateParticlesData;  // thread filling the particlesDataQueue in GPU mode
    std::thread threadUpdate;           // thread updating particles
    std::atomic<bool> stopFlag;         // flag to stop the simulation

    // constructors
    Simulation()
        : galaxies(), octreeRoot(std::make_shared<Octree>(Vec3(0,0,0),1000.0f)), currentParticlesData(nullptr), currentFlattenedOctree(nullptr), stopFlag(false) {Octree::root = octreeRoot;std::cout<<"Octree root width : "<<octreeRoot->width<<std::endl;}
    Simulation(scalar_t width)
        : galaxies(), octreeRoot(std::make_shared<Octree>(Vec3(0,0,0),width)), currentParticlesData(nullptr), currentFlattenedOctree(nullptr), stopFlag(false) {Octree::root = octreeRoot;}
    Simulation(OctreePtr& octreeRoot_)
        : galaxies(), octreeRoot(octreeRoot_), currentParticlesData(nullptr), currentFlattenedOctree(nullptr), stopFlag(false) {Octree::root = octreeRoot;}
    Simulation(Galaxies& galaxies_, OctreePtr& octreeRoot_)
        : galaxies(galaxies_), octreeRoot(octreeRoot_), currentParticlesData(nullptr), currentFlattenedOctree(nullptr), stopFlag(false) {Octree::root = octreeRoot;}
    
        //destructor
    ~Simulation(){stop();}


    //methods

    // method to run the simulation 
    // if withGpu is true the simulation use the GPU
    void run(bool withGPU=false);

    // method to stop the simulation
    void stop();

    // method to update the volucity and the position of particles without gpu
    void update();

    // method to update the simulation with gpu
    void updateWithGPU();

    // method to update the particles data in the particlesDataQueue
    void updateParticlesData();

    // method to update the flattenedOctree in the flattenedOctreeQueue
    void updateFlattenedOctree();

    // method to update the acceleration of all particles in the octree
    void updateAccelerationParticle(ParticlePtr particle, OctreePtr octreeRoot);

    // method to update the position of a particle (without GPU), if the particle leave its parent octree, it is migrated up to the parent node, if the particle is in the octree, the octree is updated. Returns true if the particle is in a new octree, false otherwise.
    bool updatePosition(ParticlePtr& particle);
   
    // method updating the acceleration of particles in no-GPU mode
    void updateAcceleration();

    // method moving a particle in a octree
    void migrateParticleUp(ParticlePtr& particle,OctreePtr& octree);

};

using SimulationPtr = std::shared_ptr<Simulation>;
using SimulationWeakPtr = std::weak_ptr<Simulation>;

#endif