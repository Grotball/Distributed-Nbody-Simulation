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
    float* getPos();
    float* getViewMatrix();
    float* getProjectionMatrix();
    void rotate(const Quaternion& q, const bool recomputeViewMatrix=true);
    void translate(float dx, float dy, float dz, const bool recomputeViewMatrix=true);
};

void computeProjectionMatrix(float (&projection)[16], const float fov, const float aspectRatio, const float nearClip, const float farClip);

void computeViewMatrix(float (&viewMatrix)[16], const float (&pos)[3], const Quaternion& rot);