#pragma once
#include "Matrix.h"
#include <string>

// mostly from http://www.chrobotics.com/library/understanding-quaternions
// complicated maths D:
class Quaternion
{
public:
	float w;
	float x;
	float y;
	float z;

	Quaternion();
	Quaternion(float w, float x, float y, float z);
	Quaternion(const v3& euler);
	Quaternion(const v3& axis, float angle);
	~Quaternion();

	Quaternion operator+(const Quaternion& a) const;
	Quaternion operator-() const;
	Quaternion operator-(const Quaternion& a) const;
	Quaternion operator*(const Quaternion& a) const;
	Quaternion operator/(const Quaternion& a) const;
	Quaternion& operator+=(const Quaternion& a);
	Quaternion& operator-=(const Quaternion& a);
	Quaternion& operator*=(const Quaternion& a);

	mat4x4 GetRotationMatrix() const;
	v3 RotateVector(const v3& a) const;
	v3 ToEuler() const;

	static float Length(const Quaternion& a);
	static Quaternion Normalize(const Quaternion& a);
	static Quaternion Conjugate(const Quaternion& a);

	bool operator==(const Quaternion& a) const;
	bool operator!=(const Quaternion& a) const;

	std::string ToString() const;

	static Quaternion identity;
};

extern float RadiiToDegrees(float angle);
extern float DegreesToRadii(float angle);