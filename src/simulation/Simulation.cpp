#include "Simulation.hpp"
#include <algorithm>
#include <chrono>

void Simulation::update(){
    //for (auto& galaxy : galaxies){
    //    for (auto& particle : galaxy->particles){
    //        particle->acceleration = Vec3(0, 0, 0);
    //        updateAcceleration(particle, octreeRoot);
    //    }
    //}
    auto start = std::chrono::high_resolution_clock::now();
    updateWithGPU();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "updateWithGPU : " << elapsed.count() << std::endl;
    double sum = 0.0;
    for (auto& galaxy : galaxies) {
        std::vector<ParticlePtr> to_remove;
        for (auto& particle : galaxy->particles) {
            start = std::chrono::high_resolution_clock::now();
            if (!updatePosition(particle)) {
                end = std::chrono::high_resolution_clock::now();
                elapsed = end - start;
                sum+=elapsed.count();
                //std::cout << "updatePosition : " << elapsed.count() << std::endl;
                //std::cout << "!updatePosition" << std::endl;
                to_remove.push_back(particle);
                //std::cout << "toremove.push_back" << std::endl;
            }
        }
    }
    std::cout << "updatePosition : " << sum/3000 << std::endl;
    //std::cout << "pre update masscenter" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    octreeRoot->updateMassCenter();
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "updateMassCenter : " << elapsed.count() << std::endl;
    //std::cout << "post update masscenter" << std::endl;
    //flattenedOctree.clear();
    //std::cout << "post flattenedOctree.clear" << std::endl;
    //unsigned int index = 0;
    //std::cout << "pre getFlattenedOctree" << std::endl;
    //auto octreeFlat = octreeRoot->getFlattenedOctree(index);
    //std::cout << "post getFlattenedOctree" << std::endl;
    //std::cout << "size :" << octreeRoot->size() << std::endl;
    ////std::cout << "octreeRoot"<<std::endl;
    //octreeRoot->printOctree();
    ////std::cout << "\n\n\noctreeFlat" << std::endl;
    //for (auto& octree : octreeFlat){
    //    //std::cout<< "index: " << octree->index << " mass: " << octree->mass << " center: " << octree->center << " parentIndex: " << octree->parentIndex << " nextSiblingIndex: " << octree->nextSiblingIndex << std::endl;
    //}
    //std::cout << "pre insert flattenedOctree" << std::endl;
    //flattenedOctree.insert(flattenedOctree.end(), octreeFlat.begin(), octreeFlat.end());
}

