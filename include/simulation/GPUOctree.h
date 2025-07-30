#ifndef GPUOCTREE_HPP
#define GPUOCTREE_HPP

#include "types.hpp"



struct GPUOctree {
    float center[3];  //center of the octree node
    scalar_t width; //width of the octree node
    float massCenter[3]; //center of mass of the octree node
    float mass; //total mass of the octree node
    unsigned int nextSiblingIndex; //index of the next sibling in the list of SimpleOctree
    unsigned int parentIndex; //index of the parent in the list of SimpleOctree
    unsigned int index; //index of the octree node in the list of SimpleOctree

    GPUOctree(const float* center_, float width_)
        : width(width_), mass(0), nextSiblingIndex(0), parentIndex(0), index(0)
    {
        for (int i = 0; i < 3; ++i) center[i] = center_[i];
        for (int i = 0; i < 3; ++i) massCenter[i] = 0.0f;
    }
};


using GPUOctreePtr = std::shared_ptr<GPUOctree>;
using GPUOctreeWeakPtr = std::weak_ptr<GPUOctree>;
#endif