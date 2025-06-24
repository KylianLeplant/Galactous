#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>
#include <exception>
#include <memory>

// Configuration ImGui - doit être inclus avant imgui.h
#include "imconfig.h"

// ImGui doit être inclus après GLAD et GLFW
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class Window {
    private:
        GLFWwindow* window;
    public:
        Window();
        ~Window();
        void run();
        GLFWwindow* getWindow(){return this->window;}
};

using WindowPtr = std::shared_ptr<Window>;
using WindowWeakPtr = std::weak_ptr<Window>;









#endif