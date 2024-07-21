#pragma once
#include "quaternion.h"


class Camera
{
    float pos[3];
    float projectionMatrix[16];
    float viewMatrix[16];
    float fov;
    float nearClip;
    float farClip;
    float aspectRatio;

    Quaternion rot;
    public:

    
    Camera(const float x, const float y, const float z, const float fov, const float nearClip, const float farClip, const int xRes, const int yRes);
};