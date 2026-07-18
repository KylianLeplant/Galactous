#ifndef SIMULATION_HPP
#define SIMULATION_HPP
#include "types.hpp"
#include "forward.hpp"
#include "Galaxy.hpp"
#include "ComputeShader.hpp"
#include "FlattenedOctree.hpp"
#include "ParticlesData.hpp"
#include <thread>
#include <atomic>
#include <memory>


struct Simulation {
    scalar_t time_step = 10;             // dt
    scalar_t G = 6.67430e-11;           // gravitational constant
    scalar_t softening = 1e-2;           // to avoid singularities in the force calculation
    scalar_t theta = 1;                  // Barnes-Hut opening criterion: if width/distance < theta, approximate the node
    std::atomic<ParticlesDataPtr> currentParticlesData;       // atomic shared_ptr for thread-safe access
    std::atomic<FlattenedOctreePtr> currentFlattenedOctree;   // atomic shared_ptr for thread-safe access
    Galaxies galaxies;
    OctreePtr octreeRoot;
    std::thread threadUpdateParticlesData;
    std::thread threadUpdate;
    std::atomic<bool> stopFlag;

    Simulation()
        : galaxies(), octreeRoot(std::make_shared<Octree>(Vec3(0,0,0),1000.0f)),
          currentParticlesData(nullptr), currentFlattenedOctree(nullptr), stopFlag(false) {
        Octree::root = octreeRoot;
    }
    Simulation(scalar_t width)
        : galaxies(), octreeRoot(std::make_shared<Octree>(Vec3(0,0,0),width)),
          currentParticlesData(nullptr), currentFlattenedOctree(nullptr), stopFlag(false) {
        Octree::root = octreeRoot;
    }
    Simulation(OctreePtr& octreeRoot_)
        : galaxies(), octreeRoot(octreeRoot_),
          currentParticlesData(nullptr), currentFlattenedOctree(nullptr), stopFlag(false) {
        Octree::root = octreeRoot;
    }
    Simulation(Galaxies& galaxies_, OctreePtr& octreeRoot_)
        : galaxies(galaxies_), octreeRoot(octreeRoot_),
          currentParticlesData(nullptr), currentFlattenedOctree(nullptr), stopFlag(false) {
        Octree::root = octreeRoot;
    }

    ~Simulation(){stop();}

    // Run the simulation. If withGPU is true, uses OpenCL.
    void run(bool withGPU=false);
    void stop();

    // CPU update loop (velocity + position)
    void update();
    // GPU update loop
    void updateWithGPU();
    // Fills currentParticlesData from the galaxies (called once at startup)
    void updateParticlesData();
    // Continuously refreshes currentFlattenedOctree from octreeRoot
    void updateFlattenedOctree();

    void updateAccelerationParticle(ParticlePtr particle, OctreePtr octreeRoot);
    bool updatePosition(ParticlePtr& particle);
    void updateAcceleration();
    void migrateParticleUp(ParticlePtr& particle, OctreePtr& octree);
};

using SimulationPtr = std::shared_ptr<Simulation>;
using SimulationWeakPtr = std::weak_ptr<Simulation>;

#endif