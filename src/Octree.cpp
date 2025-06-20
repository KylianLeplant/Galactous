// Update the include path if Octree.hpp is in an 'include' folder relative to your project root
#include "Octree.hpp"
bool Octree::contains(const Vec3& point) const {
        return (point.x >= pos.x && point.x < pos.x + size &&
                point.y >= pos.y && point.y < pos.y + size &&
                point.z >= pos.z && point.z < pos.z + size);
}

void Octree::addParticle(const ParticlePtr& particle) {
        if (this->particle == nullptr && this->branches[0] == nullptr) {
            this->particle = particle;
            mass += particle->mass;
            massCenter = (massCenter * (mass - particle->mass) + particle->pos * particle->mass) / mass;
            return;
        } else {
            // If the node already contains a particle, we need to subdivide
            // and distribute the particles among the branches
            if (branches[0] == nullptr) {
                subdivide();
            }
            for (auto& branch : branches) {
                if (branch && branch->contains(particle->pos)) {
                    branch->addParticle(particle);
                    return;
                }
            }
        }
    }