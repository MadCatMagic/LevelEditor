

#include "Matrix.h"

mat4x4::mat4x4() { }

mat4x4::mat4x4(float scalar)
{
	m[0] = scalar;
	m[5] = scalar;
	m[10] = scalar;
	m[15] = scalar;
}

mat4x4::mat4x4(const v4& diagonal)
{
	m[0] = diagonal.x;
	m[5] = diagonal.y;
	m[10] = diagonal.z;
	m[15] = diagonal.w;
}

mat4x4::mat4x4(const v4& c0, const v4& c1, const v4& c2, const v4& c3)
{
	float M[16] = {
		c0.x, c1.x, c2.x, c3.x,
		c0.y, c1.y, c2.y, c3.y,
		c0.z, c1.z, c2.z, c3.z,
		c0.w, c1.w, c2.w, c3.w
	};
	for (int i = 0; i < 16; i++)
	{
		m[i] = M[i];
	}
}

mat4x4::~mat4x4()
{
}

void mat4x4::SetColumn(const v4& data, int column)
{
	m[0 + column] = data.x;
	m[4 + column] = data.y;
	m[8 + column] = data.z;
	m[12 + column] = data.w;
}

void mat4x4::SetRow(const v4& data, int row)
{
	m[0 + row * 4] = data.x;
	m[1 + row * 4] = data.y;
	m[2 + row * 4] = data.z;
	m[3 + row * 4] = data.w;
}

v4 mat4x4::GetColumn(int column)
{
	return v4(m[0 + column], m[4 + column], m[8 + column], m[12 + column]);
}

v4 mat4x4::GetRow(int row)
{
	return v4(m[0 + row * 4], m[1 + row * 4], m[2 + row * 4], m[3 + row * 4]);
}

float mat4x4::GetValue(int column, int row)
{
	return m[column + row * 4];
}

void mat4x4::SetValue(int column, int row, float value)
{
	m[column + row * 4] = value;
}

mat4x4 mat4x4::GetTranspose()
{
	mat4x4 M = mat4x4();
	M.m[0] = m[0];
	M.m[1] = m[4];
	M.m[2] = m[8];
	M.m[3] = m[12];
	M.m[4] = m[1];
	M.m[5] = m[5];
	M.m[6] = m[9];
	M.m[7] = m[13];
	M.m[8] = m[2];
	M.m[9] = m[6];
	M.m[10] = m[10];
	M.m[11] = m[14];
	M.m[12] = m[3];
	M.m[13] = m[7];
	M.m[14] = m[11];
	return M;
}

