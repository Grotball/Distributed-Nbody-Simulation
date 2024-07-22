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

    computeProjectionMatrix(projectionMatrix, fov, aspectRatio, nearClip, farClip);
}

void computeProjectionMatrix(float projection[16], const float fov, const float aspectRatio, const float nearClip, const float farClip)
{
    float t = 1.0f / std::tan(0.5f * fov);
    // Matrix uses coloumn major ordering.
    float A[] = {
        t, 0, 0, 0,
        0, aspectRatio * t, 0, 0,
        0, 0, -(nearClip + farClip) / (farClip - nearClip), -1,
        0, 0, -2 * nearClip * farClip / (farClip - nearClip), 0
    };
    
    std::memcpy(projection, A, 16 * sizeof(float));
}