#include "Vector.h"

v3i::v3i()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

v3i::v3i(int a)
{
    this->x = a;
    this->y = a;
    this->z = a;
}

v3i::v3i(int x, int y)
{
    this->x = x;
    this->y = y;
    this->z = 0;
}

v3i::v3i(int x, int y, int z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

v3i::v3i(v3 f)
{
    this->x = (int)f.x;
    this->y = (int)f.y;
    this->z = (int)f.z;
}

v3i v3i::operator+(const v3i& a) const
{
    return v3i(x + a.x, y + a.y, z + a.z);
}

v3i v3i::operator-() const
{
    return v3i(-x, -y, -z);
}

v3i v3i::operator-(const v3i& a) const
{
    return v3i(x - a.x, y - a.y, z - a.z);
}

v3i v3i::operator*(int a) const
{
    return v3i(x * a, y * a, z * a);
}

v3i v3i::operator/(int a) const
{
    return v3i(x / a, y / a, z / a);
}

v3i& v3i::operator+=(const v3i& a)
{
    x += a.x;
    y += a.y;
    z += a.z;
    return *this;
}

v3i& v3i::operator-=(const v3i& a)
{
    x -= a.x;
    y -= a.y;
    z -= a.z;
    return *this;
}

v3i& v3i::operator*=(int a)
{
    x *= a;
    y *= a;
    z *= a;
    return *this;
}

v3i& v3i::operator/=(int a)
{
    x /= a;
    y /= a;
    z /= a;
    return *this;
}

bool v3i::operator==(const v3i& a) const
{
    return x == a.x && y == a.y && z == a.z;
}

bool v3i::operator!=(const v3i& a) const
{
    return !(*this == a);
}

std::string v3i::ToString() const
{
    return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
}

v3i v3i::zero = v3i();
v3i v3i::one = v3i(1, 1, 1);