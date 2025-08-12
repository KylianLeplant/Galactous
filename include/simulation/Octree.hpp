#ifndef OCTREE_HPP
#define OCTREE_HPP
#include "types.hpp"
#include <array>
#include "forward.hpp"  // Include forward declarations for Octree and Particle
    #include "Particle.hpp"
#include "FlattenedOctree.hpp"



struct Octree : public std::enable_shared_from_this<Octree>{
    static OctreePtr root;
    Vec3 center;  //center of the octree node
    scalar_t width; //width of the octree node
    Vec3 massCenter; //center of mass of the octree node
    scalar_t mass; //total mass of the octree node
    
        
    OctreeBranches branches;    //children of the octree node
    OctreeWeakPtr parent;       //parent node
    ParticlePtr particle; //particle contained in the octree node, if any

    Octree(const Vec3& center, scalar_t width)
        : center(center),width(width),massCenter(0,0,0),mass(0), branches(), parent(), particle(nullptr) {}

    Octree(const Vec3& center, scalar_t width, OctreeWeakPtr parent)
        : center(center),width(width),massCenter(0,0,0),mass(0),branches(), parent(parent), particle(nullptr) {}


    void migrateParticleUp(ParticlePtr& particle);
    // method to check if a point is inside the octree node   
    bool contains(const Vec3& point) const;

    // method to subdivide the octree node into 8 branches
    void subdivide();

    // method to add a particle to the octree node, if the node is empty, the particle is added to the node, if the node is not empty the node is subdivided and the particle is added to the appropriate branch
    void addParticle(ParticlePtr& particle);

    // method to fill the octree with particles
    void fillOctree(Particles& particles);

    // method to print the octree structure
    void printOctree(int n = 0);

    // method to update the center of mass of the octree node
    void updateMassCenter();

    // method to get the number of particles in the octree node
    size_t size();

    // method to check if all branches are empty
    bool allBranchesEmpty();

    // method to get the number of empty branches
    unsigned int nbEmptyBranches();

    // method to merge all branches
    void mergeBranches();

    // method to get the flattened octree
    void getFlattenedOctree(FlattenedOctreePtr& flattenedOctree, const unsigned int parentIndex = 0);

    // method to delete the particle contained in the octree node
    void deleteParticle();
};

using OctreePtr = std::shared_ptr<Octree>;
using OctreeWeakPtr = std::weak_ptr<Octree>;








#endif