void Simulation::updateWithGPU(){
    auto start = std::chrono::high_resolution_clock::now();
    ComputeShader::init("shaders/compute/computeShader.cl");    // Création des buffers OpenCL
    
    cl::Context context = ComputeShader::getContext();
    std::vector<Vec3> positions;
    std::vector<float> masses;
    for (auto& galaxy : galaxies){
        for (auto& particle : galaxy->particles){
            positions.push_back(particle->pos);
            masses.push_back(particle->mass);
        }
    }
    unsigned int index = 0;
    //auto start = std::chrono::high_resolution_clock::now();
    std::vector<GPUOctreePtr> flattenedOctree = octreeRoot->getFlattenedOctree(index);
    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> elapsed = end - start;
    //std::cout << "\nflattenedOctree : " << elapsed.count() << std::endl;
    unsigned int sizeOctree = flattenedOctree.size();
    float softening = 1e-2f;
    float G = 6.67e-10f; // Gravitational constant

    std::vector<Vec3> accelerations(positions.size(), Vec3(0.0f, 0.0f, 0.0f));
    std::vector<Vec3> octreeCenters;
    std::vector<float> octreeWidths;
    std::vector<Vec3> octreeMassCenters;
    std::vector<float> octreeMasses;
    std::vector<unsigned int> octreeNextSiblingIndexes;
    std::vector<unsigned int> octreeParentIndexes;
    std::vector<unsigned int> octreeIndexes;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "\nPartie 1 : " << elapsed.count() << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (auto& gpuOctree : flattenedOctree) {
        octreeCenters.push_back(gpuOctree->center);
        octreeWidths.push_back(gpuOctree->width);
        octreeMasses.push_back(gpuOctree->mass);
        octreeMassCenters.push_back(gpuOctree->massCenter);
        octreeNextSiblingIndexes.push_back(gpuOctree->nextSiblingIndex);
        octreeParentIndexes.push_back(gpuOctree->parentIndex);
        octreeIndexes.push_back(gpuOctree->index);
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "\nPartie 2 (octree) : " << elapsed.count() << std::endl;

    start = std::chrono::high_resolution_clock::now();
    cl::Buffer bufferPositions = ComputeShader::Buffer(positions, Permissions::Read);
    cl::Buffer bufferMasses = ComputeShader::Buffer(masses, Permissions::Read);
    cl::Buffer bufferAccelerations = ComputeShader::Buffer(accelerations, Permissions::Write);
    cl::Buffer bufferSoftening = ComputeShader::Buffer(&softening, Permissions::Read);
    cl::Buffer bufferG = ComputeShader::Buffer(&G, Permissions::Read);
    cl::Buffer bufferOctreeCenters = ComputeShader::Buffer(octreeCenters, Permissions::Read);
    cl::Buffer bufferOctreeWidths = ComputeShader::Buffer(octreeWidths, Permissions::Read);
    cl::Buffer bufferOctreeMassCenters = ComputeShader::Buffer(octreeMassCenters, Permissions::Read);
    cl::Buffer bufferOctreeMasses = ComputeShader::Buffer(octreeMasses, Permissions::Read);
    cl::Buffer bufferOctreeNextSiblingIndexes = ComputeShader::Buffer(octreeNextSiblingIndexes, Permissions::Read);
    cl::Buffer bufferOctreeParentIndexes = ComputeShader::Buffer(octreeParentIndexes, Permissions::Read);
    cl::Buffer bufferOctreeIndexes = ComputeShader::Buffer(octreeIndexes, Permissions::Read);
    cl::Buffer bufferSizeOctree = ComputeShader::Buffer(&sizeOctree, Permissions::Read);

    
    std::vector<cl::Buffer*> buffers = {&bufferPositions, &bufferMasses, &bufferAccelerations, &bufferSoftening, &bufferG, &bufferOctreeCenters, &bufferOctreeWidths, &bufferOctreeMassCenters, &bufferOctreeMasses, &bufferOctreeNextSiblingIndexes, &bufferOctreeParentIndexes, &bufferOctreeIndexes, &bufferSizeOctree};
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "\nPartie 3 (buffers) : " << elapsed.count() << std::endl;

   
    //std::cout << "pre launch" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    ComputeShader::launch("accelerations", buffers, cl::NDRange(positions.size()));
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "launch :" << elapsed.count() << std::endl;
    

    start = std::chrono::high_resolution_clock::now();
    cl::CommandQueue queue = ComputeShader::getQueue();
    
    cl_int err = queue.enqueueReadBuffer(bufferAccelerations, CL_TRUE, 0, sizeof(Vec3) * accelerations.size(), accelerations.data());
    if (err != CL_SUCCESS) {
        std::cerr << "Erreur lors de l'exécution du kernel : " << err << std::endl;
    }
    unsigned int indexGalaxy = 0;
    unsigned int indexParticle = 0;
    
    for (unsigned int i = 0; i< accelerations.size(); i++){
        if (galaxies[indexGalaxy]->particles.size() <= i){
            indexGalaxy++;
            indexParticle = 0;
        }
        galaxies[indexGalaxy]->particles[indexParticle]->acceleration = accelerations[i];
        //std::cout << "acceleration[" << i << "] : " << accelerations[i] << std::endl;
        indexParticle++;
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "updateAcceleration : " << elapsed.count() << std::endl;
    //for (int i = 0; i < 2; i++) {
    //    std::cout<< "accelerations[" << i << "] = " << accelerations[i] << std::endl;
    //}
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
    if (particle->pos.norm() > 200) {
        std::cout << "pos : " << particle->pos << "velocity : " << particle->velocity << "acceleration : " << particle->acceleration << std::endl;
    }
    particle->velocity += particle->acceleration * time_step/2;
    OctreePtr octree = particle->octree.lock(); 
    if (octree != nullptr){
        if(!octree->contains(particle->pos)){
            OctreePtr parentLocked = octree->parent.lock();
            if (parentLocked != nullptr){
                octree->particle=nullptr;
                migrateParticleUp(particle,parentLocked);
                //std::cout << "return to updatepos" << std::endl;
                return true;
            }
        }
    }
    return false;
}

void Simulation::migrateParticleUp(ParticlePtr& particle,OctreePtr& octree){
    if (octree->contains(particle->pos)){
        //std::cout << "add migrate " << particle->id << " to octree at position " << octree->center << std::endl;
        octree->addParticle(particle);
        //std::cout << "added";
    }
    else{   
        //std::cout << "else migrate" << particle->id <<std::endl;
        OctreePtr parentLocked = octree->parent.lock();

        if (parentLocked != nullptr){
            //std::cout << "migrating particle " << particle->id << " to parent octree at position " << parentLocked->center << std::endl;
            migrateParticleUp(particle,parentLocked);
        }
        else{
            //std::cout << "particle is not in the octree" << std::endl;
        }
    }
    //std::cout << "END MIGRATION" << std::endl;
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


