#include <iostream>
#include "Octree.hpp"
#include "Simulation.hpp"
#include "Galaxy.hpp"
#include "Particle.hpp"
#include <cmath>
#include "raylib.h"
#include "rlgl.h"

#include "GalaxyFactory.hpp"
#include "ComputeShader.hpp"

void DrawParticles(const std::vector<Vec3>& particlesPos, Color color) {
    // Dessiner en batch avec des petits cubes
    
    for (const auto& position : particlesPos) {
        DrawCubeV(position.toVector3(), {0.5f, 0.5f, 0.5f}, color);
    }
}

int main() {
    std::cout << "Starting program..." << std::endl;
    std::cout.flush();
    
    std::cout << "Vec3 size: " << sizeof(Vec3) << std::endl;
    std::cout.flush();
    
    size_t nbParticles =30000;
    scalar_t mass = 100.0f;
    scalar_t radius = 100.0f;
    scalar_t thickness = 1.0f;
    scalar_t starSpeed = 1.0f;

    SimulationPtr simulation = std::make_shared<Simulation>(2000.0f);
    simulation->init();
    simulation->createGalaxy(nbParticles, mass, radius, thickness, starSpeed);
    std::thread threadSimulation(&Simulation::run, simulation.get(), true);

    std::cout << "Searching for OpenCL devices..." << std::endl;
    std::cout.flush();

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 200.0f, 200.0f, 200.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
    
    try {
        std::vector<cl::Device> devices = ComputeShader::getListDevices();

        if (devices.size() == 0) {
            std::cerr << "No OpenCL devices found." << std::endl;
            std::cerr.flush();
            return 1;
        }
        
        std::cout << "Found " << devices.size() << " OpenCL device(s)." << std::endl;
        std::cout.flush();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr.flush();
        return 1;
    }
    int width{1920}, height{1080};
    InitWindow(width, height, "Galactous");
    SetTargetFPS(60);                    // Sets the highest FPS
    while (WindowShouldClose() == false) // Keep the Window open
    {
        UpdateCamera(&camera, CAMERA_FREE); // Update camera
        // Setup Canvas
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(camera);
            // Repères visuels
            DrawGrid(20, 20.0f);  // Grille adaptée au rayon de 100
            DrawLine3D({0,0,0}, {100,0,0}, RED);   // Axe X
            DrawLine3D({0,0,0}, {0,100,0}, GREEN); // Axe Y
            DrawLine3D({0,0,0}, {0,0,100}, BLUE);  // Axe Z
            
            // Dessiner toutes les particules en un seul appel
            ParticlesDataPtr particlesData = simulation->currentParticlesData.load(std::memory_order_acquire);
            if (particlesData) {
                std::vector<Vec3> particlesPos = particlesData->positions;
                DrawParticles(particlesPos, WHITE);
            }


            EndMode3D();

            DrawFPS(10, 10);
        
        EndDrawing();
    };

    
    std::cout << "Program completed successfully." << std::endl;
    std::cout.flush();
    return 0;
}