// expensive operation
mat4x4 mat4x4::GetInverse()
{
	// https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
	float inv[16]{}, det;
	int i;
	
	inv[0] = m[5] * m[10] * m[15] -
	m[5] * m[11] * m[14] -
	m[9] * m[6] * m[15] +
	m[9] * m[7] * m[14] +
	m[13] * m[6] * m[11] -
	m[13] * m[7] * m[10];
	
	inv[4] = -m[4] * m[10] * m[15] +
	m[4] * m[11] * m[14] +
	m[8] * m[6] * m[15] -
	m[8] * m[7] * m[14] -
	m[12] * m[6] * m[11] +
	m[12] * m[7] * m[10];
	
	inv[8] = m[4] * m[9] * m[15] -
	m[4] * m[11] * m[13] -
	m[8] * m[5] * m[15] +
	m[8] * m[7] * m[13] +
	m[12] * m[5] * m[11] -
	m[12] * m[7] * m[9];
	
	inv[12] = -m[4] * m[9] * m[14] +
	m[4] * m[10] * m[13] +
	m[8] * m[5] * m[14] -
	m[8] * m[6] * m[13] -
	m[12] * m[5] * m[10] +
	m[12] * m[6] * m[9];
	
	inv[1] = -m[1] * m[10] * m[15] +
	m[1] * m[11] * m[14] +
	m[9] * m[2] * m[15] -
	m[9] * m[3] * m[14] -
	m[13] * m[2] * m[11] +
	m[13] * m[3] * m[10];
	
	inv[5] = m[0] * m[10] * m[15] -
	m[0] * m[11] * m[14] -
	m[8] * m[2] * m[15] +
	m[8] * m[3] * m[14] +
	m[12] * m[2] * m[11] -
	m[12] * m[3] * m[10];
	
	inv[9] = -m[0] * m[9] * m[15] +
	m[0] * m[11] * m[13] +
	m[8] * m[1] * m[15] -
	m[8] * m[3] * m[13] -
	m[12] * m[1] * m[11] +
	m[12] * m[3] * m[9];
	
	inv[13] = m[0] * m[9] * m[14] -
	m[0] * m[10] * m[13] -
	m[8] * m[1] * m[14] +
	m[8] * m[2] * m[13] +
	m[12] * m[1] * m[10] -
	m[12] * m[2] * m[9];
	
	inv[2] = m[1] * m[6] * m[15] -
	m[1] * m[7] * m[14] -
	m[5] * m[2] * m[15] +
	m[5] * m[3] * m[14] +
	m[13] * m[2] * m[7] -
	m[13] * m[3] * m[6];
	
	inv[6] = -m[0] * m[6] * m[15] +
	m[0] * m[7] * m[14] +
	m[4] * m[2] * m[15] -
	m[4] * m[3] * m[14] -
	m[12] * m[2] * m[7] +
	m[12] * m[3] * m[6];
	
	inv[10] = m[0] * m[5] * m[15] -
	m[0] * m[7] * m[13] -
	m[4] * m[1] * m[15] +
	m[4] * m[3] * m[13] +
	m[12] * m[1] * m[7] -
	m[12] * m[3] * m[5];
	
	inv[14] = -m[0] * m[5] * m[14] +
	m[0] * m[6] * m[13] +
	m[4] * m[1] * m[14] -
	m[4] * m[2] * m[13] -
	m[12] * m[1] * m[6] +
	m[12] * m[2] * m[5];
	
	inv[3] = -m[1] * m[6] * m[11] +
	m[1] * m[7] * m[10] +
	m[5] * m[2] * m[11] -
	m[5] * m[3] * m[10] -
	m[9] * m[2] * m[7] +
	m[9] * m[3] * m[6];
	
	inv[7] = m[0] * m[6] * m[11] -
	m[0] * m[7] * m[10] -
	m[4] * m[2] * m[11] +
	m[4] * m[3] * m[10] +
	m[8] * m[2] * m[7] -
	m[8] * m[3] * m[6];
	
	inv[11] = -m[0] * m[5] * m[11] +
	m[0] * m[7] * m[9] +
	m[4] * m[1] * m[11] -
	m[4] * m[3] * m[9] -
	m[8] * m[1] * m[7] +
	m[8] * m[3] * m[5];
	
	inv[15] = m[0] * m[5] * m[10] -
	m[0] * m[6] * m[9] -
	m[4] * m[1] * m[10] +
	m[4] * m[2] * m[9] +
	m[8] * m[1] * m[6] -
	m[8] * m[2] * m[5];
	
	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	if (det == 0.0f)
		throw("determinant is zero in matrix inversion");
	det = 1.0f / det;
	
	mat4x4 out;
	for (i = 0; i < 16; i++)
		out.m[i] = inv[i] * det;
	
	return out;
}

v4 mat4x4::GetDiagonal()
{
	return v4(m[0], m[5], m[10], m[15]);
}

mat4x4 mat4x4::operator+(const mat4x4 other) const
{
	mat4x4 M = mat4x4();
	M.m[0] = m[0] + other.m[0];
	M.m[1] = m[1] + other.m[1];
	M.m[2] = m[2] + other.m[2];
	M.m[3] = m[3] + other.m[3];
	M.m[4] = m[4] + other.m[4];
	M.m[5] = m[5] + other.m[5];
	M.m[6] = m[6] + other.m[6];
	M.m[7] = m[7] + other.m[7];
	M.m[8] = m[8] + other.m[8];
	M.m[9] = m[9] + other.m[9];
	M.m[10] = m[10] + other.m[10];
	M.m[11] = m[11] + other.m[11];
	M.m[12] = m[12] + other.m[12];
	M.m[13] = m[13] + other.m[13];
	M.m[14] = m[14] + other.m[14];
	M.m[15] = m[15] + other.m[15];
	return M;
}

