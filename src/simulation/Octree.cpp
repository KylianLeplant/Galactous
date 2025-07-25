// Update the include path if Octree.hpp is in an 'include' folder relative to your project root
#include "Octree.hpp"


bool Octree::contains(const Vec3& point) const {
        return (point.x >= center.x - width /2 && point.x < center.x + width / 2 &&
                point.y >= center.y - width /2 && point.y < center.y + width / 2 &&
                point.z >= center.z - width /2 && point.z < center.z + width / 2);
}


///////////////////////
////////////////////
//MANQUE UPDATE MASSCENTER
/////////////////////////
///////////////////////////
void Octree::addParticle(ParticlePtr& newParticle) {
    if (particle == nullptr && branches[0] == nullptr) {    //if the octree is a leaf and is empty, we add the particle to the octree
        particle = newParticle;
        particle->octree = shared_from_this();
        return;
    } else {
        // If the node already contains a particle, we need to subdivide
        // and distribute the particles among the branches
        if (branches[0] == nullptr) {
            subdivide();
            ParticlePtr oldParticle = particle;
            particle = nullptr;
            addParticle(oldParticle);
        }
        //find the appropriate branch to add the particle to
        for (auto& branch : branches) {
            if (branch && branch->contains(newParticle->pos)) {
                branch->addParticle(newParticle);
                return;
            }
        }
    }
}

void Octree::subdivide() {
    scalar_t halfWidth = width / 2.0f;
    scalar_t quarterWidth = halfWidth / 2.0f;

    // Create 8 branches
    branches[0] = std::make_shared<Octree>(center + Vec3(-quarterWidth,quarterWidth,quarterWidth), halfWidth, shared_from_this());
    branches[1] = std::make_shared<Octree>(center + Vec3(-quarterWidth,-quarterWidth,quarterWidth), halfWidth, shared_from_this());
    branches[2] = std::make_shared<Octree>(center + Vec3(-quarterWidth,-quarterWidth,-quarterWidth), halfWidth, shared_from_this());
    branches[3] = std::make_shared<Octree>(center + Vec3(quarterWidth,quarterWidth,quarterWidth), halfWidth, shared_from_this());
    branches[4] = std::make_shared<Octree>(center + Vec3(quarterWidth,quarterWidth,-quarterWidth), halfWidth, shared_from_this());
    branches[5] = std::make_shared<Octree>(center + Vec3(quarterWidth,-quarterWidth,quarterWidth), halfWidth, shared_from_this());
    branches[6] = std::make_shared<Octree>(center + Vec3(-quarterWidth,quarterWidth,-quarterWidth), halfWidth, shared_from_this());
    branches[7] = std::make_shared<Octree>(center + Vec3(quarterWidth,-quarterWidth,-quarterWidth), halfWidth, shared_from_this());
}

void Octree::migrateParticleUp(ParticlePtr& particleToMigrate) {
    if (contains(particleToMigrate->pos)){
        addParticle(particleToMigrate);
    }
    else{
        OctreePtr parentLocked = parent.lock();
        if (parentLocked != nullptr){
            parentLocked->migrateParticleUp(particleToMigrate);
        }
    }
}

void Octree::fillOctree(Particles& particles){
    for (auto& particle : particles){
        addParticle(particle);
    }
}

void Octree::printOctree(int n){
    if (particle != nullptr && branches[0] != nullptr) throw std::runtime_error("Octree has both particle and branches");
    std::cout << std::string(3*n, ' ') << "Octree with  center " << center <<" and mass center " << massCenter << " with width " << width << " and mass " << mass << std::endl;
    if (particle != nullptr){
        std::cout << std::string(3*n, ' ') << " \033[31mParticle\033[0m at position " << particle->pos << " with mass " << particle->mass << std::endl;
    }
    if (branches[0] != nullptr){
        std::cout << std::string(3*n, ' ') << " Branches: " << std::endl;
        for (auto& branch : branches){
            branch->printOctree(n + 1);
        }
    }
    else return;
    std::cout << std::string(3*n, ' ') << "End of octree" << std::endl;
}

void Octree::updateMassCenter(){
    mass = 0;
    massCenter = Vec3(0,0,0);
    if (particle != nullptr){
        mass = particle->mass;
        massCenter = particle->pos;
        return;
    }
    if (branches[0] != nullptr){
        for (auto& branch : branches){
            branch->updateMassCenter();
            mass += branch->mass;
            massCenter += branch->massCenter * branch->mass;
        }
    }
    if (mass != 0) massCenter = massCenter / mass;
}

