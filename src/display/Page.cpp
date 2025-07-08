#include "Page.hpp"


Page::Page(WindowWeakPtr window) : window(window), cameras() {
    CameraPtr camera = std::make_shared<Camera>();
    cameras.push_back(camera);
    pointRenderer = std::make_shared<PointRenderer>();
    pointRenderer->setCamera(camera);
    input = std::make_shared<Input>(window, camera);
    input->enable();
    simulation = std::make_shared<Simulation>();
    galaxyFactory = std::make_shared<GalaxyFactory>(simulation);
}

Page::Page(WindowWeakPtr window, SimulationPtr simulation) : window(window), simulation(simulation), cameras() {
    CameraPtr camera = std::make_shared<Camera>();
    cameras.push_back(camera);
    pointRenderer = std::make_shared<PointRenderer>();
    pointRenderer->setCamera(camera);
    input = std::make_shared<Input>(window, camera);
    input->enable();
    galaxyFactory = std::make_shared<GalaxyFactory>(simulation);
}

//Page::Page(WindowWeakPtr window, SimulationPtr simulation, PointRendererPtr pointRenderer) : window(window), simulation(simulation), pointRenderer(pointRenderer), cameras() {
//    CameraPtr camera = std::make_shared<Camera>();
//    cameras.push_back(camera);
//    input = std::make_shared<Input>(window, camera);
//    galaxyFactory = std::make_shared<GalaxyFactory>(simulation);
//}


Page::~Page(){
    input->disable();
}

void Page::run(){
    float f = 0.0f;
    int counter = 0;
    int testCount = 0;
    std::cout << "window.lock()->getWindow() = " << window.lock()->getWindow() << std::endl;
    while (!glfwWindowShouldClose(window.lock()->getWindow())) {
        //testCount++;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activer le test de profondeur
        glEnable(GL_DEPTH_TEST);
        simulation->update();
        printSimulation();
        // Rendu ImGui
        glfwPollEvents();
        if (input->isKeyPressed(Input::KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window.lock()->getWindow(), GLFW_TRUE);
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
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.lock()->getWindow());
    }
}

void Page::printSimulation(){
    for(auto galaxy : simulation->galaxies){
        for(auto particle : galaxy->particles){
            Vec3& position = particle->pos;
            pointRenderer->drawPoint(position.x, position.y, position.z, 1.0, 1.0, 1.0);
        }
    }
}

void Page::createGalaxy(size_t nbParticles, scalar_t mass_,scalar_t radius, scalar_t thickness, scalar_t starSpeed){
    GalaxyPtr galaxy = std::make_shared<Galaxy>(galaxyFactory->generateGalaxy(nbParticles, mass_, radius, thickness, starSpeed));
    simulation->galaxies.push_back(galaxy);
    simulation->octreeRoot->fillOctree(galaxy->particles);
    simulation->octreeRoot->updateMassCenter();
}




