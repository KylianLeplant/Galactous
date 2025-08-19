#ifndef RENDERER_HPP
#define RENDERER_HPP

//#include "GL/glew.h"
#include "glad.h"
#include "Camera.hpp"
#include <iostream>

// Class managing the point rendering
class Renderer {
private:
    unsigned int shaderProgram;
    unsigned int VAO, VBO;
    CameraPtr camera;

    // manages the vertex shader source code
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform vec3 uColor;
        out vec3 color;
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            gl_PointSize = 10.0;
            color = uColor;
        }
    )";

    // manages the color of the point
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 color;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";

    // Fonction pour compiler un shader
    unsigned int compileShader(const char* source, GLenum type);

public:
    Renderer();

    ~Renderer();

    // Method to draw a 3D point with coordinates and color
    void drawPoint(float x, float y, float z, float r, float g, float b);

    CameraPtr getCamera() { return camera; }

    void setCamera(CameraPtr camera) { this->camera = camera; }
};

using RendererPtr = std::shared_ptr<Renderer>;
using RendererWeakPtr = std::weak_ptr<Renderer>;

#endif