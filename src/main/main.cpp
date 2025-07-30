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
void test1() {
    ComputeShader::init("shaders/compute/computeShader.cl");    // Création des buffers OpenCL
    cl::Context context = ComputeShader::getContext();

    std::vector<int> a = {1, 1, 1, 2, 2, 2};
    std::vector<int> b = {3, 3, 3, 4, 4, 4};
    std::vector<int> result(6, 0);
    //cl::Buffer bufferA(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * a.size(), a.data());
    //cl::Buffer bufferB(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * b.size(), b.data());
    //cl::Buffer bufferResult(context, CL_MEM_WRITE_ONLY, sizeof(int) * result.size());
// OU
    cl::Buffer bufferA = ComputeShader::Buffer(a, Permissions::Read);
    cl::Buffer bufferB = ComputeShader::Buffer(b, Permissions::Read);
    cl::Buffer bufferResult = ComputeShader::Buffer(result, Permissions::Write);
    // Préparer le vector de pointeurs
    std::vector<cl::Buffer*> buffers = {&bufferA, &bufferB, &bufferResult};

    // Lancer le kernel
    ComputeShader::launch("add_arrays", buffers, cl::NDRange(a.size()));

    // Après l'exécution du kernel
    cl::CommandQueue queue = ComputeShader::getQueue();
    queue.enqueueReadBuffer(bufferResult, CL_TRUE, 0, sizeof(int) * result.size(), result.data());

    // Affichage du résultat
    for (int i = 0; i < 5; i++) {
        std::cout<< "result[" << i << "] = " << result[i] << std::endl;
    }
}
void test2() {
    ComputeShader::init("shaders/compute/computeShader.cl");    // Création des buffers OpenCL
    cl::Context context = ComputeShader::getContext();
    Vec3 v1(1.0f, 2.0f, 3.0f);
    Vec3 v2(4.0f, 5.0f, 6.0f);
    std::vector<Vec3> positions = {v1, v2};
    std::vector<Vec3> accelerations(positions.size(), Vec3(0.0f, 0.0f, 0.0f));
    
    cl::Buffer bufferA = ComputeShader::Buffer(positions, Permissions::Read);
    cl::Buffer bufferB = ComputeShader::Buffer(accelerations, Permissions::Write);
    std::vector<cl::Buffer*> buffers = {&bufferA, &bufferB};
    ComputeShader::launch("testVec3", buffers, cl::NDRange(positions.size()));
    cl::CommandQueue queue = ComputeShader::getQueue();
    queue.enqueueReadBuffer(bufferB, CL_TRUE, 0, sizeof(Vec3) * accelerations.size(), accelerations.data());

    // Affichage du résultat
    for (int i = 0; i < 2; i++) {
        std::cout<< "accelerations[" << i << "] = " << accelerations[i] << std::endl;
    }
}
void test3() {
    ComputeShader::init("shaders/compute/computeShader.cl");    // Création des buffers OpenCL
    cl::Context context = ComputeShader::getContext();
    Particles particles;
    OctreePtr octree = std::make_shared<Octree>(Vec3(0.0f, 0.0f, 0.0f), 3.0f);
    for (int i = 0; i < 2; ++i) {
        ParticlePtr particle = std::make_shared<Particle>(1.0f, Vec3(i, 0, 0));
        particles.push_back(particle);
    }
    octree->fillOctree(particles);
    octree->printOctree();
    octree->updateMassCenter();
    unsigned int index = 0;
    std::vector<GPUOctreePtr> flattenedOctree = octree->getFlattenedOctree(index);
    unsigned int sizeOctree = flattenedOctree.size();
    float softening = 1e-4f;
    float G = 6.67430e-11f; // Gravitational constant

    std::vector<Vec3> positions;
    std::vector<float> masses;
    std::vector<Vec3> accelerations;
    for (auto& particle : particles){
            positions.push_back(particle->pos);
            masses.push_back(particle->mass);
            accelerations.push_back(Vec3(0.0f, 0.0f, 0.0f));
    }
    
    cl::Buffer bufferPositions = ComputeShader::Buffer(positions, Permissions::Read);
    cl::Buffer bufferMasses = ComputeShader::Buffer(masses, Permissions::Read);
    cl::Buffer bufferAccelerations = ComputeShader::Buffer(accelerations, Permissions::Write);
    cl::Buffer bufferOctree = ComputeShader::Buffer(flattenedOctree, Permissions::Read);
    cl::Buffer bufferSizeOctree = ComputeShader::Buffer(&sizeOctree, Permissions::Read);
    cl::Buffer bufferSoftening = ComputeShader::Buffer(&softening, Permissions::Read);
    cl::Buffer bufferG = ComputeShader::Buffer(&G, Permissions::Read);

    std::vector<cl::Buffer*> buffers = {&bufferPositions, &bufferMasses, &bufferAccelerations, &bufferOctree, &bufferSizeOctree, &bufferSoftening, &bufferG};
    ComputeShader::launch("accelerations", buffers, cl::NDRange(positions.size()));
    cl::CommandQueue queue = ComputeShader::getQueue();
    queue.enqueueReadBuffer(bufferAccelerations, CL_TRUE, 0, sizeof(Vec3) * accelerations.size(), accelerations.data());
    for (int i = 0; i < 2; i++) {
        std::cout<< "accelerations[" << i << "] = " << accelerations[i] << std::endl;
    }
}


int main() {
    std::cout << sizeof(Vec3) << std::endl;
    size_t nbParticles =3000;
    scalar_t mass = 100.0f;
    scalar_t radius = 100.0f;
    scalar_t thickness = 1.0f;
    scalar_t starSpeed = 1.0f;
    //GalaxyPtr galaxy = std::make_shared<Galaxy>(1000, 1000.0f, 5.0f);

    WindowPtr window = std::make_shared<Window>();

    test3();

    std::vector<cl::Device> devices = ComputeShader::getListDevices();
    for (auto& device : devices) {
        std::cout << "Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    }

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