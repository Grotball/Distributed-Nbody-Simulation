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