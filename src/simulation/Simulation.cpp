#include "Simulation.hpp"
#include <algorithm>
#include <chrono>

void Simulation::update(){
    while(!stopFlag.load()){
        //updateParticlesData();
        std::cout << "UPDATE" << std::endl;
        //for (auto& galaxy : galaxies){
        //    for (auto& particle : galaxy->particles){
        //        particle->acceleration = Vec3(0, 0, 0);
        //        updateAcceleration(particle, octreeRoot);
        //    }
        //}
        auto start = std::chrono::high_resolution_clock::now();
        //updateAccelerationWithGPU();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        //std::cout << "updateAccelerationWithGPU : " << elapsed.count() << std::endl;
        double sum = 0.0;
        for (auto& galaxy : galaxies) {
            std::unordered_set<ParticlePtr> to_remove;
            for (auto& particle : galaxy->particles) {
                start = std::chrono::high_resolution_clock::now();
                if (!updatePosition(particle)) {
                    end = std::chrono::high_resolution_clock::now();
                    elapsed = end - start;
                    sum+=elapsed.count();
                    //std::cout << "updatePosition : " << elapsed.count() << std::endl;
                    //std::cout << "!updatePosition" << std::endl;
                    //std::cout << "toremove.push_back" << std::endl;
                }
                else{
                    to_remove.insert(particle);
                }
            }
            std::cout << "size to_remove : " << to_remove.size() << std::endl;
            std::cout << "nb_particles_galaxy : " << galaxy->particles.size() << std::endl;
            std::cout << "A" << std::endl;
            //for (int i = (to_remove.size()) - 1; i >= 0; --i) {
            //    std::cout << "avant B" << std::endl;
            //    std::cout << "B " << i << " , " << galaxy->particles.size() << std::endl;
            //    if (galaxy->particles[i]->octree.lock()) {
            //        std::cout << "count : " << galaxy->particles[i].use_count()<<std::endl;
            //        galaxy->particles[i]->octree.lock()->deleteParticle();
            //        std::cout << "count : " << galaxy->particles[i].use_count()<<std::endl;        
            //        std::cout << "octree deleted " << galaxy->particles.size() << std::endl;
            //    }
            //    std::cout << "C " << i << " , " << galaxy->particles.size() << std::endl;
            //    //supprime la particule   de la galaxie
            //    galaxy->particles[i].reset();
            //    std::erase(galaxy->particles, galaxy->particles[i]);
            //    std::cout << "D " << i << std::endl;
            //}
            //std::cout << "nb_particles_galaxy : " << galaxy->particles.size() << std::endl;
        }
        std::cout << "nb_particles_octree : " << octreeRoot->size() << std::endl << std::endl;
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
}

void Simulation::updateAccelerationWithGPU(){
    while(!stopFlag.load()){
        auto superstart = std::chrono::high_resolution_clock::now();
        std::cout << "UPDATE WITH GPU" << std::endl;
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
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(end - start);
        std::cout << "\nPartie 1 : " << elapsed.count() << std::endl;

        start = std::chrono::high_resolution_clock::now();
        FlattenedOctree flattenedOctree;
        octreeRoot->getFlattenedOctree(flattenedOctree);
        
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "\nflattenedOctree : " << elapsed.count() << std::endl;
        unsigned int sizeOctree = flattenedOctree.size();
        //float softening = 1e-2f;
        //float G = 6.67e-10f; // Gravitational constant

        std::vector<Vec3> accelerations(positions.size(), Vec3(0.0f, 0.0f, 0.0f));
        //std::vector<Vec3> octreeCenters;
        //std::vector<float> octreeWidths;
        //std::vector<Vec3> octreeMassCenters;
        //std::vector<float> octreeMasses;
        //std::vector<unsigned int> octreeNextSiblingIndexes;
        //std::vector<unsigned int> octreeParentIndexes;
        //std::vector<unsigned int> octreeIndexes;
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "\nPartie 2 : " << elapsed.count() << std::endl;
        //start = std::chrono::high_resolution_clock::now();
        //for (auto& gpuOctree : flattenedOctree) {
        //    octreeCenters.push_back(gpuOctree->center);
        //    octreeWidths.push_back(gpuOctree->width);
        //    octreeMasses.push_back(gpuOctree->mass);
        //    octreeMassCenters.push_back(gpuOctree->massCenter);
        //    octreeNextSiblingIndexes.push_back(gpuOctree->nextSiblingIndex);
        //    octreeParentIndexes.push_back(gpuOctree->parentIndex);
        //    octreeIndexes.push_back(gpuOctree->index);
        //}
        //end = std::chrono::high_resolution_clock::now();
        //elapsed = end - start;
        //std::cout << "\nPartie 2 (octree) : " << elapsed.count() << std::endl;

        start = std::chrono::high_resolution_clock::now();
        cl::Buffer bufferPositions = ComputeShader::Buffer(positions, Permissions::Read);
        cl::Buffer bufferMasses = ComputeShader::Buffer(masses, Permissions::Read);
        cl::Buffer bufferAccelerations = ComputeShader::Buffer(accelerations, Permissions::Write);
        cl::Buffer bufferSoftening = ComputeShader::Buffer(&softening, Permissions::Read);
        cl::Buffer bufferG = ComputeShader::Buffer(&G, Permissions::Read);
        cl::Buffer bufferOctreeCenters = ComputeShader::Buffer(flattenedOctree.centers, Permissions::Read);
        cl::Buffer bufferOctreeWidths = ComputeShader::Buffer(flattenedOctree.widths, Permissions::Read);
        cl::Buffer bufferOctreeMassCenters = ComputeShader::Buffer(flattenedOctree.massCenters, Permissions::Read);
        cl::Buffer bufferOctreeMasses = ComputeShader::Buffer(flattenedOctree.masses, Permissions::Read);
        cl::Buffer bufferOctreeNextSiblingIndexes = ComputeShader::Buffer(flattenedOctree.nextSiblingIndices, Permissions::Read);
        cl::Buffer bufferOctreeParentIndexes = ComputeShader::Buffer(flattenedOctree.parentIndices, Permissions::Read);
        //cl::Buffer bufferOctreeIndexes = ComputeShader::Buffer(octreeIndexes, Permissions::Read);
        cl::Buffer bufferSizeOctree = ComputeShader::Buffer(&sizeOctree, Permissions::Read);

        
        std::vector<cl::Buffer*> buffers = {&bufferPositions, &bufferMasses, &bufferAccelerations, &bufferSoftening, &bufferG, &bufferOctreeCenters, &bufferOctreeWidths, &bufferOctreeMassCenters, &bufferOctreeMasses, &bufferOctreeParentIndexes, &bufferOctreeNextSiblingIndexes, &bufferSizeOctree}; //, &bufferOctreeIndexes
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "\nPartie 3 (buffers) : " << elapsed.count() << std::endl;

        std::cout << "sizeOctree : " << sizeOctree << std::endl;
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
            
        //for (int i = 0; i < 2; i++) {
        //    std::cout<< "accelerations[" << i << "] = " << accelerations[i] << std::endl;
        //}
        
        auto superend = std::chrono::high_resolution_clock::now();
        elapsed = superend-superstart;
        std::cout << "total : " << elapsed.count() << std::endl;
    }
}

void Simulation::updateAccelerationParticle(ParticlePtr particle, OctreePtr octreeRoot){
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
        else updateAccelerationParticle(particle, octreeBranch);
    }
    //particle->acceleration = particle->acceleration.limitNorm(max_acceleration);
    
}

