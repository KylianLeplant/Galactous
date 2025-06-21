#include "Galaxy.hpp"

Galaxy::Galaxy(size_t nbParticles, scalar_t mass_,scalar_t radius){
    massCenter = Vec3(0, 0, 0);
    mass = mass_;
    scalar_t diameter = (2 * radius);
    srand(time(NULL));
    particles.reserve(nbParticles);
    for (size_t i = 0; i < nbParticles; i++){
        scalar_t x = (static_cast<scalar_t>(rand()) / RAND_MAX) * diameter - radius;
        scalar_t y = (static_cast<scalar_t>(rand()) / RAND_MAX) * diameter - radius;
        scalar_t z = (static_cast<scalar_t>(rand()) / RAND_MAX) * diameter - radius;
        particles.push_back(std::make_shared<Particle>(mass_ / nbParticles, Vec3(x, y, z), Vec3(0, 0, 0), TypeParticle::ETOILE));
    }
}


void Galaxy::printParticles(){
    for (auto& particle : particles){
        std::cout << "Particle " << particle->id << " : " << particle->pos << std::endl;
    }
}