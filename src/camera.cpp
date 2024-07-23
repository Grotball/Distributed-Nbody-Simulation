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
    computeViewMatrix(viewMatrix, pos, conjugate(rot));
}

float* Camera::getPos()
{
    return pos;
}

float* Camera::getViewMatrix()
{
    return viewMatrix;
}

float* Camera::getProjectionMatrix()
{
    return projectionMatrix;
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

void computeViewMatrix(float viewMatrix[16], const float pos[3], const Quaternion& rot)
{
    computeQuaternionRotationMatrix(viewMatrix, rot);

    for (int i = 0; i < 3; i++)
    {
        viewMatrix[12 + i] = 0;
        for (int j = 0; j < 3; j++)
        {
            viewMatrix[12 + i] -= viewMatrix[i + 4 * j] * pos[j];
        }
    }

    viewMatrix[15] = 1;
}

void Camera::rotate(const Quaternion& q)
{
    rot *= q;
    computeViewMatrix(viewMatrix, pos, conjugate(rot));
}
void Camera::translate(float dx, float dy, float dz)
{
    pos[0] += dx;
    pos[1] += dy;
    pos[2] += dz;
    computeViewMatrix(viewMatrix, pos, conjugate(rot));
}