// Update the include path if Octree.hpp is in an 'include' folder relative to your project root
#include "Octree.hpp"

OctreePtr Octree::root = nullptr;

bool Octree::contains(const Vec3& point) const {
        return (point.x >= gpuOctree->center.x - gpuOctree->width /2 && point.x < gpuOctree->center.x + gpuOctree->width / 2 &&
                point.y >= gpuOctree->center.y - gpuOctree->width /2 && point.y < gpuOctree->center.y + gpuOctree->width / 2 &&
                point.z >= gpuOctree->center.z - gpuOctree->width /2 && point.z < gpuOctree->center.z + gpuOctree->width / 2);
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
    scalar_t halfWidth = gpuOctree->width / 2.0f;
    scalar_t quarterWidth = halfWidth / 2.0f;

    // Create 8 branches
    branches[0] = std::make_shared<Octree>(gpuOctree->center + Vec3(-quarterWidth,quarterWidth,quarterWidth), halfWidth, shared_from_this());
    branches[1] = std::make_shared<Octree>(gpuOctree->center + Vec3(-quarterWidth,-quarterWidth,quarterWidth), halfWidth, shared_from_this());
    branches[2] = std::make_shared<Octree>(gpuOctree->center + Vec3(-quarterWidth,-quarterWidth,-quarterWidth), halfWidth, shared_from_this());
    branches[3] = std::make_shared<Octree>(gpuOctree->center + Vec3(quarterWidth,quarterWidth,quarterWidth), halfWidth, shared_from_this());
    branches[4] = std::make_shared<Octree>(gpuOctree->center + Vec3(quarterWidth,quarterWidth,-quarterWidth), halfWidth, shared_from_this());
    branches[5] = std::make_shared<Octree>(gpuOctree->center + Vec3(quarterWidth,-quarterWidth,quarterWidth), halfWidth, shared_from_this());
    branches[6] = std::make_shared<Octree>(gpuOctree->center + Vec3(-quarterWidth,quarterWidth,-quarterWidth), halfWidth, shared_from_this());
    branches[7] = std::make_shared<Octree>(gpuOctree->center + Vec3(quarterWidth,-quarterWidth,-quarterWidth), halfWidth, shared_from_this());
}

//void Octree::migrateParticleUp(ParticlePtr& particleToMigrate) {
//    if (contains(particleToMigrate->pos)){
//        addParticle(particleToMigrate);
//    }
//    else{
//        OctreePtr parentLocked = parent.lock();
//        if (parentLocked != nullptr){
//            parentLocked->migrateParticleUp(particleToMigrate);
//        }
//    }
//}

void Octree::fillOctree(Particles& particles){
    for (auto& particle : particles){
        addParticle(particle);
    }
}

void Octree::printOctree(int n){
    if (particle != nullptr && branches[0] != nullptr) throw std::runtime_error("Octree has both particle and branches");
    std::cout << std::string(3*n, ' ') << "Octree with  center " << gpuOctree->center <<" and mass center " << gpuOctree->massCenter << " with width " << gpuOctree->width << " and mass " << gpuOctree->mass << std::endl;
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
    gpuOctree->mass = 0;
    gpuOctree->massCenter = Vec3(0,0,0);
    if (particle != nullptr){
        gpuOctree->mass = particle->mass;
        gpuOctree->massCenter = particle->pos;
        return;
    }
    if (branches[0] != nullptr){
        for (auto& branch : branches){
            branch->updateMassCenter();
            gpuOctree->mass += branch->gpuOctree->mass;
            gpuOctree->massCenter += branch->gpuOctree->massCenter * branch->gpuOctree->mass;
        }
    }
    if (gpuOctree->mass != 0) gpuOctree->massCenter = gpuOctree->massCenter / gpuOctree->mass;
    else {
        mergeBranches();
        //std::cout << "mergeBranches" << std::endl;
    }
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
        if (branch->gpuOctree->mass == 0) n++;
    }
    return n;
}

// Function to merge the branches of the octree, the branches are merged into the parent node. Branches need to be empty
void Octree::mergeBranches(){
    for (auto& branch : branches){
        branch = nullptr;
    }
}


std::vector<GPUOctreePtr> Octree::getFlattenedOctree(unsigned int& index){
    //std::cout << "actual index: " << index << std::endl; 
    std::vector<GPUOctreePtr> flattenedOctree;
    if (gpuOctree->mass == 0){
        return flattenedOctree;
    }
    else{
        flattenedOctree.push_back(gpuOctree); 
        gpuOctree->index = index;
        //std::cout << "gpuOctree index: " << flattenedOctree.back()->index << std::endl << std::endl;
        index++;          
        for (auto& branch : branches){
            if (branch == nullptr) break;
            if (branch->gpuOctree->mass != 0){
                auto branchFlattened = branch->getFlattenedOctree(index);
                flattenedOctree.insert(flattenedOctree.end(), branchFlattened.begin(), branchFlattened.end());
            }
        }
        GPUOctreePtr branchGPUOctree = nullptr;
        for (auto& branch : branches){
            if (branch == nullptr) break;
            else if (branch->gpuOctree->mass != 0){
                if (branchGPUOctree != nullptr) branchGPUOctree->nextSiblingIndex = branch->gpuOctree->index;
                branchGPUOctree = branch->gpuOctree;
                branchGPUOctree->parentIndex = gpuOctree->index;
            } 
        }
        return flattenedOctree;

    }
}