#include "GalaxyFactory.hpp"

Galaxy GalaxyFactory::generateGalaxy(size_t nbParticles, scalar_t mass_,scalar_t radius, scalar_t thickness, scalar_t starSpeed){
    std::cout << "starSpeed = " << starSpeed << std::endl;
    unsigned int count = 0;
    std::vector<ParticlePtr> particles;
    while(count < nbParticles){
        scalar_t x = -radius + (2*radius*rand())/(RAND_MAX+1);
        scalar_t y = -thickness + (2*thickness*rand())/(RAND_MAX+1);
        scalar_t z = -radius + (2*radius*rand())/(RAND_MAX+1);
        Vec3 pos = Vec3(x,y,z);
        Vec3 normal = Vec3(0,1,0);
        scalar_t d = sqrt(x*x + z*z);
        Vec3 v = pos.vectorialProduct(normal) * starSpeed/radius/1300;                              //pos.vectorialProduct(normal) * inertiaMoment * sqrt(simulation->G*mass_/pow(radius,3));
        v.y = 0;
        //std::cout << "v = " << v.norm()  << " d = " << d << " starSpeed = " << starSpeed << std::endl;
        
        if (x*x + z*z + y*y*radius*radius/thickness/thickness <= radius*radius){
            Particle particle(mass_/nbParticles,Vec3(x,y,z),v,TypeParticle::STAR);
            particles.push_back(std::make_shared<Particle>(particle));
            count++;
        }
    }
    return Galaxy(particles);
}
