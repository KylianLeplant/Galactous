#ifndef INPUT_HPP
#define INPUT_HPP

#include "glad.h"
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
    //constructor
    Input(WindowWeakPtr win, CameraPtr camera) : window(win.lock()->getWindow()), camera(camera) { lastMouseX = lastMouseY = 0.0; }

    //active the input
    void activate();

    // deactivate the input
    void deactivate();

    // Internal callbacks (called by GLFW)
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);

    // Scroll callback
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    // Mouse button callback
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    // Key callback
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // Méthodes utilitaires
    bool isKeyPressed(int key) const { return keyStates.find(key) != keyStates.end() && keyStates.at(key); }

    // Verify if the left mouse button is pressed
    bool isMouseLeftPressed() const { return mouseLeftPressed; }

    // Verify if the right mouse button is pressed
    bool isMouseRightPressed() const { return mouseRightPressed; }

    // Verify if the middle mouse button is pressed
    bool isMouseMiddlePressed() const { return mouseMiddlePressed; }

    // Get the current mouse position
    void getMousePosition(double& x, double& y) const { glfwGetCursorPos(window, &x, &y); }

    // Constants to facilitate key usage
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