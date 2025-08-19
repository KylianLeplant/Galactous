#include "Renderer.hpp"
//#include "dim/windows/Scene.hpp"
unsigned int Renderer::compileShader(const char* source, GLenum type) {
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


Renderer::Renderer() : camera() {
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

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void Renderer::drawPoint(float x, float y, float z, float r, float g, float b) {
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