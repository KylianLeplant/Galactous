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

int main() {
    size_t nbParticles = 3000;
    scalar_t mass = 100.0f;
    scalar_t radius = 100.0f;
    scalar_t thickness = 1.0f;
    scalar_t starSpeed = 1.0f;
    //GalaxyPtr galaxy = std::make_shared<Galaxy>(1000, 1000.0f, 5.0f);

    WindowPtr window = std::make_shared<Window>();



    Page page(window); 
    page.createGalaxy(nbParticles, mass, radius, thickness, starSpeed);


    // Créer le renderer de points 3D

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
    //while (!glfwWindowShouldClose(window->getWindow())) {
    //    std::cout << "window->getWindow() = " << window->getWindow() << std::endl;
//
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    
    //    // Activer le test de profondeur
    //    glEnable(GL_DEPTH_TEST);
    //    //page.simulation->update();
    //    //page.printSimulation();
    //    // Rendu ImGui
    //    glfwPollEvents();
    //    if (input.isKeyPressed(Input::KEY_ESCAPE)) {
    //        glfwSetWindowShouldClose(window->getWindow(), GLFW_TRUE);
    //    }
    //    ImGui_ImplGlfw_NewFrame();
    //    ImGui_ImplOpenGL3_NewFrame();
    //    ImGui::NewFrame();
    //    
    //    ImGui::Begin("Galactous");
    //    ImGui::Text("Bienvenue dans Galactous !");
    //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    //    ImGui::ColorEdit3("clear color", (float*)&glClearColor);
    //    if (ImGui::Button("Button")) {
    //        counter++;
    //        std::cout << "counter = " << counter << std::endl;
    //    }
    //    ImGui::SameLine();
    //    ImGui::Text("counter = %d", counter);
    //    if (testCount / 4000 % 2 == 0) ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //    ImGui::End();
    //    
    //    ImGui::Render();
    //    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
    //    glfwSwapBuffers(window->getWindow());
    //}
/*
    // Variables pour ImGui
    float f = 0.0f;
    int counter = 0;
    int testCount = 0;
    // Boucle principale
    while (!glfwWindowShouldClose(window->getWindow())) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activer le test de profondeur
        glEnable(GL_DEPTH_TEST);

        // Animation de la caméra (rotation autour de l'origine)
        time += 0.0001f;
        //pointRenderer.orbitCamera(time * 0.1f, 0.3f);

        // Exemples d'utilisation avec vraies coordonnées 3D
        // Point blanc au centre
        //pointRenderer.drawPoint(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
        //
        //// Points colorés dans l'espace 3D
        //pointRenderer.drawPoint(2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);  // Rouge à droite
        //pointRenderer.drawPoint(-2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); // Vert à gauche
        //pointRenderer.drawPoint(0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f);  // Bleu en haut
        //pointRenderer.drawPoint(0.0f, -2.0f, 0.0f, 1.0f, 1.0f, 0.0f); // Jaune en bas
        //pointRenderer.drawPoint(0.0f, 0.0f, 2.0f, 1.0f, 0.0f, 1.0f);  // Magenta devant
        //pointRenderer.drawPoint(0.0f, 0.0f, -2.0f, 0.0f, 1.0f, 1.0f); // Cyan derrière
        page.simulation->update();
        page.printSimulation();
        // Rendu ImGui
        glfwPollEvents();
        if (input.isKeyPressed(Input::KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window->getWindow(), GLFW_TRUE);
        }
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Galactous");
        ImGui::Text("Bienvenue dans Galactous !");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&glClearColor);
        if (ImGui::Button("Button")) {
            counter++;
            std::cout << "counter = " << counter << std::endl;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        if (testCount / 4000 % 2 == 0) ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window->getWindow());
    }*/

    // Nettoyer ImGui
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();


    /*
    GalaxyPtr galaxy = std::make_shared<Galaxy>(100000, 1000.0f, 5.0f);
    //galaxy->printParticles();
    OctreePtr octree = std::make_shared<Octree>(Vec3(0, 0, 0), 10.0f);
    octree->fillOctree(galaxy->particles);
    octree->updateMassCenter();
    std::cout <<  "galaxy mass : " << galaxy->mass << std::endl;
    std::cout << "octree mass : " <<octree->mass << std::endl;
    //octree->printOctree();*/
    return 0;
}