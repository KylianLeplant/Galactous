#ifndef FLATTENEDOCTREE_HPP
#define FLATTENEDOCTREE_HPP

#include <vector>

#include "types.hpp"



struct FlattenedOctree {
    std::vector<Vec3> centers;  // Center of the octree node
    std::vector<float> widths; // Width of the octree node
    std::vector<Vec3> massCenters; // Center of mass of the octree node
    std::vector<float> masses; // Total mass of the octree node
    std::vector<unsigned int> nextSiblingIndices; // Index of the next sibling in the list of SimpleOctree
    std::vector<unsigned int> parentIndices; // Index of the parent in the list of SimpleOctree
    //std::vector<unsigned int> indices; // Index of the octree node in the list of SimpleOctree
    unsigned int size(){return centers.size();}
    
};

using FlattenedOctreePtr = std::shared_ptr<FlattenedOctree>;
using FlattenedOctreeWeakPtr = std::weak_ptr<FlattenedOctree>;


#endif