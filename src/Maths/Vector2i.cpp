#include "Vector.h"

v2i::v2i()
{
    this->x = 0;
    this->y = 0;
}

v2i::v2i(int a)
{
    x = a;
    y = a;
}

v2i::v2i(int x, int y)
{
    this->x = x;
    this->y = y;
}

v2i::v2i(v2 f)
{
    this->x = (int)f.x;
    this->y = (int)f.y;
}

v2i v2i::operator+(const v2i& a) const
{
    return v2i(x + a.x, y + a.y);
}

v2i v2i::operator-() const
{
    return v2i(-x, -y);
}

v2i v2i::operator-(const v2i& a) const
{
    return v2i(x - a.x, y - a.y);
}

v2i v2i::operator*(int a) const
{
    return v2i(x * a, y * a);
}

v2i v2i::operator/(int a) const
{
    return v2i(x / a, y / a);
}

v2i& v2i::operator+=(const v2i& a)
{
    x += a.x;
    y += a.y;
    return *this;
}

v2i& v2i::operator-=(const v2i& a)
{
    x -= a.x;
    y -= a.y;
    return *this;
}

v2i& v2i::operator*=(int a)
{
    x *= a;
    y *= a;
    return *this;
}

v2i& v2i::operator/=(int a)
{
    x /= a;
    y /= a;
    return *this;
}

bool v2i::operator==(const v2i& a) const
{
    return x == a.x && y == a.y;
}

bool v2i::operator!=(const v2i& a) const
{
    return !(*this == a);
}

std::string v2i::ToString() const
{
    return std::to_string(x) + ", " + std::to_string(y);
}

v2i v2i::zero = v2i();
v2i v2i::one = v2i(1, 1);