mat4x4 mat4x4::operator-(const mat4x4 other) const
{
	mat4x4 M = mat4x4();
	M.m[0] = m[0] - other.m[0];
	M.m[1] = m[1] - other.m[1];
	M.m[2] = m[2] - other.m[2];
	M.m[3] = m[3] - other.m[3];
	M.m[4] = m[4] - other.m[4];
	M.m[5] = m[5] - other.m[5];
	M.m[6] = m[6] - other.m[6];
	M.m[7] = m[7] - other.m[7];
	M.m[8] = m[8] - other.m[8];
	M.m[9] = m[9] - other.m[9];
	M.m[10] = m[10] - other.m[10];
	M.m[11] = m[11] - other.m[11];
	M.m[12] = m[12] - other.m[12];
	M.m[13] = m[13] - other.m[13];
	M.m[14] = m[14] - other.m[14];
	M.m[15] = m[15] - other.m[15];
	return M;
}

mat4x4 mat4x4::operator*(const mat4x4 other) const
{
	mat4x4 M = mat4x4();
	M.m[0] = other.m[0] * m[0] + other.m[4] * m[1] + other.m[8] * m[2] + other.m[12] * m[3];
	M.m[1] = other.m[1] * m[0] + other.m[5] * m[1] + other.m[9] * m[2] + other.m[13] * m[3];
	M.m[2] = other.m[2] * m[0] + other.m[6] * m[1] + other.m[10] * m[2] + other.m[14] * m[3];
	M.m[3] = other.m[3] * m[0] + other.m[7] * m[1] + other.m[11] * m[2] + other.m[15] * m[3];

	M.m[4] = other.m[0] * m[4] + other.m[4] * m[5] + other.m[8] * m[6] + other.m[12] * m[7];
	M.m[5] = other.m[1] * m[4] + other.m[5] * m[5] + other.m[9] * m[6] + other.m[13] * m[7];
	M.m[6] = other.m[2] * m[4] + other.m[6] * m[5] + other.m[10] * m[6] + other.m[14] * m[7];
	M.m[7] = other.m[3] * m[4] + other.m[7] * m[5] + other.m[11] * m[6] + other.m[15] * m[7];

	M.m[8] = other.m[0] * m[8] + other.m[4] * m[9] + other.m[8] * m[10] + other.m[12] * m[11];
	M.m[9] = other.m[1] * m[8] + other.m[5] * m[9] + other.m[9] * m[10] + other.m[13] * m[11];
	M.m[10] = other.m[2] * m[8] + other.m[6] * m[9] + other.m[10] * m[10] + other.m[14] * m[11];
	M.m[11] = other.m[3] * m[8] + other.m[7] * m[9] + other.m[11] * m[10] + other.m[15] * m[11];

	M.m[12] = other.m[0] * m[12] + other.m[4] * m[13] + other.m[8] * m[14] + other.m[12] * m[15];
	M.m[13] = other.m[1] * m[12] + other.m[5] * m[13] + other.m[9] * m[14] + other.m[13] * m[15];
	M.m[14] = other.m[2] * m[12] + other.m[6] * m[13] + other.m[10] * m[14] + other.m[14] * m[15];
	M.m[15] = other.m[3] * m[12] + other.m[7] * m[13] + other.m[11] * m[14] + other.m[15] * m[15];
	return M;
}

v4 mat4x4::operator*(const v4 other) const
{
	v4 v = v4();
	v.x = other.x * m[0] + other.y * m[1] + other.z * m[2] + other.w * m[3];
	v.y = other.x * m[4] + other.y * m[5] + other.z * m[6] + other.w * m[7];
	v.z = other.x * m[8] + other.y * m[9] + other.z * m[10] + other.w * m[11];
	v.w = other.x * m[12] + other.y * m[13] + other.z * m[14] + other.w * m[15];
	return v;
}

v3 mat4x4::operator*(v3 other) const
{
	return v3(*this * v4(other));
}

mat4x4 mat4x4::operator*(float scalar) const
{
	mat4x4 M = mat4x4();
	M.m[0] = m[0] * scalar;
	M.m[1] = m[1] * scalar;
	M.m[2] = m[2] * scalar;
	M.m[3] = m[3] * scalar;
	M.m[4] = m[4] * scalar;
	M.m[5] = m[5] * scalar;
	M.m[6] = m[6] * scalar;
	M.m[7] = m[7] * scalar;
	M.m[8] = m[8] * scalar;
	M.m[9] = m[9] * scalar;
	M.m[10] = m[10] * scalar;
	M.m[11] = m[11] * scalar;
	M.m[12] = m[12] * scalar;
	M.m[13] = m[13] * scalar;
	M.m[14] = m[14] * scalar;
	M.m[15] = m[15] * scalar;
	return M;
}

