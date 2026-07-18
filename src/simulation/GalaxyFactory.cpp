#include "GalaxyFactory.hpp"
#include <cstdlib>
#include <ctime>

Galaxy GalaxyFactory::generateGalaxy(size_t nbParticles, scalar_t mass_, scalar_t radius, scalar_t thickness, scalar_t starSpeed){
    static bool randSeeded = false;
    if (!randSeeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        randSeeded = true;
    }

    std::vector<ParticlePtr> particles;
    particles.reserve(nbParticles);

    unsigned int count = 0;
    while (count < nbParticles){
        scalar_t x = -radius + (2 * radius * rand()) / (RAND_MAX + 1);
        scalar_t y = -thickness + (2 * thickness * rand()) / (RAND_MAX + 1);
        scalar_t z = -radius + (2 * radius * rand()) / (RAND_MAX + 1);

        if (x*x + z*z + y*y * radius*radius / (thickness*thickness) <= radius*radius){
            Vec3 pos = Vec3(x, y, z);
            Vec3 normal = Vec3(0, 1, 0);
            Vec3 v = pos.vectorialProduct(normal) * starSpeed / radius / 1300;
            v.y = 0;

            Particle particle(mass_ / nbParticles, Vec3(x, y, z), v, TypeParticle::STAR);
            particles.push_back(std::make_shared<Particle>(particle));
            count++;
        }
    }
    return Galaxy(particles);
}