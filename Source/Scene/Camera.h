#pragma once

#include "../vmath.h"

class Camera
{
public:
    vmath::vec3 Position;
    vmath::vec3 Target;
    vmath::vec3 Up;

    float Radius;
    float Yaw;
    float Pitch;

    Camera() : 
        Position(vmath::vec3(0.0f, 0.0f, 5.0f)), 
        Target(vmath::vec3(0.0f, 0.0f, 0.0f)), 
        Up(vmath::vec3(0.0f, 1.0f, 0.0f)),
        Radius(5.0f),
        Yaw(0.0f),
        Pitch(0.0f) {}

    vmath::mat4 GetViewMatrix() const;
};