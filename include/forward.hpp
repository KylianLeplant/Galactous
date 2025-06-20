#ifndef FORWARD_HPP
#define FORWARD_HPP
#include <memory>
#include <vector>

struct Octree;
using OctreePtr = std::shared_ptr<Octree>;
using OctreeWeakPtr = std::weak_ptr<Octree>;
using OctreeBranches = std::array<OctreePtr,8>;

struct Particle;
using ParticlePtr = std::shared_ptr<Particle>;
using ParticleWeakPtr = std::weak_ptr<Particle>;
using Particles = std::vector<ParticlePtr>;



#endif // FORWARD_HPP