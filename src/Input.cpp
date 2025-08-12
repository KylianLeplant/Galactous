#include "Input.hpp"


void Input::activate() {
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

void Input::deactivate() {
    glfwSetWindowUserPointer(window, nullptr);
    glfwSetCursorPosCallback(window, nullptr);
    glfwSetMouseButtonCallback(window, nullptr);
    glfwSetKeyCallback(window, nullptr);
    glfwSetScrollCallback(window, nullptr);
}

void Input::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (ImGui::GetIO().WantCaptureMouse) return;
    camera->zoom(yoffset);
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
    } 
    else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
    }
}