#include <cmath>
#include "Vector.h"

v3::v3()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

v3::v3(float a)
{
	x = a;
	y = a;
	z = a;
}

v3::v3(float x, float y)
{
	this->x = x;
	this->y = y;
	z = 0.0f;
}

v3::v3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

v3::v3(v4 wis1)
{
	this->x = wis1.x;
	this->y = wis1.y;
	this->z = wis1.z;
}

v3::v3(v3i i)
{
	this->x = (float)i.x;
	this->y = (float)i.y;
	this->z = (float)i.z;
}

float v3::Dot(const v3& a, const v3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float v3::Magnitude(const v3& a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

float v3::Distance(const v3& a, const v3& b)
{
	return Magnitude(a - b);
}

static float kEpsilon = 0.00001f;
v3 v3::Normalize(const v3& a)
{
	float mag = Magnitude(a);
	if (mag > kEpsilon)
		return a / mag;
	else
		return zero;
}

v3 v3::Cross(const v3& a, const v3& b)
{
	return v3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

v3 v3::operator+(const v3& a) const
{
	return v3(x + a.x, y + a.y, z + a.z);
}

v3 v3::operator-() const
{
	return v3(-x, -y, -z);
}

v3 v3::operator-(const v3& a) const
{
	return v3(x - a.x, y - a.y, z - a.z);
}

v3 v3::operator*(float a) const
{
	return v3(x * a, y * a, z * a);
}

v3 v3::operator/(float a) const
{
	return v3(x / a, y / a, z / a);
}

v3& v3::operator+=(const v3& a)
{
	this->x += a.x;
	this->y += a.y;
	this->z += a.z;
	return *this;
}

v3& v3::operator-=(const v3& a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	return *this;
}

v3& v3::operator*=(float a)
{
	x *= a;
	y *= a;
	z *= a;
	return *this;
}

v3& v3::operator/=(float a)
{
	x /= a;
	y /= a;
	z /= a;
	return *this;
}

bool v3::operator==(const v3& a) const
{
	return x == a.x && y == a.y && z == a.z;
}

bool v3::operator!=(const v3& a) const
{
	return !operator==(a);
}

std::string v3::ToString() const
{
	return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
}

v3 v3::zero = v3();
v3 v3::one = v3(1.0f, 1.0f, 1.0f);