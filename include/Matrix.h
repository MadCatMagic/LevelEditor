#pragma once
#include <string>
#include "Vector.h"

struct mat4x4
{
	mat4x4();
	mat4x4(float scalar);
	mat4x4(const v4& diagonal);
	mat4x4(const v4& c0, const v4& c1, const v4& c2, const v4& c3);
	~mat4x4();

	void SetColumn(const v4& data, int column);
	void SetRow(const v4& data, int row);
	v4 GetColumn(int column);
	v4 GetRow(int row);
	float GetValue(int column, int row);
	void SetValue(int column, int row, float value);

	mat4x4 GetTranspose();
	mat4x4 GetInverse();
	v4 GetDiagonal();

	mat4x4 operator+(mat4x4 other) const;
	mat4x4 operator-(mat4x4 other) const;
	mat4x4 operator*(mat4x4 other) const;
	v4 operator*(v4 other) const;
	v3 operator*(v3 other) const;
	mat4x4 operator*(float scalar) const;
	std::string ToString();

	static mat4x4 PointAt(const v3& pos, const v3& target, const v3& up);
	static mat4x4 LookAt(const v3& pos, const v3& target, const v3& up);
	static mat4x4 Rotation(const v3& euler);

	static mat4x4 ProjectionMatrix(float fov, float zNear, float zFar, float widthHeightRatio);
	static mat4x4 ViewMatrix(const v3& pos, const v3& dir, const v3& up);
	static mat4x4 OrthoMatrix(float left, float right, float bottom, float top, float near, float far);

	static mat4x4 zero;
	static mat4x4 identity;

	float m[16] { };
	/* m = [
		 0,  1,  2,  3,
		 4,  5,  6,  7,
		 8,  9, 10, 11,
		12, 13, 14, 15
	]*/
};