#ifndef POINTRENDERER_HPP
#define POINTRENDERER_HPP

#include "glad.h"
#include "Camera.hpp"
#include <iostream>

// Classe pour gérer l'affichage de points avec caméra 3D
class PointRenderer {
private:
    unsigned int shaderProgram;
    unsigned int VAO, VBO;
    CameraPtr camera;
    
    // Shaders pour afficher un point avec matrices 3D
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

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 color;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";

    // Fonction pour compiler un shader
    unsigned int compileShader(const char* source, GLenum type) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "Erreur de compilation du shader : " << infoLog << std::endl;
        }
        
        return shader;
    }

public:
    PointRenderer() : camera() {
        // Compiler les shaders
        unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
        unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
        
        // Créer le programme de shader
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cerr << "Erreur de liaison du programme de shader : " << infoLog << std::endl;
        }
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Créer et configurer le VAO et VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    ~PointRenderer() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }

    // Fonction pour afficher un point 3D avec coordonnées et couleur
    void drawPoint(float x, float y, float z, float r, float g, float b) {
        // Coordonnées du point
        float pointVertices[] = { x, y, z };
        
        // Mettre à jour le buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertices), pointVertices, GL_DYNAMIC_DRAW);
        
        // Utiliser le programme de shader
        glUseProgram(shaderProgram);
        
        // Matrices de transformation
        Mat4 model; // Matrice identité (pas de transformation de l'objet)
        Mat4 view = camera->getViewMatrix();
        Mat4 projection = camera->getProjectionMatrix();
        
        // Passer les matrices au shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection.m);
        
        // Passer la couleur
        glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b);
        
        // Dessiner le point
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 1);
    }

    CameraPtr getCamera() { return camera; }

    void setCamera(CameraPtr camera) { this->camera = camera; }
};

using PointRendererPtr = std::shared_ptr<PointRenderer>;
using PointRendererWeakPtr = std::weak_ptr<PointRenderer>;

#endif