#include <cmath>
#include "Vector.h"

v4::v4()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

v4::v4(const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	w = 0.0f;
}

v4::v4(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

v4::v4(v3 wtobe1)
{
	this->x = wtobe1.x;
	this->y = wtobe1.y;
	this->z = wtobe1.z;
	this->w = 1.0f;
}

v4::v4(v4i i)
{
	this->x = (float)i.x;
	this->y = (float)i.y;
	this->z = (float)i.z;
	this->w = (float)i.w;
}

float v4::Dot(const v4& a, const v4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float v4::Magnitude(const v4& a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

float v4::Distance(const v4& a, const v4& b)
{
	return Magnitude(a - b);
}

static float kEpsilon = 0.00001f;
v4 v4::Normalize(const v4& a)
{
	float mag = Magnitude(a);
	if (mag > kEpsilon)
		return a / mag;
	else
		return zero;
}

v4 v4::operator+(const v4& a) const
{
	return v4(x + a.x, y + a.y, z + a.z, w + a.w);
}

v4 v4::operator-() const
{
	return v4(-x, -y, -z, -w);
}

v4 v4::operator-(const v4& a) const
{
	return v4(x - a.x, y - a.y, z - a.z, w - a.w);
}

v4 v4::operator*(float a) const
{
	return v4(x * a, y * a, z * a, w * a);
}

v4 v4::operator/(float a) const
{
	return v4(x / a, y / a, z / a, w / a);
}

v4& v4::operator+=(const v4& a)
{
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;
	return *this;
}

v4& v4::operator-=(const v4& a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;
	return *this;
}

v4& v4::operator*=(float a)
{
	x *= a;
	y *= a;
	z *= a;
	w *= a;
	return *this;
}

v4& v4::operator/=(float a)
{
	x /= a;
	y /= a;
	z /= a;
	w /= a;
	return *this;
}

bool v4::operator==(const v4& a) const
{
	return x == a.x && y == a.y && z == a.z && w == a.w;
}

bool v4::operator!=(const v4& a) const
{
	return !operator==(a);
}

std::string v4::ToString() const
{
	return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w);
}

v4 v4::zero = v4();
v4 v4::one = v4(1.0f, 1.0f, 1.0f, 1.0f);