#include <iostream>
#include "Octree.hpp"
#include "Simulation.hpp"
#include "Galaxy.hpp"
#include "Particle.hpp"
#include "glad.h"
#include <GLFW/glfw3.h>
#include <cmath>

#include "PointRenderer.hpp"
#include "Camera.hpp"




int main() {
    // Initialiser GLFW
    if (!glfwInit()) {
        std::cerr << "Erreur lors de l'initialisation de GLFW" << std::endl;
        return -1;
    }

    // Version OpenGL souhaitée : 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Créer une fenêtre
    GLFWwindow* window = glfwCreateWindow(800, 600, "Galactous", nullptr, nullptr);
    if (!window) {
        std::cerr << "Erreur de création de la fenêtre GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Faire de cette fenêtre le contexte courant
    glfwMakeContextCurrent(window);

    // Charger les fonctions OpenGL avec GLAD
    if (!gladLoadGL()) {
        std::cerr << "Erreur d'initialisation de GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL prêt ! Version : " << glGetString(GL_VERSION) << std::endl;

    // Créer le renderer de points 3D
    PointRenderer pointRenderer;

    // Configuration de la couleur de fond
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Fond gris foncé

    // Variables pour l'animation de la caméra
    float time = 0.0f;

    // Variables pour le calcul des FPS
    double lastTime = glfwGetTime();
    int frameCount = 0;
    double fpsUpdateInterval = 1.0; // Mettre à jour les FPS toutes les secondes

    // Boucle principale
    while (!glfwWindowShouldClose(window)) {
        // Calcul des FPS
        double currentTime = glfwGetTime();
        frameCount++;
        
        // Afficher les FPS toutes les secondes
        if (currentTime - lastTime >= fpsUpdateInterval) {
            double fps = frameCount / (currentTime - lastTime);
            std::cout << "FPS: " << fps << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activer le test de profondeur
        glEnable(GL_DEPTH_TEST);

        // Animation de la caméra (rotation autour de l'origine)
        time += 0.0001f;
        pointRenderer.orbitCamera(time * 0.1f, 0.3f);

        // Exemples d'utilisation avec vraies coordonnées 3D
        // Point blanc au centre
        pointRenderer.drawPoint(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
        
        // Points colorés dans l'espace 3D
        pointRenderer.drawPoint(2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);  // Rouge à droite
        pointRenderer.drawPoint(-2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); // Vert à gauche
        pointRenderer.drawPoint(0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f);  // Bleu en haut
        pointRenderer.drawPoint(0.0f, -2.0f, 0.0f, 1.0f, 1.0f, 0.0f); // Jaune en bas
        pointRenderer.drawPoint(0.0f, 0.0f, 2.0f, 1.0f, 0.0f, 1.0f);  // Magenta devant
        pointRenderer.drawPoint(0.0f, 0.0f, -2.0f, 0.0f, 1.0f, 1.0f); // Cyan derrière

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    /*
    GalaxyPtr galaxy = std::make_shared<Galaxy>(100000, 1000.0f, 5.0f);
    //galaxy->printParticles();
    OctreePtr octree = std::make_shared<Octree>(Vec3(0, 0, 0), 10.0f);
    octree->fillOctree(galaxy->particles);
    octree->updateMassCenter();
    std::cout <<  "galaxy mass : " << galaxy->mass << std::endl;
    std::cout << "octree mass : " <<octree->mass << std::endl;
    std::cout << "nb particules : " << Octree::nbParticul << std::endl;
    //octree->printOctree();*/
    return 0;
}