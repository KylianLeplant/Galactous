#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Mat4.hpp"
#include <iostream>
#include <memory>
#include "types.hpp"
#include <vector>

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
        posX(0.0f), posY(250.0f), posZ(50.0f),    // Caméra à 20 unités de l'origine (au lieu de 5)
        targetX(0.0f), targetY(0.0f), targetZ(0.0f),      // Regarde vers l'origine
        upX(0.0f), upY(1.0f), upZ(0.0f),          // Vecteur "haut"
        fov(45.0f),                    // Champ de vision
        aspectRatio(800.0f/600.0f),    // Ratio largeur/hauteur
        nearPlane(0.1f),               // Plan proche
        farPlane(10000.0f)               // Plan lointain
    {}

    Camera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ) : 
        posX(posX), posY(posY), posZ(posZ),
        targetX(targetX), targetY(targetY), targetZ(targetZ),
        upX(0.0f), upY(1.0f), upZ(0.0f),
        fov(45.0f),
        aspectRatio(800.0f/600.0f),
        nearPlane(0.1f),
        farPlane(10000.0f)
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
        fov -= factor;
        if (fov < 1.0f) fov = 1.0f;
        if (fov > 179.0f) fov = 179.0f;
    }

    // Tourner autour de la cible (utilisé par Input)
    void turnAroundTarget(float xoffset, float yoffset) {
        // Convertir les offsets en angles (sensibilité en radians)
        float sensitivity = 0.1f;  // Sensibilité augmentée (était 0.01f)
        float yaw = xoffset * sensitivity;
        float pitch = yoffset * sensitivity;


        // Calculer la distance actuelle de la caméra à la cible
        float dx = posX - targetX;
        float dy = posY - targetY;
        float dz = posZ - targetZ;
        float distance = sqrt(dx*dx + dy*dy + dz*dz);

        // Calculer les angles actuels (en radians)
        float currentYaw = atan2(dz, dx);
        float currentPitch = asin(dy / distance);


        // Ajouter les nouveaux angles
        float newYaw = currentYaw + yaw;
        float newPitch = currentPitch - pitch;

        // Limiter le pitch pour éviter de passer au-dessus/au-dessous de la cible
        // π/2 ≈ 1.57 radians = 90 degrés
        if (newPitch > 1.5f) newPitch = 1.5f;  // Limite à ~85 degrés
        if (newPitch < -1.5f) newPitch = -1.5f; // Limite à ~-85 degrés

        // Calculer la nouvelle position (tous les angles en radians)
        posX = targetX + distance * cos(newPitch) * cos(newYaw);
        posY = targetY + distance * sin(newPitch);
        posZ = targetZ + distance * cos(newPitch) * sin(newYaw);
    }

    void goForward(float distance) {
        Vec3 direction = getDirection();
        scalar_t coeff = 10;
        if (distance > 10) coeff = distance;
        posX += distance * direction.x * coeff;
        posY += distance * direction.y * coeff;
        posZ += distance * direction.z * coeff;
        targetX += distance * direction.x * coeff;
        targetY += distance * direction.y * coeff;
        targetZ += distance * direction.z * coeff;
    }

    void goBackward(float distance) {
        Vec3 direction = getDirection();
        scalar_t coeff = 10;
        if (distance > 10) coeff = distance;
        posX -= distance * direction.x * coeff;
        posY -= distance * direction.y * coeff;
        posZ -= distance * direction.z * coeff;
        targetX -= distance * direction.x * coeff;
        targetY -= distance * direction.y * coeff;
        targetZ -= distance * direction.z * coeff;
    }

    void goLeft(float distance) {
        Vec3 direction = getDirection();
        scalar_t coeff = 10;
        if (distance > 10) coeff = distance;
        posX -= distance * direction.z * coeff;
        posZ += distance * direction.x * coeff;
    }

    void goRight(float distance) {
        Vec3 direction = getDirection();
        scalar_t coeff = 10;
        if (distance > 10) coeff = distance;
        posX += distance * direction.z * coeff;
        posZ -= distance * direction.x * coeff;
    }

    Vec3 getDirection() {
        return Vec3(targetX - posX, targetY - posY, targetZ - posZ);
    }  

    void setTarget(float x, float y, float z) {
        targetX = x;
        targetY = y;
        targetZ = z;
    }



};

using CameraPtr = std::shared_ptr<Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;
using Cameras = std::vector<CameraPtr>;

#endif