#include "Simulation.hpp"
#include "PointRenderer.hpp"
#include "GalaxyFactory.hpp"
#include "Camera.hpp"
#include "Window.hpp"
#include "Input.hpp"
#include "Page.hpp"

int main() {
    size_t nbParticles = 100000;
    scalar_t mass = 100.0f;
    scalar_t radius = 100.0f;
    scalar_t thickness = 1.0f;
    scalar_t starSpeed = 1.0f;

    WindowPtr window = std::make_shared<Window>();

    Page page(window);
    page.createGalaxy(nbParticles, mass, radius, thickness, starSpeed);

    page.run();

    return 0;
}