#ifndef GPUOCTREE_HPP
#define GPUOCTREE_HPP

#include "types.hpp"



struct GPUOctree{
    Vec3 center;  //center of the octree node
    scalar_t width; //width of the octree node
    Vec3 massCenter; //center of mass of the octree node
    scalar_t mass; //total mass of the octree node
    unsigned int nextSiblingIndex; //index of the next sibling in the list of SimpleOctree
    unsigned int parentIndex; //index of the parent in the list of SimpleOctree
    unsigned int index; //index of the octree node in the list of SimpleOctree
    GPUOctree(const Vec3& center, scalar_t width) : center(center), width(width), massCenter(Vec3(0,0,0)), mass(0), nextSiblingIndex(0), parentIndex(0), index(0) {}
};

#endif