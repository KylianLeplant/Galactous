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
using SimulationWeakPtr = std::weak_ptr<Simulation>;

struct Galaxy;
using GalaxyPtr = std::shared_ptr<Galaxy>;
using GalaxyWeakPtr = std::weak_ptr<Galaxy>;
using Galaxies = std::vector<GalaxyPtr>;

struct GalaxyFactory;
using GalaxyFactoryPtr = std::shared_ptr<GalaxyFactory>;
using GalaxyFactoryWeakPtr = std::weak_ptr<GalaxyFactory>;

// Forward declarations pour éviter les inclusions circulaires
class Window;
class Camera;
using WindowPtr = std::shared_ptr<Window>;
using WindowWeakPtr = std::weak_ptr<Window>;
using CameraPtr = std::shared_ptr<Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;

struct Renderer;
using RendererPtr = std::shared_ptr<Renderer>;
using RendererWeakPtr = std::weak_ptr<Renderer>;

struct Input;
using InputPtr = std::shared_ptr<Input>;
using InputWeakPtr = std::weak_ptr<Input>;

struct Camera;
using CameraPtr = std::shared_ptr<Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;
using Cameras = std::vector<CameraPtr>;

#endif // FORWARD_HPP