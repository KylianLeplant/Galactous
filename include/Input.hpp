#ifndef INPUT_HPP
#define INPUT_HPP

#include <GLFW/glfw3.h>
#include <functional>
#include <map>

// Classe pour gérer les inputs (souris, clavier)
class Input {
private:
    GLFWwindow* window;
    
    // Variables pour la souris
    double lastMouseX, lastMouseY;
    bool firstMouse = true;
    bool mousePressed = false;
    
    // Variables pour le clavier
    std::map<int, bool> keyStates;
    
    // Callbacks pour les événements
    std::function<void(double, double)> mouseMoveCallback;
    std::function<void(int, int, int)> mouseClickCallback;
    std::function<void(int, int, int)> keyCallback;

public:
    Input(GLFWwindow* win) : window(win) {
        lastMouseX = lastMouseY = 0.0;
        
        // Configurer les callbacks GLFW
        glfwSetWindowUserPointer(window, this);
        
        glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
            static_cast<Input*>(glfwGetWindowUserPointer(w))->mouse_callback(w, x, y);
        });
        
        glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
            static_cast<Input*>(glfwGetWindowUserPointer(w))->mouse_button_callback(w, button, action, mods);
        });
        
        glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
            static_cast<Input*>(glfwGetWindowUserPointer(w))->key_callback(w, key, scancode, action, mods);
        });
    }

    // Callbacks internes (appelés par GLFW)
    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        if (firstMouse) {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;
        }

        double xoffset = xpos - lastMouseX;
        double yoffset = lastMouseY - ypos; // Inversé car l'axe Y va de haut en bas

        lastMouseX = xpos;
        lastMouseY = ypos;

        if (mouseMoveCallback) {
            mouseMoveCallback(xoffset, yoffset);
        }
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
        } else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }

        if (mouseClickCallback) {
            mouseClickCallback(button, action, mods);
        }
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            keyStates[key] = true;
        } else if (action == GLFW_RELEASE) {
            keyStates[key] = false;
        }

        if (keyCallback) {
            keyCallback(key, action, mods);
        }
    }

    // Méthodes publiques pour configurer les callbacks
    void setMouseMoveCallback(std::function<void(double, double)> callback) {
        mouseMoveCallback = callback;
    }

    void setMouseClickCallback(std::function<void(int, int, int)> callback) {
        mouseClickCallback = callback;
    }

    void setKeyCallback(std::function<void(int, int, int)> callback) {
        keyCallback = callback;
    }

    // Méthodes utilitaires
    bool isKeyPressed(int key) const {
        return keyStates.find(key) != keyStates.end() && keyStates.at(key);
    }

    bool isMousePressed() const {
        return mousePressed;
    }

    void getMousePosition(double& x, double& y) const {
        glfwGetCursorPos(window, &x, &y);
    }

    // Constantes pour les touches (pour faciliter l'utilisation)
    static const int KEY_W = GLFW_KEY_W;
    static const int KEY_S = GLFW_KEY_S;
    static const int KEY_A = GLFW_KEY_A;
    static const int KEY_D = GLFW_KEY_D;
    static const int KEY_SPACE = GLFW_KEY_SPACE;
    static const int KEY_ESCAPE = GLFW_KEY_ESCAPE;
    
    static const int MOUSE_LEFT = GLFW_MOUSE_BUTTON_LEFT;
    static const int MOUSE_RIGHT = GLFW_MOUSE_BUTTON_RIGHT;
    static const int MOUSE_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE;
    
    static const int ACTION_PRESS = GLFW_PRESS;
    static const int ACTION_RELEASE = GLFW_RELEASE;
};

#endif 