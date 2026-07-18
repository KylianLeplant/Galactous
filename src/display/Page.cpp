#include "Page.hpp"


Page::Page(WindowWeakPtr window) : window(window), cameras() {
    CameraPtr camera = std::make_shared<Camera>();
    cameras.push_back(camera);
    pointRenderer = std::make_shared<PointRenderer>();
    pointRenderer->setCamera(camera);
    input = std::make_shared<Input>(window, camera);
    input->activate();
    simulation = std::make_shared<Simulation>();
    galaxyFactory = std::make_shared<GalaxyFactory>(simulation);
}

Page::Page(WindowWeakPtr window, SimulationPtr simulation) : window(window), simulation(simulation), cameras() {
    CameraPtr camera = std::make_shared<Camera>();
    cameras.push_back(camera);
    pointRenderer = std::make_shared<PointRenderer>();
    pointRenderer->setCamera(camera);
    input = std::make_shared<Input>(window, camera);
    input->activate();
    galaxyFactory = std::make_shared<GalaxyFactory>(simulation);
}

Page::~Page(){
    input->deactivate();
}

void Page::run(){
    float f = 0.0f;
    int counter = 0;
    int fps = 0;
    int frameCount = 0;
    double lastFpsTime = glfwGetTime();
    std::thread threadSimulation([this]() { simulation->run(true); });
    while (!glfwWindowShouldClose(window.lock()->getWindow())) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        printSimulation();

        glfwPollEvents();
        if (input->isKeyPressed(Input::KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window.lock()->getWindow(), GLFW_TRUE);
        }

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // FPS overlay en haut de l'écran, mis à jour chaque seconde
        frameCount++;
        double now = glfwGetTime();
        if (now - lastFpsTime >= 1.0) {
            fps = frameCount;
            frameCount = 0;
            lastFpsTime = now;
        }
        const ImGuiWindowFlags overlayFlags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings;
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(140.0f, 0.0f), ImGuiCond_Always);
        ImGui::Begin("##FPSOverlay", nullptr, overlayFlags);
        ImGui::Text("FPS: %d", fps);
        ImGui::End();

        ImGui::Begin("Galactous");
        ImGui::Text("Bienvenue dans Galactous !");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&glClearColor);
        if (ImGui::Button("Button")) {
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.lock()->getWindow());
    }
    simulation->stop();
    if (threadSimulation.joinable()) threadSimulation.join();
}

void Page::printSimulation(){
    // Construction d'un seul buffer interleaved : [px,py,pz, cr,cg,cb] x N
    std::vector<float> vertices;
    size_t total = 0;
    for (auto galaxy : simulation->galaxies) total += galaxy->particles.size();
    vertices.reserve(total * 6);

    for (auto galaxy : simulation->galaxies) {
        for (auto particle : galaxy->particles) {
            const Vec3& pos = particle->pos;
            // Couleur selon le type de particule
            float r, g, b;
            switch (particle->type) {
                case TypeParticle::STAR: {
                    // Bleu-violet vers jaune selon le module de vitesse (effet Doppler visuel)
                    scalar_t sp = particle->velocity.norm();
                    float t = sp > 1.0f ? 1.0f : sp; // normalise grossierement
                    r = 0.3f + 0.7f * t;
                    g = 0.4f + 0.6f * t;
                    b = 1.0f - 0.5f * t;
                    break;
                }
                case TypeParticle::DARK_MATTER:
                    r = 0.15f; g = 0.15f; b = 0.25f; // sombre, presque invisible
                    break;
                case TypeParticle::GAS:
                    r = 0.2f; g = 0.8f; b = 0.7f;  // cyan-gas
                    break;
                default:
                    r = g = b = 1.0f;
            }
            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);
        }
    }
    pointRenderer->drawPoints(vertices, total);
}

void Page::createGalaxy(size_t nbParticles, scalar_t mass_, scalar_t radius, scalar_t thickness, scalar_t starSpeed){
    GalaxyPtr galaxy = std::make_shared<Galaxy>(galaxyFactory->generateGalaxy(nbParticles, mass_, radius, thickness, starSpeed));
    simulation->galaxies.push_back(galaxy);
    simulation->octreeRoot->fillOctree(galaxy->particles);
    simulation->octreeRoot->updateMassCenter();
}