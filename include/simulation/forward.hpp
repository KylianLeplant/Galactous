#ifndef FORWARD_HPP
#define FORWARD_HPP
#include <memory>
#include <vector>
#include <array>

struct Octree;
using OctreePtr = std::shared_ptr<Octree>;
using OctreeWeakPtr = std::weak_ptr<Octree>;
using OctreeBranches = std::array<OctreePtr,8>;

struct Particle;
using ParticlePtr = std::shared_ptr<Particle>;
using ParticleWeakPtr = std::weak_ptr<Particle>;
using Particles = std::vector<ParticlePtr>;

struct Simulation;
using SimulationPtr = std::shared_ptr<Simulation>;

class GalaxyFactory;
using GalaxyFactoryPtr = std::shared_ptr<GalaxyFactory>;
using GalaxyFactoryWeakPtr = std::weak_ptr<GalaxyFactory>;

struct Galaxy;
using GalaxyPtr = std::shared_ptr<Galaxy>;
using GalaxyWeakPtr = std::weak_ptr<Galaxy>;


#endif // FORWARD_HPP