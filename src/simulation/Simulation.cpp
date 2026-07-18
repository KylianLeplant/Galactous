#include "Simulation.hpp"
#include <algorithm>

void Simulation::update(){
    while(!stopFlag.load()){
        for (auto& galaxy : galaxies) {
            for (auto& particle : galaxy->particles) {
                updatePosition(particle);
            }
        }
        octreeRoot->updateMassCenter();
    }
}

void Simulation::updateWithGPU(){
    while(!stopFlag.load()){
        FlattenedOctreePtr flattenedOctree = currentFlattenedOctree.load(std::memory_order_acquire);
        if (!flattenedOctree) {
            flattenedOctree = std::make_shared<FlattenedOctree>();
            octreeRoot->getFlattenedOctree(flattenedOctree);
            currentFlattenedOctree.store(flattenedOctree, std::memory_order_release);
        }

        ParticlesDataPtr particlesData = currentParticlesData.load(std::memory_order_acquire);
        if (!particlesData) {
            continue;
        }

        unsigned int sizeOctree = flattenedOctree->centers.size();

        cl::Buffer bufferPositions = ComputeShader::Buffer(particlesData->positions, Permissions::All);
        cl::Buffer bufferMasses = ComputeShader::Buffer(particlesData->masses, Permissions::Read);
        cl::Buffer bufferVelocities = ComputeShader::Buffer(particlesData->velocities, Permissions::All);
        cl::Buffer bufferSoftening = ComputeShader::Buffer(&softening, Permissions::Read);
        cl::Buffer bufferG = ComputeShader::Buffer(&G, Permissions::Read);
        cl::Buffer bufferOctreeCenters = ComputeShader::Buffer(flattenedOctree->centers, Permissions::Read);
        cl::Buffer bufferOctreeWidths = ComputeShader::Buffer(flattenedOctree->widths, Permissions::Read);
        cl::Buffer bufferOctreeMassCenters = ComputeShader::Buffer(flattenedOctree->massCenters, Permissions::Read);
        cl::Buffer bufferOctreeMasses = ComputeShader::Buffer(flattenedOctree->masses, Permissions::Read);
        cl::Buffer bufferOctreeNextSiblingIndexes = ComputeShader::Buffer(flattenedOctree->nextSiblingIndices, Permissions::Read);
        cl::Buffer bufferOctreeParentIndexes = ComputeShader::Buffer(flattenedOctree->parentIndices, Permissions::Read);
        cl::Buffer bufferSizeOctree = ComputeShader::Buffer(&sizeOctree, Permissions::Read);
        cl::Buffer buffertimeStep = ComputeShader::Buffer(&time_step, Permissions::Read);

        std::vector<cl::Buffer*> buffers = {
            &bufferPositions, &bufferMasses, &bufferVelocities,
            &bufferSoftening, &bufferG,
            &bufferOctreeCenters, &bufferOctreeWidths, &bufferOctreeMassCenters,
            &bufferOctreeMasses, &bufferOctreeParentIndexes, &bufferOctreeNextSiblingIndexes,
            &bufferSizeOctree, &buffertimeStep
        };

        ComputeShader::launch("accelerations", buffers, cl::NDRange(particlesData->positions.size()));

        cl::CommandQueue queue = ComputeShader::getQueue();

        cl_int err = queue.enqueueReadBuffer(bufferVelocities, CL_TRUE, 0,
            sizeof(Vec3) * particlesData->velocities.size(), particlesData->velocities.data());
        cl_int err2 = queue.enqueueReadBuffer(bufferPositions, CL_TRUE, 0,
            sizeof(Vec3) * particlesData->positions.size(), particlesData->positions.data());
        if (err != CL_SUCCESS) {
            std::cerr << "Erreur lors de l'exécution du kernel : " << err << std::endl;
        }
        if (err2 != CL_SUCCESS) {
            std::cerr << "Erreur lors de la lecture des positions : " << err2 << std::endl;
        }

        unsigned int indexGalaxy = 0;
        unsigned int indexParticle = 0;
        for (unsigned int i = 0; i < particlesData->positions.size(); i++){
            if (galaxies[indexGalaxy]->particles.size() <= i){
                indexGalaxy++;
                indexParticle = 0;
            }
            galaxies[indexGalaxy]->particles[indexParticle]->velocity = particlesData->velocities[i];
            galaxies[indexGalaxy]->particles[indexParticle]->pos = particlesData->positions[i];
            indexParticle++;
        }
    }
}

void Simulation::updateAccelerationParticle(ParticlePtr particle, OctreePtr octreeRoot){
    if (octreeRoot->particle != nullptr){
        Vec3 r = octreeRoot->particle->pos - particle->pos;
        particle->acceleration += r * (G * octreeRoot->particle->mass / r.norm() - softening);
        return;
    }
    if (octreeRoot->branches[0] == nullptr){
        return;
    }
    for (auto& octreeBranch : octreeRoot->branches){
        scalar_t d = (particle->pos - octreeBranch->massCenter).norm() + softening;
        scalar_t s_d = octreeBranch->width / d;
        if (s_d < theta || octreeBranch->particle != nullptr){
            particle->acceleration += (octreeBranch->massCenter - particle->pos) * (G * octreeBranch->mass / d);
            continue;
        }
        else updateAccelerationParticle(particle, octreeBranch);
    }
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
    particle->velocity += particle->acceleration * time_step/2;
    OctreePtr octree = particle->octree.lock();
    if (octree != nullptr){
        if(!octree->contains(particle->pos)){
            OctreePtr parentLocked = octree->parent.lock();
            if (parentLocked != nullptr){
                octree->particle = nullptr;
                migrateParticleUp(particle, parentLocked);
                return true;
            }
        }
    }
    return false;
}

void Simulation::migrateParticleUp(ParticlePtr& particle, OctreePtr& octree){
    if (octree->contains(particle->pos)){
        octree->addParticle(particle);
    }
    else{
        OctreePtr parentLocked = octree->parent.lock();
        if (parentLocked != nullptr){
            migrateParticleUp(particle, parentLocked);
        }
    }
}

void Simulation::updateParticlesData() {
    ParticlesDataPtr newParticlesData = std::make_shared<ParticlesData>();
    for (auto& galaxy : galaxies){
        for (auto& particle : galaxy->particles){
            newParticlesData->positions.push_back(particle->pos);
            newParticlesData->masses.push_back(particle->mass);
            newParticlesData->velocities.push_back(particle->velocity);
        }
    }
    currentParticlesData.store(newParticlesData, std::memory_order_release);
}

void Simulation::updateFlattenedOctree() {
    while (!stopFlag.load()) {
        FlattenedOctreePtr flattenedOctree = std::make_shared<FlattenedOctree>();
        octreeRoot->getFlattenedOctree(flattenedOctree);
        currentFlattenedOctree.store(flattenedOctree, std::memory_order_release);
    }
}

void Simulation::run(bool withGPU){
    if (withGPU){
        ComputeShader::init("shaders/compute/computeShader.cl");
        threadUpdateParticlesData = std::thread(&Simulation::updateParticlesData, this);
        threadUpdate = std::thread(&Simulation::updateWithGPU, this);
        updateFlattenedOctree();
    }
    else{
        threadUpdate = std::thread(&Simulation::update, this);
        updateAcceleration();
    }
}

void Simulation::stop(){
    stopFlag.store(true);
    if (threadUpdateParticlesData.joinable()) {
        threadUpdateParticlesData.join();
    }
    if (threadUpdate.joinable()) {
        threadUpdate.join();
    }
}