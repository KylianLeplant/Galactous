#ifndef OCTREE_HPP
#define OCTREE_HPP
#include "types.hpp"
#include <array>
#include "forward.hpp"  // Include forward declarations for Octree and Particle
#include "Particle.hpp"


struct Octree : public std::enable_shared_from_this<Octree>{
    Vec3 center;           //position vector of the center of the octree node
    scalar_t width;      //width of the octree node
    Vec3 massCenter;        //center of mass of the octree node
    scalar_t mass;          //total mass of the octree node
    OctreeBranches branches;    //children of the octree node
    OctreeWeakPtr parent;       //parent node
    ParticlePtr particle; //particle contained in the octree node, if any

    Octree(const Vec3& center, scalar_t width)
        : center(center), width(width), massCenter(center), mass(0), branches(), parent(), particle(nullptr) {}

    Octree(const Vec3& center, scalar_t width, OctreeWeakPtr parent)
        : center(center), width(width), massCenter(center), mass(0), branches(), parent(parent), particle(nullptr) {}

    // Function to check if a point is inside the octree node   
    bool contains(const Vec3& point) const;

    // Function to subdivide the octree node into 8 branches
    void subdivide();

    // Function to add a particle to the octree node, if the node is empty, the particle is added to the node, if the node is not empty the node is subdivided and the particle is added to the appropriate branch
    void addParticle(ParticlePtr& particle);

    // Function to migrate a particle up to the parent node, if particle is not in the area of the node, this function is called recursively until the particle is added in a node
    void migrateParticleUp(ParticlePtr& particle);

    void fillOctree(Particles& particles);

    void printOctree(int n = 0);

    void updateMassCenter();

    size_t size(){
        if (particle != nullptr) return 1;
        size_t s = 0;
        if (branches[0] != nullptr){
            for (auto& branch : branches){
                s += branch->size();
            }
        }
        return s;
    }

};








#endif
