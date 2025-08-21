#include "Window.hpp"
#include "Input.hpp"
WindowPtr Window::instance = nullptr;

Window::Window() {
    indexCamera = 0;
    // Initialiser GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Error GLFW init");
    }


    // Version OpenGL souhaitée : 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Créer une fenêtre
    this->window = glfwCreateWindow(800, 600, "Galactous", nullptr, nullptr);
    if (!this->window) {
        glfwTerminate();
        throw std::runtime_error("Error GLFW window creation");
    }

    // Faire de cette fenêtre le contexte courant
    glfwMakeContextCurrent(this->window);

    // Charger les fonctions OpenGL avec GLAD
    if (!gladLoadGL()) {  std::runtime_error("Error GLAD init");
    }
    std::cout << "OpenGL prêt ! Version : " << glGetString(GL_VERSION) << std::endl;

    // Initialiser ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    CameraPtr camera = std::make_shared<Camera>();
    cameras.push_back(camera);
    renderer = std::make_shared<Renderer>();
    renderer->setCamera(camera);
    input = std::make_shared<Input>(window, camera);
    input->activate();
    simulation = std::make_shared<Simulation>();
    galaxyFactory = std::make_shared<GalaxyFactory>(simulation);
}


Window::~Window() {
    input->deactivate();
    glfwDestroyWindow(this->window);
    glfwTerminate();
}


WindowPtr Window::getInstance() {
    if (!instance) {
        instance = std::make_shared<Window>();
    }
    return instance;
}

void Window::run(){
    float f = 0.0f;
    int counter = 0;
    std::cout << "window.lock()->getWindow() = " << Window::getWindow() << std::endl;
    std::thread threadSimulation([]() { Window::getInstance()->simulation->run(true); });
    std::cout << "after thread" << std::endl;
    while (!glfwWindowShouldClose(Window::getInstance()->getWindow())) {
        std::cout << 36 << std::endl;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        std::cout << 38 << std::endl;
        
        // Activer le test de profondeur
        glEnable(GL_DEPTH_TEST);
        std::cout << 40 << std::endl;
        printSimulation();
        std::cout << 42 << std::endl;
        // Rendu ImGui
        glfwPollEvents();
        std::cout << 44 << std::endl;
        if (Window::getInstance()->input->isKeyPressed(Input::KEY_ESCAPE)) {
            glfwSetWindowShouldClose(Window::getInstance()->getWindow(), GLFW_TRUE);
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

        glfwSwapBuffers(Window::getInstance()->getWindow());
    }
}

void Window::printSimulation(){
    for(auto galaxy : Window::getInstance()->simulation->galaxies){
        for(auto particle : galaxy->particles){
            Vec3& position = particle->pos;
            Window::getInstance()->renderer->drawPoint(position.x, position.y, position.z, 1.0, 1.0, 1.0);
        }
    }
}

void Window::createGalaxy(size_t nbParticles, scalar_t mass_,scalar_t radius, scalar_t thickness, scalar_t starSpeed){
    GalaxyPtr galaxy = std::make_shared<Galaxy>(Window::getInstance()->galaxyFactory->generateGalaxy(nbParticles, mass_, radius, thickness, starSpeed));
    Window::getInstance()->simulation->galaxies.push_back(galaxy);
    Window::getInstance()->simulation->octreeRoot->fillOctree(galaxy->particles);
    Window::getInstance()->simulation->octreeRoot->updateMassCenter();
}


