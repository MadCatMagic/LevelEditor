#include "Vector.h"
#include <cmath>

v2::v2()
{
	x = 0.0f;
	y = 0.0f;
}

v2::v2(float a)
{
	x = a;
	y = a;
}

v2::v2(float x, float y)
{
	this->x = x;
	this->y = y;
}

v2::v2(v2i i)
{
	this->x = (float)i.x;
	this->y = (float)i.y;
}

v2 v2::Scale(const v2& a, const v2& b)
{
	return v2(a.x * b.x, a.y * b.y);
}

float v2::Dot(const v2& a, const v2& b)
{
	return a.x * b.x + a.y * b.y;
}

float v2::Magnitude(const v2& a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}

float v2::Distance(const v2& a, const v2& b)
{
	return Magnitude(a - b);
}

static float kEpsilon = 0.00001f;
v2 v2::Normalize(const v2& a)
{
	float mag = Magnitude(a);
	if (mag > kEpsilon)
		return a / mag;
	else
		return zero;
}

v2 v2::Reciprocal(const v2& a)
{
	return v2(1.0f / a.x, 1.0f / a.y);
}

v2 v2::operator+(const v2& a) const
{
	return v2(x + a.x, y + a.y);
}

v2 v2::operator-() const
{
	return v2(-x, -y);
}

v2 v2::operator-(const v2& a) const
{
	return v2(x - a.x, y - a.y);
}

v2 v2::operator*(float a) const
{
	return v2(x * a, y * a);
}

v2 v2::operator/(float a) const
{
	return v2(x / a, y / a);
}

v2& v2::operator+=(const v2& a)
{
	x += a.x;
	y += a.y;
	return *this;
}

v2& v2::operator-=(const v2& a)
{
	x -= a.x;
	y -= a.y;
	return *this;
}

v2& v2::operator*=(float a)
{
	x *= a;
	y *= a;
	return *this;
}

v2& v2::operator/=(float a)
{
	x /= a;
	y /= a;
	return *this;
}

bool v2::operator==(const v2& a) const
{
	return x == a.x && y == a.y;
}

bool v2::operator!=(const v2& a) const
{
	return !operator==(a);
}

std::string v2::ToString() const
{
	return std::to_string(x) + ", " + std::to_string(y);
}

v2 v2::zero = v2();
v2 v2::one = v2(1.0f, 1.0f);