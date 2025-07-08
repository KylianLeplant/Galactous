#ifndef OCTREE_HPP
#define OCTREE_HPP
#include "types.hpp"
#include <array>
#include "forward.hpp"  // Include forward declarations for Octree and Particle
#include "Particle.hpp"
#include "GPUOctree.h"


//struct GPUOctree{
//    Vec3 center;  //center of the octree node
//    scalar_t width; //width of the octree node
//    Vec3 massCenter; //center of mass of the octree node
//    scalar_t mass; //total mass of the octree node
//    unsigned int nextSiblingIndex; //index of the next sibling in the list of SimpleOctree
//    unsigned int parentIndex; //index of the parent in the list of SimpleOctree
//    unsigned int index; //index of the octree node in the list of SimpleOctree
//    GPUOctree(const Vec3& center, scalar_t width) : center(center), width(width), massCenter(Vec3(0,0,0)), mass(0), nextSiblingIndex(0), parentIndex(0), index(0) {}
//};

struct Octree : public std::enable_shared_from_this<Octree>{
    static OctreePtr root;
    GPUOctreePtr gpuOctree;
    //Vec3 center;  //center of the octree node
    //scalar_t width; //width of the octree node
    //Vec3 massCenter; //center of mass of the octree node
    //scalar_t mass; //total mass of the octree node
    
        
    OctreeBranches branches;    //children of the octree node
    OctreeWeakPtr parent;       //parent node
    ParticlePtr particle; //particle contained in the octree node, if any

    Octree(const Vec3& center, scalar_t width)
        : gpuOctree(std::make_shared<GPUOctree>(center, width)), branches(), parent(), particle(nullptr) {}

    Octree(const Vec3& center, scalar_t width, OctreeWeakPtr parent)
        : gpuOctree(std::make_shared<GPUOctree>(center, width)), branches(), parent(parent), particle(nullptr) {}


    void migrateParticleUp(ParticlePtr& particle);
    // Function to check if a point is inside the octree node   
    bool contains(const Vec3& point) const;

    // Function to subdivide the octree node into 8 branches
    void subdivide();

    // Function to add a particle to the octree node, if the node is empty, the particle is added to the node, if the node is not empty the node is subdivided and the particle is added to the appropriate branch
    void addParticle(ParticlePtr& particle);

    // Function to migrate a particle up to the parent node, if particle is not in the area of the node, this function is called recursively until the particle is added in a node
    //void migrateParticleUp(ParticlePtr& particle);

    void fillOctree(Particles& particles);

    void printOctree(int n = 0);

    void updateMassCenter();

    size_t size();

    bool allBranchesEmpty();

    unsigned int nbEmptyBranches();

    void mergeBranches();

    std::vector<GPUOctreePtr> getFlattenedOctree(unsigned int& index);
};

using OctreePtr = std::shared_ptr<Octree>;
using OctreeWeakPtr = std::weak_ptr<Octree>;

using GPUOctreePtr = std::shared_ptr<GPUOctree>;
using GPUOctreeWeakPtr = std::weak_ptr<GPUOctree>;








#endif
