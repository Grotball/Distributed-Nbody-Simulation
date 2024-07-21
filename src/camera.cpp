#include "camera.h"
#include <cmath>
#include <cstring>

Camera::Camera(const float x, const float y, const float z, const float fov, const float nearClip, const float farClip, const int xRes, const int yRes) : 
    pos {x, y, z}, 
    fov(fov), 
    nearClip(nearClip), 
    farClip(farClip), 
    rot{1.0f, 0.0f, 0.0f, 0.0f}
{
    aspectRatio = static_cast<float>(xRes) / yRes;
}