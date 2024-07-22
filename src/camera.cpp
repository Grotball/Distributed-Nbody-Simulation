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
    float w2 = rot.w * rot.w;
    float x2 = rot.x * rot.x;
    float y2 = rot.y * rot.y;
    float z2 = rot.z * rot.z;
    
    float k = 1.0f / (w2 + x2 + y2 + z2);
    
    viewMatrix[0] = 1 - 2 * k * (y2 + z2);
    viewMatrix[1] = 2 * k * (rot.x * rot.y + rot.w * rot.z);
    viewMatrix[2] = 2 * k * (rot.x * rot.z - rot.w * rot.y);
    viewMatrix[3] = 0;
    
    viewMatrix[4] = 2 * k * (rot.x * rot.y - rot.w * rot.z);
    viewMatrix[5] = 1 - 2 * k * (x2 + z2);
    viewMatrix[6] = 2 * k * (rot.y * rot.z + rot.w * rot.x);
    viewMatrix[7] = 0;
    
    viewMatrix[8] = 2 * k * (rot.x * rot.z + rot.w * rot.y);
    viewMatrix[9] = 2 * k * (rot.y * rot.z - rot.w * rot.x);
    viewMatrix[10] = 1 - 2 * k * (x2 + y2);
    viewMatrix[11] = 0;

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