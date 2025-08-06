#ifndef GPUOCTREE_HPP
#define GPUOCTREE_HPP

#include "types.hpp"

#pragma pack(push, 1) 
struct GPUOctree {
    Vec3 center;  //center of the octree node
    float width; //width of the octree node
    Vec3 massCenter; //center of mass of the octree node
    float mass; //total mass of the octree node
    unsigned int nextSiblingIndex; //index of the next sibling in the list of SimpleOctree
    unsigned int parentIndex; //index of the parent in the list of SimpleOctree
    unsigned int index; //index of the octree node in the list of SimpleOctree

    GPUOctree(const Vec3 center_, float width_)
        : center(center_), width(width_), massCenter(Vec3()), mass(0), nextSiblingIndex(0), parentIndex(0), index(0){}
};
#pragma pack(pop) 


using GPUOctreePtr = std::shared_ptr<GPUOctree>;
using GPUOctreeWeakPtr = std::weak_ptr<GPUOctree>;
#endif