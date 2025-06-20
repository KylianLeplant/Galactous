#ifndef PARTICLE_HPP
#define PARTICLE_HPP
#include "types.hpp"
#include <memory>
#include <vector>
#include "forward.hpp"  
#include "Octree.hpp"



enum class TypeParticle {
    ETOILE,
    MATIERE_NOIRE,
    GAZ
};

struct Particle{
    static id_t nextId; //static variable to generate unique ids for each particule
    id_t id;            //unique id of the particule
    scalar_t mass;          
    Vec3 pos;           //position vector
    Vec3 velocity;      //velocity vector
    Vec3 acceleration;  //acceleration vector
    OctreeWeakPtr octree;   //parent node


    TypeParticle type; //type of the particule

    Particle(scalar_t mass, const Vec3& pos, const Vec3& velocity, TypeParticle type, OctreeWeakPtr octree = OctreeWeakPtr())
        : id(nextId), mass(mass), pos(pos), velocity(velocity), acceleration(Vec3()), octree(octree), type(type) {nextId++;}
    
    bool operator==(const Particle& other) const { return id == other.id; }

};

using ParticlePtr = std::shared_ptr<Particle>;

using ParticleWeakPtr = std::weak_ptr<Particle>;

using Particles = std::vector<ParticlePtr>;


#endif