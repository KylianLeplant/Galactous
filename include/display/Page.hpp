#ifndef PAGE_HPP
#define PAGE_HPP

#include "PointRenderer.hpp"
#include "BloomRenderer.hpp"
#include "Window.hpp"
#include "Simulation.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Input.hpp"
#include "GalaxyFactory.hpp"

class Page {
    private:
    WindowWeakPtr window;
    SimulationPtr simulation;
    PointRendererPtr pointRenderer;
    BloomRendererPtr bloomRenderer;
    InputPtr input;
    GalaxyFactoryPtr galaxyFactory;

    Cameras cameras;

    public:
    Page(WindowWeakPtr window);
    Page(WindowWeakPtr window, SimulationPtr simulation);
    ~Page();

    // create a galaxy
    void createGalaxy(size_t nbParticles, scalar_t mass_, scalar_t radius, scalar_t thickness, scalar_t starSpeed=0);

    // run the display
    void run();

    // Print the simulation
    void printSimulation();
};

#endif