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

void DrawParticles(const Particles& particles, Color color) {
    // Dessiner en batch avec des petits cubes
    for (const auto& particlePtr : particles) {
        Vector3 pos = particlePtr->pos.toVector3();
        DrawCubeV(pos, {0.5f, 0.5f, 0.5f}, color);
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
            GalaxyWeakPtr galaxyWeakPtr = simulation->galaxies.front();
            auto galaxy = galaxyWeakPtr.lock();
            Particles& particles = galaxy->particles;
            if (galaxy) {
                Particles& particles = galaxy->particles;
                
                // Log la position de la première particule toutes les 60 frames
                static int frameCount = 0;
                if (!particles.empty()) {
                    Vec3 pos = particles[0]->pos;
                    //std::cout << "Particle[0] pos: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
                }
                frameCount++;
                
                DrawParticles(particles, WHITE);
            }

            DrawParticles(particles, WHITE);

            EndMode3D();

            DrawText("Welcome to the third dimension!", 10, 40, 20, DARKGRAY);

            DrawFPS(10, 10);
        
        EndDrawing();
    };

    
    std::cout << "Program completed successfully." << std::endl;
    std::cout.flush();
    return 0;
}