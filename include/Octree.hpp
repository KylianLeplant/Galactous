#ifndef OCTREE_HPP
#define OCTREE_HPP
#include "types.hpp"
#include <array>
#include "forward.hpp"  // Include forward declarations for Octree and Particle
#include "Particle.hpp"


struct Octree : public std::enable_shared_from_this<Octree>{
    Vec3 pos;           //position vector of the octree node
    scalar_t size;      //size of the octree node
    Vec3 massCenter;        //center of mass of the octree node
    scalar_t mass;          //total mass of the octree node
    OctreeBranches branches;    //children of the octree node
    OctreeWeakPtr parent;       //parent node
    ParticlePtr particle; //particle contained in the octree node, if any

    Octree(const Vec3& pos, scalar_t size)
        : pos(pos), size(size), massCenter(pos), mass(0), branches(), parent(), particle(nullptr) {}

    Octree(const Vec3& pos, scalar_t size, OctreeWeakPtr parent)
        : pos(pos), size(size), massCenter(pos), mass(0), branches(), parent(parent), particle(nullptr) {}

    // Function to check if a point is inside the octree node   
    bool contains(const Vec3& point) const;

    // Function to subdivide the octree node into 8 branches
    void subdivide() {
        scalar_t halfSize = size / 2.0f;
        Vec3 newPos = pos;

        // Create 8 branches
        branches[0] = std::make_shared<Octree>(newPos, halfSize, shared_from_this());
        branches[1] = std::make_shared<Octree>(newPos + Vec3(halfSize, 0, 0), halfSize, shared_from_this());
        branches[2] = std::make_shared<Octree>(newPos + Vec3(0, halfSize, 0), halfSize, shared_from_this());
        branches[3] = std::make_shared<Octree>(newPos + Vec3(halfSize, halfSize, 0), halfSize, shared_from_this());
        branches[4] = std::make_shared<Octree>(newPos + Vec3(0, 0, halfSize), halfSize, shared_from_this());
        branches[5] = std::make_shared<Octree>(newPos + Vec3(halfSize, 0, halfSize), halfSize, shared_from_this());
        branches[6] = std::make_shared<Octree>(newPos + Vec3(0, halfSize, halfSize), halfSize, shared_from_this());
        branches[7] = std::make_shared<Octree>(newPos + Vec3(halfSize, halfSize, halfSize), halfSize, shared_from_this());
    }

    void addParticle(const ParticlePtr& particle);

};








#endif
