#ifndef INPUT_HPP
#define INPUT_HPP

#include <GLFW/glfw3.h>
#include <functional>
#include <map>
#include <memory>
#include <iostream>
#include "Camera.hpp"
#include "Window.hpp"

// Classe pour gérer les inputs (souris, clavier)
class Input {
private:
    GLFWwindow* window;  // Pointeur brut car GLFWwindow n'est pas géré par nos smart pointers
    CameraPtr camera;
    
    // Variables pour la souris
    double lastMouseX, lastMouseY;
    bool firstMouse = true;
    bool mouseLeftPressed = false;
    bool mouseRightPressed = false;
    bool mouseMiddlePressed = false;
    
    // Variables pour le clavier
    std::map<int, bool> keyStates;
    GLFWcursorposfun oldCursorPosCallback = nullptr;
    GLFWmousebuttonfun oldMouseButtonCallback = nullptr;
    GLFWkeyfun oldKeyCallback = nullptr;
    GLFWscrollfun oldScrollCallback = nullptr;
    

public:
    Input(WindowWeakPtr win, CameraPtr camera) : window(win.lock()->getWindow()), camera(camera) {
        lastMouseX = lastMouseY = 0.0;
        
    }

    void enable() {
        // Configurer les callbacks GLFW
        glfwSetWindowUserPointer(window, this);
        
        oldCursorPosCallback = glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
            auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
            if (self->oldCursorPosCallback) self->oldCursorPosCallback(w, x, y); // Appelle ImGui
            self->mouse_callback(w, x, y);
        });

        oldMouseButtonCallback = glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
            auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
            if (self->oldMouseButtonCallback) self->oldMouseButtonCallback(w, button, action, mods); // Appelle ImGui
            self->mouse_button_callback(w, button, action, mods);
        });

        oldKeyCallback = glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
            auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
            if (self->oldKeyCallback) self->oldKeyCallback(w, key, scancode, action, mods); // Appelle ImGui
            self->key_callback(w, key, scancode, action, mods);
        });

        oldScrollCallback = glfwSetScrollCallback(window, [](GLFWwindow* w, double xoffset, double yoffset) {
            auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
            if (self->oldScrollCallback) self->oldScrollCallback(w, xoffset, yoffset); // Appelle ImGui
            self->scroll_callback(w, xoffset, yoffset);
        });

    }

    void disable() {
        glfwSetWindowUserPointer(window, nullptr);
        glfwSetCursorPosCallback(window, nullptr);
        glfwSetMouseButtonCallback(window, nullptr);
        glfwSetKeyCallback(window, nullptr);
    }

    // Callbacks internes (appelés par GLFW)
    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        if (ImGui::GetIO().WantCaptureMouse) return;
        if (firstMouse) {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;
        }

        double xoffset = xpos - lastMouseX;
        double yoffset = lastMouseY - ypos; 

        lastMouseX = xpos;
        lastMouseY = ypos;

        if (mouseLeftPressed) {
            camera->turnAroundTarget(xoffset, yoffset);
        } 
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        if (ImGui::GetIO().WantCaptureMouse) return;
        camera->zoom(yoffset);
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (ImGui::GetIO().WantCaptureMouse) return;

        if (action == GLFW_PRESS) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                mouseLeftPressed = true;
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                mouseRightPressed = true;
            } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                mouseMiddlePressed = true;
            }
        } else if (action == GLFW_RELEASE) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                mouseLeftPressed = false;
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                mouseRightPressed = false;
            } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                mouseMiddlePressed = false;
            }
        }
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (ImGui::GetIO().WantCaptureKeyboard) return;
        if (action == GLFW_PRESS) {
            keyStates[key] = true;
            if (key == KEY_Z) {
                camera->goForward(0.1f);
            } else if (key == KEY_S) {
                camera->goBackward(0.1f);
            } else if (key == KEY_Q) {
                camera->goLeft(0.1f);
            } else if (key == KEY_D) {
                camera->goRight(0.1f);
            }
        } else if (action == GLFW_RELEASE) {
            keyStates[key] = false;
        }

    }


    // Méthodes utilitaires
    bool isKeyPressed(int key) const {
        return keyStates.find(key) != keyStates.end() && keyStates.at(key);
    }

    bool isMouseLeftPressed() const {
        return mouseLeftPressed;
    }

    bool isMouseRightPressed() const {
        return mouseRightPressed;
    }

    bool isMouseMiddlePressed() const {
        return mouseMiddlePressed;
    }
    void getMousePosition(double& x, double& y) const {
        glfwGetCursorPos(window, &x, &y);
    }

    // Constantes pour les touches (pour faciliter l'utilisation)
    static const int KEY_Z = GLFW_KEY_Z;
    static const int KEY_S = GLFW_KEY_S;
    static const int KEY_Q = GLFW_KEY_Q;
    static const int KEY_D = GLFW_KEY_D;
    static const int KEY_SPACE = GLFW_KEY_SPACE;
    static const int KEY_ESCAPE = GLFW_KEY_ESCAPE;
    
    static const int MOUSE_LEFT = GLFW_MOUSE_BUTTON_LEFT;
    static const int MOUSE_RIGHT = GLFW_MOUSE_BUTTON_RIGHT;
    static const int MOUSE_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE;
    
    static const int ACTION_PRESS = GLFW_PRESS;
    static const int ACTION_RELEASE = GLFW_RELEASE;
};


using InputPtr = std::shared_ptr<Input>;
using InputWeakPtr = std::weak_ptr<Input>;

#endif 