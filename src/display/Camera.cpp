#include "Camera.hpp"


Camera::Camera() : 
    pos({0.0f,250.0f,50.0f}),       // Camera's position
    target({0.0f,0.0f,0.0f}),       // Target point
    up({0.0f,1.0f,0.0f}),           // Up vector
    fov(45.0f),                     // Field of view
    aspectRatio(800.0f/600.0f),     // Aspect ratio
    nearPlane(0.1f),                // Near plane
    farPlane(10000.0f)              // Far plane
{}

Camera::Camera(Vec3 pos, Vec3 target) : 
    pos(pos),
    target(target),
    up({0.0f, 1.0f, 0.0f}),
    fov(45.0f),
    aspectRatio(800.0f/600.0f),
    nearPlane(0.1f),
    farPlane(10000.0f)
{}

void Camera::orbit(float angleX, float angleY) {
    float radius = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
    pos.x = radius * cos(angleY) * cos(angleX);
    pos.y = radius * sin(angleY);
    pos.z = radius * cos(angleY) * sin(angleX);
}

void Camera::zoom(float factor) {
    fov -= factor;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 179.0f) fov = 179.0f;
}

void Camera::turnAroundTarget(float xoffset, float yoffset) {
    // Convert offsets to angles (sensitivity in radians)
    float sensitivity = 0.1f;  // Increased sensitivity (was 0.01f)
    float yaw = xoffset * sensitivity;
    float pitch = yoffset * sensitivity;
    // Calculate the current distance from the camera to the target
    float dx = pos.x - target.x;
    float dy = pos.y - target.y;
    float dz = pos.z - target.z;
    float distance = sqrt(dx*dx + dy*dy + dz*dz);
    // Calculate the current angles (in radians)
    float currentYaw = atan2(dz, dx);
    float currentPitch = asin(dy / distance);
    // Add the new angles
    float newYaw = currentYaw + yaw;
    float newPitch = currentPitch - pitch;
    // Limit the pitch to avoid going above/below the target
    // π/2 ≈ 1.57 radians = 90 degrees
    if (newPitch > 1.5f) newPitch = 1.5f;  // Limit to ~85 degrees
    if (newPitch < -1.5f) newPitch = -1.5f; // Limit to ~-85 degrees
    // Calculate the new position (all angles in radians)
    pos = {
        static_cast<scalar_t>(target.x + distance * cos(newPitch) * cos(newYaw)),
        static_cast<scalar_t>(target.y + distance * sin(newPitch)),
        static_cast<scalar_t>(target.z + distance * cos(newPitch) * sin(newYaw))
    };
}

void Camera::goForward(float distance) {
    Vec3 direction = getDirection();
    scalar_t coeff = 10;
    if (distance > 10) coeff = distance;
    pos += direction * distance * coeff;
    target += direction * distance * coeff;
}

void Camera::goBackward(float distance) {
    Vec3 direction = getDirection();
    scalar_t coeff = 10;
    if (distance > 10) coeff = distance;
    pos -= direction * distance * coeff;
    target -= direction * distance * coeff;
}

void Camera::goLeft(float distance) {
    Vec3 direction = getDirection();
    scalar_t coeff = 10;
    if (distance > 10) coeff = distance;
    pos.x -= distance * direction.z * coeff;
    pos.z += distance * direction.x * coeff;
}

void Camera::goRight(float distance) {
    Vec3 direction = getDirection();
    scalar_t coeff = 10;
    if (distance > 10) coeff = distance;
    pos.x += distance * direction.z * coeff;
    pos.z -= distance * direction.x * coeff;
}

Vec3 Camera::getDirection() {
    return Vec3(target.x - pos.x, target.y - pos.y, target.z - pos.z);
}

void Camera::setTarget(float x, float y, float z) {
    target.x = x;
    target.y = y;
    target.z = z;
}