void Simulation::updateAcceleration(){
    while(!stopFlag.load()){
        for (auto& galaxy : galaxies){
            for (auto& particle : galaxy->particles){
                updateAccelerationParticle(particle, octreeRoot);
            }
        }
    }
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

void Simulation::updateParticlesData() {
    ParticlesDataPtr newParticlesData = std::make_shared<ParticlesData>();
    for (auto& galaxy : galaxies){
        for (auto& particle : galaxy->particles){
            newParticlesData->positions.push_back(particle->pos);
            newParticlesData->masses.push_back(particle->mass);
        }
    }
    newParticlesData->accelerations = std::vector<Vec3>(newParticlesData->positions.size(), Vec3(0.0f, 0.0f, 0.0f));
    particlesDataQueue.push(newParticlesData);
}

void Simulation::run(bool withGPU){
    threadUpdateParticlesData=std::thread(&Simulation::updateParticlesData, this);
    if (withGPU){
        threadUpdateAcceleration=std::thread(&Simulation::updateAccelerationWithGPU, this);
    }
    else{
        threadUpdateAcceleration=std::thread(&Simulation::updateAcceleration, this);
    }
    update();
}


void Simulation::stop(){
    stopFlag.store(true);
    if (threadUpdateParticlesData.joinable()) {
        threadUpdateParticlesData.join();
    }
    if (threadUpdateAcceleration.joinable()) {
        threadUpdateAcceleration.join();
    }
}