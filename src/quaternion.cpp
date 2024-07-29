#include "quaternion.h"

Quaternion& Quaternion::operator=(const Quaternion& rhs)
{
    this->w = rhs.w;
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;

    return *this;
}

Quaternion Quaternion::operator+() const
{
    Quaternion q {this->w, this->x, this->y, this->z};
    return q;
}
Quaternion Quaternion::operator-() const
{
    Quaternion q {-this->w, -this->x, -this->y, -this->z};
    return q;
}

Quaternion& Quaternion::operator+=(const Quaternion& rhs)
{
    this->w += rhs.w;
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;

    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& rhs)
{
    this->w -= rhs.w;
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;

    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
    Quaternion lhs {w, x, y, z};
    this->w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
    this->x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
    this->y = lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z;
    this->z = lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x;

    return *this;
}

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion q = lhs;
    q += rhs;
    return q;
}
Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion q = lhs;
    q -= rhs;
    return q;
}
Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion q = lhs;
    q *= rhs;
    return q;
}

Quaternion conjugate(const Quaternion& quaternion)
{
    Quaternion q {quaternion.w, -quaternion.x, -quaternion.y, -quaternion.z};
    return q;
}

void computeQuaternionRotationMatrix(float (&R)[16], const Quaternion& rot)
{
    const float w2 = rot.w * rot.w;
    const float x2 = rot.x * rot.x;
    const float y2 = rot.y * rot.y;
    const float z2 = rot.z * rot.z;
    
    const float k = 1.0f / (w2 + x2 + y2 + z2);
    
    R[0] = 1 - 2 * k * (y2 + z2);
    R[1] = 2 * k * (rot.x * rot.y + rot.w * rot.z);
    R[2] = 2 * k * (rot.x * rot.z - rot.w * rot.y);
    R[3] = 0;
    
    R[4] = 2 * k * (rot.x * rot.y - rot.w * rot.z);
    R[5] = 1 - 2 * k * (x2 + z2);
    R[6] = 2 * k * (rot.y * rot.z + rot.w * rot.x);
    R[7] = 0;
    
    R[8] = 2 * k * (rot.x * rot.z + rot.w * rot.y);
    R[9] = 2 * k * (rot.y * rot.z - rot.w * rot.x);
    R[10] = 1 - 2 * k * (x2 + y2);
    R[11] = 0;

    for (int i = 0; i < 3; i++)
    {
        R[12 + i] = 0;
    }

    R[15] = 1;
}