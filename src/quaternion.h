#pragma once

struct Quaternion
{
    float w, x, y, z;
    Quaternion& operator=(const Quaternion& rhs);
    Quaternion& operator+=(const Quaternion& rhs);
    Quaternion& operator-=(const Quaternion& rhs);
    Quaternion& operator*=(const Quaternion& rhs);
    Quaternion operator+() const;
    Quaternion operator-() const;
};

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs);
Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

Quaternion conjugate(const Quaternion& quaternion);