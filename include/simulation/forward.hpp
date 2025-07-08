#ifndef FORWARD_HPP
#define FORWARD_HPP
#include <memory>
#include <vector>
#include <array>

struct GPUOctree;
struct Octree;
using OctreePtr = std::shared_ptr<Octree>;
using OctreeWeakPtr = std::weak_ptr<Octree>;
using OctreeBranches = std::array<OctreePtr,8>;
using GPUOctreePtr = std::shared_ptr<GPUOctree>;
using GPUOctreeWeakPtr = std::weak_ptr<GPUOctree>;

struct Particle;
using ParticlePtr = std::shared_ptr<Particle>;
using ParticleWeakPtr = std::weak_ptr<Particle>;
using Particles = std::vector<ParticlePtr>;

// Forward declarations pour éviter les inclusions circulaires
class Window;
class Camera;
using WindowPtr = std::shared_ptr<Window>;
using WindowWeakPtr = std::weak_ptr<Window>;
using CameraPtr = std::shared_ptr<Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;

#endif // FORWARD_HPP