#include "Simulation.hpp"


void Simulation::update(){
    for (auto& galaxy : galaxies){
        for (auto& particle : galaxy->particules){
            particle->acceleration = Vec3(0, 0, 0);
            
        }
    }
}


void Simulation::updateParticuleAcceleration(ParticlePtr& particle, const OctreePtr& octreeRoot){
    for (auto& octreeBranch : octreeRoot->branches){
        scalar_t d = (particle->pos - octreeBranch->massCenter).norm() + softening;
        scalar_t s_d = octreeBranch->width / d;
        if (s_d < theta || octreeBranch->particle != nullptr){  // if the octree is a leaf or if the octree is a branch and is quite far from the particle compared to its size, the force is calculated using the octree
            particle->acceleration += (G * octreeBranch->mass / d * (octreeBranch->massCenter - particle->pos)).limitNorm(max_acceleration);
            continue;
        }
        else updateParticuleAcceleration(particle, octreeBranch);
    }
}