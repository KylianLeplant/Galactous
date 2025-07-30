// Update the include path if Octree.hpp is in an 'include' folder relative to your project root
#include "Octree.hpp"

OctreePtr Octree::root = nullptr;

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
    //std::cout<<"ADD PARTICLE with pos :"<< newParticle->pos << "root size :" << Octree::root->size() <<std::endl;
    if (particle == nullptr && branches[0] == nullptr) {    //if the octree is a leaf and is empty, we add the particle to the octree
        //std::cout << "was empty";
        particle = newParticle;
        //std::cout << ", particle set";
        particle->octree = shared_from_this();
        //std::cout << ", octree set" << std::endl;
        return;
    } 
    else {    
        // If the node already contains a particle, we need to subdivide
        // and distribute the particles among the branches
        if (branches[0] == nullptr) {
            //std::cout << "subdividing" << std::endl;
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
        //std::cout <<width <<"No branch found for particle at position " << newParticle->pos << std::endl;
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
        //std::cout<<std::endl << particle->id << " at position " << particle->pos << std::endl;
        addParticle(particle);
    }
    //std::cout << "\n\n\nOctree filled with " << Octree::root->size() << " particles.\n\n\n" << std::endl;
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
        if (branch->mass == 0) n++;
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
    if (mass == 0){
        return flattenedOctree;
    }
    else{
        updateGPUOctree();
        gpuOctree->index = index;    
        flattenedOctree.push_back(gpuOctree); 
        //std::cout << "gpuOctree index: " << flattenedOctree.back()->index << std::endl << std::endl;
        index++;          
        for (auto& branch : branches){
            if (branch == nullptr) break;
            if (branch->mass != 0){
                auto branchFlattened = branch->getFlattenedOctree(index);
                flattenedOctree.insert(flattenedOctree.end(), branchFlattened.begin(), branchFlattened.end());
            }
        }
        GPUOctreePtr branchGPUOctree = nullptr;
        for (auto& branch : branches){
            if (branch == nullptr) break;
            else if (branch->mass != 0){
                if (branchGPUOctree != nullptr) branch->gpuOctree->nextSiblingIndex = branch->gpuOctree->index;
                branchGPUOctree = branch->gpuOctree;
                branch->gpuOctree->parentIndex = index;
            } 
        }
        return flattenedOctree;

    }
}

void Octree::updateGPUOctree() {
    gpuOctree->center[0] = center.x;
    gpuOctree->center[1] = center.y;
    gpuOctree->center[2] = center.z;
    gpuOctree->massCenter[0] = massCenter.x;
    gpuOctree->massCenter[1] = massCenter.y;
    gpuOctree->massCenter[2] = massCenter.z;
    gpuOctree->mass = mass;
    gpuOctree->width = width;
}