#include "Quaternion.h"

#include <cmath>

Quaternion::Quaternion()
{
    this->w = 1;
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Quaternion::Quaternion(float w, float x, float y, float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

// z is yaw
// y is pitch
// x is roll
Quaternion::Quaternion(const v3& euler)
{
    double cz = cos(euler.z * 0.5);
    double sz = sin(euler.z * 0.5);
    double cy = cos(euler.y * 0.5);
    double sy = sin(euler.y * 0.5);
    double cx = cos(euler.x * 0.5);
    double sx = sin(euler.x * 0.5);

    this->w = (float)(cz * cx * cy - sz * sx * sy);
    this->x = (float)(cz * sx * cy - sz * cx * sy);
    this->y = (float)(sz * sx * cy + cz * cx * sy);
    this->z = (float)(cz * sx * sy + sz * cx * cy);
}

Quaternion::Quaternion(const v3& axis, float angle)
{
    this->w = cos(angle / 2);
    this->x = sin(angle / 2) * axis.x;
    this->y = sin(angle / 2) * axis.y;
    this->z = sin(angle / 2) * axis.z;
}

Quaternion::~Quaternion() { }

Quaternion Quaternion::operator+(const Quaternion& a) const
{
    return Quaternion(w + a.w, x + a.x, y + a.y, z + a.z);
}

Quaternion Quaternion::operator-() const
{
    return Quaternion(-w, -x, -y, -z);
}

Quaternion Quaternion::operator-(const Quaternion& a) const
{
    return Quaternion(w - a.w, x - a.x, y - a.y, z - a.z);
}

Quaternion Quaternion::operator*(const Quaternion& a) const
{
    return Quaternion(
        w * a.w - x * a.x - y * a.y - z * a.z,
        w * a.x + x * a.w + y * a.z - z * a.y,
        w * a.y - x * a.z + y * a.w + z * a.x,
        w * a.z - x * a.y - y * a.x - z * a.w
    );
}

Quaternion Quaternion::operator/(const Quaternion& a) const
{
    return *this * Quaternion::Conjugate(a);
}

Quaternion& Quaternion::operator+=(const Quaternion& a)
{
    this->w += a.w;
    this->x += a.x;
    this->y += a.y;
    this->z += a.z;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& a)
{
    this->w -= a.w;
    this->x -= a.x;
    this->y -= a.y;
    this->z -= a.z;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& a)
{
    this->x =  x * a.w + y * a.z - z * a.y + w * a.x;
    this->y = -x * a.z + y * a.w + z * a.x + w * a.y;
    this->z =  x * a.y - y * a.x + z * a.w + w * a.z;
    this->w = -x * a.x - y * a.y - z * a.z + w * a.w;
    return *this;
}

mat4x4 Quaternion::GetRotationMatrix() const
{
    mat4x4 mat = mat4x4();
    double w = this->w;
    double x = this->x;
    double y = this->y;
    double z = this->z;
    // calculate coefficients
    double x2 = x + x; double y2 = y + y;
    double z2 = z + z;
    double xx = x * x2; double xy = x * y2; double xz = x * z2;
    double yy = y * y2; double yz = y * z2; double zz = z * z2;
    double wx = w * x2; double wy = w * y2; double wz = w * z2;


    mat.m[0] = (float)(1.0 - (yy + zz)); mat.m[1] = (float)(xy - wz);
    mat.m[2] = (float)(xz + wy); mat.m[3] = 0.0;
    mat.m[4] = (float)(xy + wz); mat.m[5] = (float)(1.0 - (xx + zz));
    mat.m[6] = (float)(yz - wx); mat.m[7] = 0.0;
    mat.m[8] = (float)(xz - wy); mat.m[9] = (float)(yz + wx);
    mat.m[10] = (float)(1.0 - (xx + yy)); mat.m[11] = 0.0;
    mat.m[12] = 0; mat.m[14] = 0;
    mat.m[13] = 0; mat.m[15] = 1;
    return mat;
}

v3 Quaternion::RotateVector(const v3& a) const
{
    Quaternion q = Quaternion(0.0f, a.x, a.y, a.z);
    Quaternion t = Quaternion(this->w, this->x, this->y, this->z);
    q = t * q * Quaternion::Conjugate(t);
    v3 o = v3(q.x, q.y, q.z);
    return o;
}

v3 Quaternion::ToEuler() const
{
    double w = this->w;
    double x = this->x;
    double y = this->y;
    double z = this->z;
    return v3(
        (float)atan2(2.0 * (w * x + y * z), w * w - x * x - y * y + z * z),
        (float)-asin(2.0 * (x * z - w * y)),
        (float)atan2(2.0 * (w * z + x * y), w * w + x * x - y * y - z * z)
    );
}

float Quaternion::Length(const Quaternion& a)
{
    return sqrt(a.w * a.w + a.x * a.x + a.y * a.y + a.z * a.z);
}

Quaternion Quaternion::Normalize(const Quaternion& a)
{
    float n = (float)sqrt(a.w * a.w + a.x * a.x + a.y * a.y + a.z * a.z);
    if (n != 0.0f)
        return Quaternion(a.w / n, a.x / n, a.y / n, a.z / n);
    return Quaternion();
}

Quaternion Quaternion::Conjugate(const Quaternion& a)
{
    return Quaternion(a.w, -a.x, -a.y, -a.z);
}

bool Quaternion::operator==(const Quaternion& a) const
{
    return w == a.w && x == a.x && y == a.y && z == a.z;
}

bool Quaternion::operator!=(const Quaternion& a) const
{
    return !(*this == a);
}

std::string Quaternion::ToString() const
{
    return std::to_string(w) + ", " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
}

Quaternion Quaternion::identity = Quaternion();

#define C_RadToPi 57.2957795131f;
float RadiiToDegrees(float angle)
{
    return angle * C_RadToPi;
}

#define C_PiToRad 0.01745329251f;
float DegreesToRadii(float angle)
{
    return angle * C_PiToRad;
}