std::string mat4x4::ToString()
{
	std::string str = "";
	for (int column = 0; column < 4; column++) 
	{
		for (int row = 0; row < 4; row++)
		{
			str += std::to_string(m[column + row * 4]) + "\t";
		}
		str += "\n";
	}
	return str;
}

mat4x4 mat4x4::PointAt(const v3& pos, const v3& target, const v3& up)
{
	v3 newForward = v3::Normalize(target - pos);
	v3 a = newForward * v3::Dot(up, newForward);
	v3 newUp = v3::Normalize(up - a);
	v3 newRight = v3::Cross(newUp, newForward);
	mat4x4 matrix = mat4x4(
		v4(newRight.x, newRight.y, newRight.z, 0.0f),
		v4(newUp.x, newUp.y, newUp.z, 0.0f),
		v4(newForward.x, newForward.y, newForward.z, 0.0f),
		v4(pos.x, pos.y, pos.z, 1.0f)
	);
	return matrix;
}

mat4x4 mat4x4::LookAt(const v3& pos, const v3& target, const v3& up)
{
	v3 newForward = v3::Normalize(target - pos);
	v3 a = newForward * v3::Dot(up, newForward);
	v3 newUp = v3::Normalize(up - a);
	v3 newRight = v3::Cross(newUp, newForward);

	/*
	Matrix4x4 matrix = Matrix4x4(
		Vector4(newRight.x,   newRight.y,   newRight.z,   -Vector3::Dot(pos, newRight)),
		Vector4(newUp.x,      newUp.y,      newUp.z,      -Vector3::Dot(pos, newUp)),
		Vector4(newForward.x, newForward.y, newForward.z, -Vector3::Dot(pos, newForward)),
		Vector4(0.0f, 0.0f, 0.0f, 1.0f)
	);*/
	
	mat4x4 matrix = mat4x4(
		v4(newRight.x, newUp.x, newForward.x, 0.0f),
		v4(newRight.y, newUp.y, newForward.y, 0.0f),
		v4(newRight.z, newUp.z, newForward.z, 0.0f),
		v4(-v3::Dot(pos, newRight), -v3::Dot(pos, newUp), -v3::Dot(pos, newForward), 1.0f)
	);
	return matrix;
}

mat4x4 mat4x4::Rotation(const v3& euler)
{
	float sx = (float)sin(euler.x);
	float sy = (float)sin(euler.y);
	float sz = (float)sin(euler.z);
	float cx = (float)cos(euler.x);
	float cy = (float)cos(euler.y);
	float cz = (float)cos(euler.z);

	return mat4x4(
		v4(cz * cx - sz * sx * sy, -cx * sz, cz * sy + cy * sz * sx, 0.0f),
		v4(cy * sz + cz * sx * sy, cz * cx, sz * sy - cz * cy * sx, 0.0f),
		v4(-cx * sy, sx, cx * cy, 0.0f),
		v4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

mat4x4 mat4x4::ProjectionMatrix(float fov, float zNear, float zFar, float widthHeightRatio)
{
	const float f = (float)(1.0 / tan((double)fov * 0.5));
	const float q = zFar / (zFar - zNear);
	return mat4x4(
		v4(f, 0.0f, 0.0f, 0.0f),
		v4(0.0f, f * widthHeightRatio, 0.0f, 0.0f),
		v4(0.0f, 0.0f, q, 1.0f),
		v4(0.0f, 0.0f, -zNear * q, 0.0f)
	);
}

mat4x4 mat4x4::ViewMatrix(const v3& pos, const v3& dir, const v3& up)
{
	// you have to use the conjugate for correct camera movement
	// I DONT KNOW WHY WTF
	return mat4x4::LookAt(
		pos,
		pos - dir,
		up
	);
}

// returns orthoganal projection matrix
// here min represents (left, bottom, -near) and max represents (right, top, -far)
mat4x4 mat4x4::OrthoMatrix(float left, float right, float bottom, float top, float near, float far)
{
	return mat4x4(
		v4(2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left)),
		v4(0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom)),
		v4(0.0f, 0.0f, -2.0f / (near - far), -(far + near) / (far - near)),
		v4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}

mat4x4 mat4x4::zero = mat4x4();

mat4x4 mat4x4::identity = mat4x4(1.0f);

