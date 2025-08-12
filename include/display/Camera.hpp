#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Mat4.hpp"
#include <iostream>
#include <memory>
#include "types.hpp"
#include <vector>

// class managing the camera of the 3D view.
class Camera {
private:
    Vec3 pos;     
    Vec3 target;    // target position
    Vec3 up;        // up vector
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;


public:
    //constructors
    Camera();

    Camera(Vec3 pos, Vec3 target);

    // get the view matrix
    Mat4 getViewMatrix() { return Mat4::lookAt(pos.x, pos.y, pos.z, target.x, target.y, target.z, up.x, up.y, up.z); }

    // get the projection matrix
    Mat4 getProjectionMatrix() { return Mat4::perspective(fov, aspectRatio, nearPlane, farPlane); }

    // move the camera
    void setPosition(float x, float y, float z) { pos = {x, y, z}; }


    // rotate the camera around the origin
    void orbit(float angleX, float angleY);

    // Zoom
    void zoom(float factor);

    // rotate around the target (used by Input)
    void turnAroundTarget(float xoffset, float yoffset);

    void goForward(float distance);

    void goBackward(float distance);

    void goLeft(float distance);

    void goRight(float distance);

    // Get the direction from the camera to the target
    Vec3 getDirection();

    // Set the target position
    void setTarget(float x, float y, float z);

};

using CameraPtr = std::shared_ptr<Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;
using Cameras = std::vector<CameraPtr>;

#endif