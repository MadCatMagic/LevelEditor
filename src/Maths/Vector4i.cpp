#include "Vector.h"

v4i::v4i()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->w = 0;
}

v4i::v4i(int x, int y, int z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = 0;
}

v4i::v4i(int x, int y, int z, int w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

v4i::v4i(v4 f)
{
    x = (int)f.x;
    y = (int)f.y;
    z = (int)f.z;
    w = (int)f.w;
}

v4i v4i::operator+(const v4i& a) const
{
    return v4i(x + a.x, y + a.y, z + a.z, w + a.w);
}

v4i v4i::operator-() const
{
    return v4i(-x, -y, -z, -w);
}

v4i v4i::operator-(const v4i& a) const
{
    return v4i(x - a.x, y - a.y, z - a.z, w - a.w);
}

v4i v4i::operator*(int a) const
{
    return v4i(x * a, y * a, z * a, w * a);
}

v4i v4i::operator/(int a) const
{
    return v4i(x / a, y / a, z / a, w / a);
}

v4i& v4i::operator+=(const v4i& a)
{
    x += a.x;
    y += a.y;
    z += a.z;
    w += a.w;
    return *this;
}

v4i& v4i::operator-=(const v4i& a)
{
    x -= a.x;
    y -= a.y;
    z -= a.z;
    w -= a.w;
    return *this;
}

v4i& v4i::operator*=(int a)
{
    x *= a;
    y *= a;
    z *= a;
    w *= a;
    return *this;
}

v4i& v4i::operator/=(int a)
{
    x /= a;
    y /= a;
    z /= a;
    w /= a;
    return *this;
}

bool v4i::operator==(const v4i& a) const
{
    return x == a.x && y == a.y && z == a.z && w == a.w;
}

bool v4i::operator!=(const v4i& a) const
{
    return !(*this == a);
}

std::string v4i::ToString() const
{
    return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w);
}

v4i v4i::zero = v4i();
v4i v4i::one = v4i(1, 1, 1, 1);