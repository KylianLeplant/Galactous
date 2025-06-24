#include "Simulation.hpp"


void Simulation::update(){
    for (auto& galaxy : galaxies){
        for (auto& particle : galaxy->particles){
            particle->acceleration = Vec3(0, 0, 0);
            updateAcceleration(particle, octreeRoot);
            if (updatePosition(particle)){
            }
        }
    }
}


void Simulation::updateAcceleration(ParticlePtr& particle, const OctreePtr& octreeRoot){
    if (octreeRoot->particle != nullptr){
        particle->acceleration += (octreeRoot->particle->pos - particle->pos) * (G * octreeRoot->particle->mass / (particle->pos - octreeRoot->particle->pos).norm()-softening);
        return;
    }
    if (octreeRoot->branches[0] == nullptr){
        return;
    }
    for (auto& octreeBranch : octreeRoot->branches){
        scalar_t d = (particle->pos - octreeBranch->massCenter).norm() + softening;
        scalar_t s_d = octreeBranch->width / d;
        if (s_d < theta || octreeBranch->particle != nullptr){  // if the octree is a leaf or if the octree is a branch and is quite far from the particle compared to its size, the force is calculated using the octree
            particle->acceleration += (octreeBranch->massCenter - particle->pos).limitNorm(max_acceleration) * (G * octreeBranch->mass / d);  
            continue;
        }
        else updateAcceleration(particle, octreeBranch);
    }
    particle->acceleration = particle->acceleration.limitNorm(max_acceleration);
}

bool Simulation::updatePosition(ParticlePtr& particle){
    particle->velocity += particle->acceleration * time_step/2;
    particle->pos += particle->velocity * time_step;
    particle->velocity += particle->acceleration * time_step/2;
    OctreePtr octree = particle->octree.lock();
    if (octree != nullptr){
        if(!octree->contains(particle->pos)){
            octree->migrateParticleUp(particle);
            return true;
        }
    }
    return false;
}





