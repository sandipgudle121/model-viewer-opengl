#include "Camera.h"

vmath::vec3 Camera::GetPosition() const
{
    vmath::vec3 pos;
    float p = vmath::radians(Pitch);
    float y = vmath::radians(Yaw);

    pos[0] = Target[0] + Radius * cosf(p) * sinf(y);
    pos[1] = Target[1] + Radius * sinf(p);
    pos[2] = Target[2] + Radius * cosf(p) * cosf(y);
    return pos;
}

vmath::mat4 Camera::GetViewMatrix() const
{
    // Calculate actual camera position from spherical coordinates
    vmath::vec3 pos = GetPosition();
    return vmath::lookat(pos, Target, Up);
}
