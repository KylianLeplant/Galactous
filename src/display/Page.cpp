#include "Page.hpp"


Page::Page(WindowWeakPtr window) : window(window), cameras() {
    CameraPtr camera = std::make_shared<Camera>();
    cameras.push_back(camera);
    pointRenderer = std::make_shared<PointRenderer>();
    pointRenderer->setCamera(camera);
    bloomRenderer = std::make_shared<BloomRenderer>();
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
    bloomRenderer = std::make_shared<BloomRenderer>();
    input = std::make_shared<Input>(window, camera);
    input->activate();
    galaxyFactory = std::make_shared<GalaxyFactory>(simulation);
}

Page::~Page(){
    input->deactivate();
}

void Page::run(){
    int fps = 0;
    int frameCount = 0;
    double lastFpsTime = glfwGetTime();
    std::thread threadSimulation([this]() { simulation->run(true); });

    GLFWwindow* win = window.lock()->getWindow();
    int fbW = 0, fbH = 0;

    while (!glfwWindowShouldClose(win)) {
        // Taille framebuffer courante (peut changer si l'utilisateur resize)
        glfwGetFramebufferSize(win, &fbW, &fbH);
        bloomRenderer->resize(fbW, fbH);

        // 1) Rendu de la scène (particules) dans le FBO HDR du bloom
        bloomRenderer->beginScene();
        glEnable(GL_DEPTH_TEST);
        printSimulation();

        // 2) Bright pass + flou gaussien itératif
        bloomRenderer->renderBloom();

        // 3) Compositage final + tone mapping dans le framebuffer par défaut
        bloomRenderer->renderToScreen();

        glfwPollEvents();
        if (input->isKeyPressed(Input::KEY_ESCAPE)) {
            glfwSetWindowShouldClose(win, GLFW_TRUE);
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
            ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoFocusOnAppearing;
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
        ImGui::Begin("##FPSOverlay", nullptr, overlayFlags);
        // Couleur selon le framerate (vert / jaune / rouge)
        ImVec4 fpsColor = fps >= 50 ? ImVec4(0.3f, 1.0f, 0.3f, 1.0f)
                       : fps >= 30 ? ImVec4(1.0f, 1.0f, 0.3f, 1.0f)
                                    : ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
        ImGui::TextColored(fpsColor, "FPS: %d", fps);
        ImGui::TextDisabled("%.1f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::End();

        // Panneau de contrôle principal (docké à gauche, collapsible)
        ImGui::SetNextWindowPos(ImVec2(10.0f, 60.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(260.0f, 0.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Galactous##ControlPanel", nullptr, ImGuiWindowFlags_NoCollapse);

        if (ImGui::CollapsingHeader("Statistiques", ImGuiTreeNodeFlags_DefaultOpen)) {
            size_t totalParticles = 0;
            for (auto& galaxy : simulation->galaxies) totalParticles += galaxy->particles.size();
            ImGui::Text("Galaxies:      %d", (int)simulation->galaxies.size());
            ImGui::Text("Particules:    %d", (int)totalParticles);
            ImGui::Text("FPS:           %d", fps);
        }

        if (ImGui::CollapsingHeader("Simulation", ImGuiTreeNodeFlags_DefaultOpen)) {
            float ts = simulation->time_step;
            if (ImGui::SliderFloat("Pas de temps", &ts, 0.0f, 50.0f, "%.2f"))
                simulation->time_step = ts;
            float th = simulation->theta;
            if (ImGui::SliderFloat("Theta (Barnes-Hut)", &th, 0.1f, 2.0f, "%.2f"))
                simulation->theta = th;
            float soft = simulation->softening;
            if (ImGui::SliderFloat("Softening", &soft, 0.0f, 1.0f, "%.3f"))
                simulation->softening = soft;
        }

        if (ImGui::CollapsingHeader("Affichage", ImGuiTreeNodeFlags_DefaultOpen)) {
            float ps = pointRenderer->getPointSize();
            if (ImGui::SliderFloat("Taille points", &ps, 1.0f, 20.0f, "%.1f"))
                pointRenderer->setPointSize(ps);
        }

        if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Seuil", &bloomRenderer->threshold, 0.0f, 2.0f, "%.2f");
            ImGui::SliderFloat("Intensité", &bloomRenderer->intensity, 0.0f, 3.0f, "%.2f");
            ImGui::SliderInt("Passes de flou", &bloomRenderer->blurPasses, 1, 10);
        }

        if (ImGui::CollapsingHeader("Contrôles")) {
            ImGui::TextDisabled("Z/S    avancer / reculer");
            ImGui::TextDisabled("Q/D    gauche / droite");
            ImGui::TextDisabled("Souris-G    rotation orbitale");
            ImGui::TextDisabled("Molette   zoom");
            ImGui::TextDisabled("Échap quitter");
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(win);
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