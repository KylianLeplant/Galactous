#include "Window.hpp"

Window::Window() {
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
    if (!gladLoadGL()) {
        throw std::runtime_error("Error GLAD init");
    }
    std::cout << "OpenGL prêt ! Version : " << glGetString(GL_VERSION) << std::endl;

    // Initialiser ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


Window::~Window() {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

/*void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse)
        return;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Mouse left button pressed" << std::endl;
    }
}*/