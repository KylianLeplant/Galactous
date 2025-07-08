#include "Simulation.hpp"
#include <algorithm>


void Simulation::update(){
    for (auto& galaxy : galaxies){
        for (auto& particle : galaxy->particles){
            particle->acceleration = Vec3(0, 0, 0);
            updateAcceleration(particle, octreeRoot);
        }
    }
    for (auto& galaxy : galaxies) {
        std::vector<ParticlePtr> to_remove;
        for (auto& particle : galaxy->particles) {
            if (!updatePosition(particle)) {
                to_remove.push_back(particle);
            }
        }
    }
    octreeRoot->updateMassCenter();
    flattenedOctree.clear();
    unsigned int index = 0;
    auto octreeFlat = octreeRoot->getFlattenedOctree(index);
    //std::cout << "octreeRoot"<<std::endl;
    //octreeRoot->printOctree();
    //std::cout << "\n\n\noctreeFlat" << std::endl;
    //for (auto& octree : octreeFlat){
    //    std::cout<< "index: " << octree->index << " mass: " << octree->mass << " center: " << octree->center << " parentIndex: " << octree->parentIndex << " nextSiblingIndex: " << octree->nextSiblingIndex << std::endl;
    //}
    flattenedOctree.insert(flattenedOctree.end(), octreeFlat.begin(), octreeFlat.end());
}


void Simulation::updateAcceleration(ParticlePtr& particle, const OctreePtr& octreeRoot){
    if (octreeRoot->particle != nullptr){
        Vec3 r = octreeRoot->particle->pos - particle->pos;
        particle->acceleration += r * (G * octreeRoot->particle->mass / r.norm()-softening);
        return;
    }
    if (octreeRoot->branches[0] == nullptr){
        return;
    }
    for (auto& octreeBranch : octreeRoot->branches){
        scalar_t d = (particle->pos - octreeBranch->gpuOctree->massCenter).norm() + softening;
        scalar_t s_d = octreeBranch->gpuOctree->width / d;
        if (s_d < theta || octreeBranch->particle != nullptr){  // if the octree is a leaf or if the octree is a branch and is quite far from the particle compared to its size, the force is calculated using the octree
            particle->acceleration += (octreeBranch->gpuOctree->massCenter - particle->pos) * (G * octreeBranch->gpuOctree->mass / d);  
            continue;
        }
        else updateAcceleration(particle, octreeBranch);
    }
    //particle->acceleration = particle->acceleration.limitNorm(max_acceleration);
}

bool Simulation::updatePosition(ParticlePtr& particle){
    particle->velocity += particle->acceleration * time_step/2;
    particle->pos += particle->velocity * time_step;
    particle->velocity += particle->acceleration * time_step/2;
    OctreePtr octree = particle->octree.lock(); 
    if (octree != nullptr){
        if(!octree->contains(particle->pos)){
            OctreePtr parentLocked = octree->parent.lock();
            if (parentLocked != nullptr){
                octree->particle=nullptr;
                migrateParticleUp(particle,parentLocked);
                return true;
            }
        }
    }
    return false;
}

void Simulation::migrateParticleUp(ParticlePtr& particle,OctreePtr& octree){
    if (octree->contains(particle->pos)){
        octree->addParticle(particle);
    }
    else{   
        OctreePtr parentLocked = octree->parent.lock();

        if (parentLocked != nullptr){
            migrateParticleUp(particle,parentLocked);
        }
        else{
            std::cout << "particle is not in the octree" << std::endl;
        }
    }
}
// Function to clean the octree, if the octree is empty and has no parent, the octree is deleted
void Simulation::cleanOctree(OctreePtr& octree){
    if (octree->branches[0] != nullptr && octree->size() == 0){
        for (auto& branch : octree->branches){
            branch = nullptr;
        }
    }
} 

//std::vector<GPUOctreePtr> Simulation::getFlattenedOctree(OctreePtr& octree, unsigned int& index){
//    std::vector<GPUOctreePtr> flattenedOctree;
//    if (octree->gpuOctree->mass == 0){
//        return flattenedOctree;
//    }
//    else{
//        flattenedOctree.push_back(std::make_shared<GPUOctree>(octree->gpuOctree));
//        octree->gpuOctree->index = index;
//        index++;          
//        for (auto& branch : octree->branches){
//            if (branch == nullptr) break;
//            if (branch->gpuOctree->mass != 0){
//                auto branchFlattened = getFlattenedOctree(branch, index);
//                flattenedOctree.insert(flattenedOctree.end(), branchFlattened.begin(), branchFlattened.end());
//            }
//        }
//        GPUOctreePtr branchGPUOctree = nullptr;
//        for (auto& branch : octree->branches){
//            if (branch == nullptr) break;
//            if (branchGPUOctree == nullptr && branch->gpuOctree->mass != 0) branchGPUOctree = std::make_shared<GPUOctree>(branch->gpuOctree);
//            else if (branch->gpuOctree->mass != 0){
//                branchGPUOctree->nextSiblingIndex = branch->gpuOctree->index;
//                branchGPUOctree = std::make_shared<GPUOctree>(branch->gpuOctree);
//            } 
//        }
//        return flattenedOctree;
//
//    }
//}


