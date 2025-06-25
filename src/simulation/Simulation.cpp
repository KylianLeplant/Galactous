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
        scalar_t d = (particle->pos - octreeBranch->massCenter).norm() + softening;
        scalar_t s_d = octreeBranch->width / d;
        if (s_d < theta || octreeBranch->particle != nullptr){  // if the octree is a leaf or if the octree is a branch and is quite far from the particle compared to its size, the force is calculated using the octree
            particle->acceleration += (octreeBranch->massCenter - particle->pos) * (G * octreeBranch->mass / d);  
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
            octree->parent.lock()->migrateParticleUp(particle);
            octree->particle=nullptr;
            return true;
        }
    }
    return false;
}





