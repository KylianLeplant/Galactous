#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Mat4.hpp"

// Classe Camera pour gérer la vue 3D
class Camera {
private:
    float posX, posY, posZ;
    float targetX, targetY, targetZ;
    float upX, upY, upZ;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

public:
    Camera() : 
        posX(0.0f), posY(0.0f), posZ(5.0f),    // Caméra à 5 unités de l'origine
        targetX(0.0f), targetY(0.0f), targetZ(0.0f),      // Regarde vers l'origine
        upX(0.0f), upY(1.0f), upZ(0.0f),          // Vecteur "haut"
        fov(45.0f),                    // Champ de vision
        aspectRatio(800.0f/600.0f),    // Ratio largeur/hauteur
        nearPlane(0.1f),               // Plan proche
        farPlane(100.0f)               // Plan lointain
    {}

    // Obtenir la matrice de vue (View Matrix)
    Mat4 getViewMatrix() {
        return Mat4::lookAt(posX, posY, posZ, targetX, targetY, targetZ, upX, upY, upZ);
    }

    // Obtenir la matrice de projection (Projection Matrix)
    Mat4 getProjectionMatrix() {
        return Mat4::perspective(fov, aspectRatio, nearPlane, farPlane);
    }

    // Déplacer la caméra
    void setPosition(float x, float y, float z) {
        posX = x; posY = y; posZ = z;
    }

    // Faire tourner la caméra autour de l'origine
    void orbit(float angleX, float angleY) {
        float radius = sqrt(posX*posX + posY*posY + posZ*posZ);
        posX = radius * cos(angleY) * cos(angleX);
        posY = radius * sin(angleY);
        posZ = radius * cos(angleY) * sin(angleX);
    }

    // Zoom
    void zoom(float factor) {
        posX *= factor; posY *= factor; posZ *= factor;
    }
};

#endif