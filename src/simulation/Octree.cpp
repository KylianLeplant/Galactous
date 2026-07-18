#include "Octree.hpp"

OctreePtr Octree::root = nullptr;

bool Octree::contains(const Vec3& point) const {
    return (point.x >= center.x - width / 2 && point.x < center.x + width / 2 &&
            point.y >= center.y - width / 2 && point.y < center.y + width / 2 &&
            point.z >= center.z - width / 2 && point.z < center.z + width / 2);
}

void Octree::addParticle(ParticlePtr& newParticle) {
    if (particle == nullptr && branches[0] == nullptr) {
        particle = newParticle;
        particle->octree = shared_from_this();
        return;
    }

    if (branches[0] == nullptr) {
        subdivide();
        ParticlePtr oldParticle = particle;
        particle = nullptr;
        addParticle(oldParticle);
    }

    for (auto& branch : branches) {
        if (branch && branch->contains(newParticle->pos)) {
            branch->addParticle(newParticle);
            return;
        }
    }
}

void Octree::subdivide() {
    scalar_t halfWidth = width / 2.0f;
    scalar_t quarterWidth = halfWidth / 2.0f;

    branches[0] = std::make_shared<Octree>(center + Vec3(-quarterWidth, quarterWidth, quarterWidth), halfWidth, shared_from_this());
    branches[1] = std::make_shared<Octree>(center + Vec3(-quarterWidth, -quarterWidth, quarterWidth), halfWidth, shared_from_this());
    branches[2] = std::make_shared<Octree>(center + Vec3(-quarterWidth, -quarterWidth, -quarterWidth), halfWidth, shared_from_this());
    branches[3] = std::make_shared<Octree>(center + Vec3(quarterWidth, quarterWidth, quarterWidth), halfWidth, shared_from_this());
    branches[4] = std::make_shared<Octree>(center + Vec3(quarterWidth, quarterWidth, -quarterWidth), halfWidth, shared_from_this());
    branches[5] = std::make_shared<Octree>(center + Vec3(quarterWidth, -quarterWidth, quarterWidth), halfWidth, shared_from_this());
    branches[6] = std::make_shared<Octree>(center + Vec3(-quarterWidth, quarterWidth, -quarterWidth), halfWidth, shared_from_this());
    branches[7] = std::make_shared<Octree>(center + Vec3(quarterWidth, -quarterWidth, -quarterWidth), halfWidth, shared_from_this());
}

void Octree::fillOctree(Particles& particles){
    for (auto& particle : particles){
        addParticle(particle);
    }
}

void Octree::printOctree(int n){
    if (particle != nullptr && branches[0] != nullptr) throw std::runtime_error("Octree has both particle and branches");
    std::cout << std::string(3*n, ' ') << "Octree with center " << center
              << " and mass center " << massCenter
              << " with width " << width
              << " and mass " << mass << std::endl;
    if (particle != nullptr){
        std::cout << std::string(3*n, ' ') << " \033[31mParticle\033[0m at position "
                  << particle->pos << " with mass " << particle->mass << std::endl;
    }
    if (branches[0] != nullptr){
        std::cout << std::string(3*n, ' ') << " Branches: " << std::endl;
        for (auto& branch : branches){
            branch->printOctree(n + 1);
        }
    }
    std::cout << std::string(3*n, ' ') << "End of octree" << std::endl;
}

void Octree::updateMassCenter(){
    mass = 0;
    massCenter = Vec3(0, 0, 0);
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
    else mergeBranches();
}

size_t Octree::size(){
    if (particle != nullptr) return 1;
    size_t s = 0;
    if (branches[0] != nullptr){
        for (auto& branch : branches){
            s += branch->size();
        }
    }
    return s;
}

bool Octree::allBranchesEmpty(){
    if (particle != nullptr) return false;
    for (auto& branch : branches){
        if (branch->particle != nullptr || branch->branches[0] != nullptr) return false;
    }
    return true;
}

unsigned int Octree::nbEmptyBranches(){
    unsigned int n = 0;
    for (auto& branch : branches){
        if (branch->mass == 0) n++;
    }
    return n;
}

void Octree::mergeBranches(){
    for (auto& branch : branches){
        branch = nullptr;
    }
}

void Octree::getFlattenedOctree(FlattenedOctreePtr& flattenedOctree, const unsigned int parentIndex) {
    if (mass == 0){
        return;
    }
    flattenedOctree->centers.push_back(center);
    flattenedOctree->widths.push_back(width);
    flattenedOctree->massCenters.push_back(massCenter);
    flattenedOctree->masses.push_back(mass);
    flattenedOctree->nextSiblingIndices.push_back(0);
    flattenedOctree->parentIndices.push_back(parentIndex);

    unsigned int index = flattenedOctree->size() - 1;
    std::vector<unsigned> tabIndex;
    for (auto& branch : branches){
        if (branch == nullptr) break;
        if (branch->mass != 0){
            tabIndex.push_back(flattenedOctree->size());
            branch->getFlattenedOctree(flattenedOctree, index);
        }
    }

    for (size_t i = 0; i + 1 < tabIndex.size(); i++){
        unsigned int childIndex = tabIndex[i];
        unsigned int nextIndex = tabIndex[i+1];
        flattenedOctree->nextSiblingIndices[childIndex] = nextIndex;
    }
}

void Octree::deleteParticle() {
    particle.reset();
}