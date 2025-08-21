#ifndef WINDOW_HPP
#define WINDOW_HPP
#include "forward.hpp"
#include "types.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "GalaxyFactory.hpp"
#include <thread>
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
    static WindowPtr instance;
    GLFWwindow* window;
    SimulationPtr simulation;
    RendererPtr renderer;
    InputPtr input;
    GalaxyFactoryPtr galaxyFactory;

    Cameras cameras;
    unsigned int indexCamera;

    Window();
    //Windows(SimulationPtr simulation);
    ~Window();

public:
    static WindowPtr getInstance();


    static GLFWwindow* getWindow(){return Window::getInstance()->window;}

    static CameraPtr getCamera();

        // create a galaxy
    static void createGalaxy(size_t nbParticles, scalar_t mass_,scalar_t radius, scalar_t thickness, scalar_t starSpeed=0);

    // run the display
    static void run();

    // Print the simulation
    static void printSimulation();
};

#include "Input.hpp"
using WindowPtr = std::shared_ptr<Window>;
using WindowWeakPtr = std::weak_ptr<Window>;









#endif