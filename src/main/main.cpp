#include <iostream>
#include "Octree.hpp"
#include "Simulation.hpp"
#include "Galaxy.hpp"
#include "Particle.hpp"
#include <cmath>

#include "PointRenderer.hpp"
#include "GalaxyFactory.hpp"
#include "Camera.hpp"
#include "Window.hpp"
#include "Input.hpp"
#include "Page.hpp"
#include "ComputeShader.hpp"


int main() {
    //std::cout << "dsddsdsds" << sizeof(Vec3) << std::endl;
    size_t nbParticles =3000;
    scalar_t mass = 100.0f;
    scalar_t radius = 100.0f;
    scalar_t thickness = 1.0f;
    scalar_t starSpeed = 1.0f;

    WindowPtr window = std::make_shared<Window>();


    std::vector<cl::Device> devices = ComputeShader::getListDevices();

    Page page(window); 
    page.createGalaxy(nbParticles, mass, radius, thickness, starSpeed);



    // Configuration de la couleur de fond
    glClearColor(0.149f, 0.153f, 0.2f, 1.0f); // Fond gris foncé

    // Variables pour l'animation de la caméra
    float time = 0.0f;

    // Variables pour le calcul des FPS
    double lastTime = glfwGetTime();
    int frameCount = 0;
    double fpsUpdateInterval = 1.0; // Mettre à jour les FPS toutes les secondes

    page.run();
    float f = 0.0f;
    int counter = 0;
    int testCount = 0;

    return 0;
}