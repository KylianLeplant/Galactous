#ifndef POINTRENDERER_HPP
#define POINTRENDERER_HPP

#include "glad.h"
#include "Camera.hpp"
#include <iostream>
#include <vector>

// Classe pour gérer l'affichage de points avec caméra 3D (batch rendering)
class PointRenderer {
private:
    unsigned int shaderProgram;
    unsigned int VAO, VBO;
    CameraPtr camera;

    int locModel, locView, locProjection, locPointSize;

    float pointSize = 6.0f;

    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform float uPointSize;
        out vec3 vColor;
        void main() {
            vec4 viewPos = view * model * vec4(aPos, 1.0);
            gl_Position = projection * viewPos;
            // Taille décroît avec la distance (1/viewPos.z)
            gl_PointSize = uPointSize * (50.0 / max(-viewPos.z, 0.1));
            vColor = aColor;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 vColor;
        out vec4 FragColor;
        void main() {
            vec2 c = gl_PointCoord - vec2(0.5);
            float d = dot(c, c);
            if (d > 0.25) discard;
            float alpha = smoothstep(0.25, 0.18, d);
            FragColor = vec4(vColor * alpha, 1.0);
        }
    )";

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
        unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
        unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

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

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // Layout : vec3 aPos + vec3 aColor (stride 6 floats)
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        // Cache des locations d'uniforms
        locModel = glGetUniformLocation(shaderProgram, "model");
        locView = glGetUniformLocation(shaderProgram, "view");
        locProjection = glGetUniformLocation(shaderProgram, "projection");
        locPointSize = glGetUniformLocation(shaderProgram, "uPointSize");
    }

    ~PointRenderer() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }

    // Affiche un lot de points en un seul draw call.
    // vertices : tableau interleaved [px,py,pz, cr,cg,cb, px,py,pz, ...]
    void drawPoints(const std::vector<float>& vertices, size_t count) {
        if (count == 0) return;

        // Permet l'additivité (les étoiles s'éclairent mutuellement)
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_PROGRAM_POINT_SIZE);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(),
                     GL_DYNAMIC_DRAW);

        glUseProgram(shaderProgram);

        Mat4 model;
        Mat4 view = camera->getViewMatrix();
        Mat4 projection = camera->getProjectionMatrix();

        glUniformMatrix4fv(locModel, 1, GL_FALSE, model.m);
        glUniformMatrix4fv(locView, 1, GL_FALSE, view.m);
        glUniformMatrix4fv(locProjection, 1, GL_FALSE, projection.m);
        glUniform1f(locPointSize, pointSize);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, (GLsizei)count);

        // Restaure l'état pour ImGui (qui dessine ensuite dans la même frame)
        glDisable(GL_BLEND);
        glDisable(GL_PROGRAM_POINT_SIZE);
    }

    void setPointSize(float s) { pointSize = s; }
    float getPointSize() const { return pointSize; }

    CameraPtr getCamera() { return camera; }
    void setCamera(CameraPtr camera) { this->camera = camera; }
};

using PointRendererPtr = std::shared_ptr<PointRenderer>;
using PointRendererWeakPtr = std::weak_ptr<PointRenderer>;

#endif