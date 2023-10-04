#pragma once
#include <string>

struct v2;
struct v3;
struct v4;
struct v2i;
struct v3i;
struct v4i;

struct v2
{
	float x;
	float y;

	v2();
	v2(float a);
	v2(float x, float y);
	v2(v2i i);

	static v2 Scale(const v2& a, const v2& b);

	static float Dot(const v2& a, const v2& b);
	static float Magnitude(const v2& a);
	static float Distance(const v2& a, const v2& b);
	static v2 Normalize(const v2& a);
	static v2 Reciprocal(const v2& a);
	
	v2 operator+(const v2& a) const;
	v2 operator-() const;
	v2 operator-(const v2& a) const;
	v2 operator*(float a) const;
	v2 operator/(float a) const;
	v2& operator+=(const v2& a);
	v2& operator-=(const v2& a);
	v2& operator*=(float a);
	v2& operator/=(float a);

	bool operator==(const v2& a) const;
	bool operator!=(const v2& a) const;
	
	std::string ToString() const;

	static v2 zero;
	static v2 one;
};

struct v3
{
	float x;
	float y;
	float z;

	v3();
	v3(float a);
	v3(float x, float y);
	v3(float x, float y, float z);
	v3(v4 wis1);
	v3(v3i i);

	static float Dot(const v3& a, const v3& b);
	static float Magnitude(const v3& a);
	static float Distance(const v3& a, const v3& b);
	static v3 Normalize(const v3& a);
	static v3 Cross(const v3& a, const v3& b);

	v3 operator+(const v3& a) const;
	v3 operator-() const;
	v3 operator-(const v3& a) const;
	v3 operator*(float a) const;
	v3 operator/(float a) const;
	v3& operator+=(const v3& a);
	v3& operator-=(const v3& a);
	v3& operator*=(float a);
	v3& operator/=(float a);

	bool operator==(const v3& a) const;
	bool operator!=(const v3& a) const;
	
	std::string ToString() const;

	static v3 zero;
	static v3 one;
};

struct v4
{
	float x;
	float y;
	float z;
	float w;

	v4();
	v4(float x, float y, float z);
	v4(float x, float y, float z, float w);
	v4(v3 wtobe1);
	v4(v4i i);

	static v4 Scale(const v4& a, const v4& b);

	static float Dot(const v4& a, const v4& b);
	static float Magnitude(const v4& a);
	static float Distance(const v4& a, const v4& b);
	static v4 Normalize(const v4& a);

	v4 operator+(const v4& a) const;
	v4 operator-() const;
	v4 operator-(const v4& a) const;
	v4 operator*(float a) const;
	v4 operator/(float a) const;
	v4& operator+=(const v4& a);
	v4& operator-=(const v4& a);
	v4& operator*=(float a);
	v4& operator/=(float a);

	bool operator==(const v4& a) const;
	bool operator!=(const v4& a) const;

	std::string ToString() const;

	static v4 zero;
	static v4 one;
};

struct v2i
{
	int x;
	int y;

	v2i();
	v2i(int a);
	v2i(int x, int y);
	explicit v2i(v2 f);

	v2i operator+(const v2i& a) const;
	v2i operator-() const;
	v2i operator-(const v2i& a) const;
	v2i operator*(int a) const;
	v2i operator/(int a) const;
	v2i& operator+=(const v2i& a);
	v2i& operator-=(const v2i& a);
	v2i& operator*=(int a);
	v2i& operator/=(int a);

	bool operator==(const v2i& a) const;
	bool operator!=(const v2i& a) const;

	std::string ToString() const;

	static v2i zero;
	static v2i one;
};

struct v3i
{
	int x;
	int y;
	int z;

	v3i();
	v3i(int a);
	v3i(int x, int y);
	v3i(int x, int y, int z);
	explicit v3i(v3 f);

	v3i operator+(const v3i& a) const;
	v3i operator-() const;
	v3i operator-(const v3i& a) const;
	v3i operator*(int a) const;
	v3i operator/(int a) const;
	v3i& operator+=(const v3i& a);
	v3i& operator-=(const v3i& a);
	v3i& operator*=(int a);
	v3i& operator/=(int a);

	bool operator==(const v3i& a) const;
	bool operator!=(const v3i& a) const;

	std::string ToString() const;

	static v3i zero;
	static v3i one;
};

struct v4i
{
	int x;
	int y;
	int z;
	int w;

	v4i();
	v4i(int x, int y, int z);
	v4i(int x, int y, int z, int w);
	explicit v4i(v4 f);

	v4i operator+(const v4i& a) const;
	v4i operator-() const;
	v4i operator-(const v4i& a) const;
	v4i operator*(int a) const;
	v4i operator/(int a) const;
	v4i& operator+=(const v4i& a);
	v4i& operator-=(const v4i& a);
	v4i& operator*=(int a);
	v4i& operator/=(int a);

	bool operator==(const v4i& a) const;
	bool operator!=(const v4i& a) const;

	std::string ToString() const;

	static v4i zero;
	static v4i one;
};


// vector hashing operations for use with unordered_map/unordered_set
// https://en.cppreference.com/w/cpp/container/unordered_map/unordered_map
namespace vecHash
{
	template<class vec, class vecType>
	struct KeyHash {
		inline std::size_t operator()(const vec& k) const
		{
			return std::hash<vecType>()(k.x) ^
				(std::hash<vecType>()(k.y) << 1);
		};
	};
	template<class vec>
	struct KeyEqual {
		inline bool operator()(const vec& lhs, const vec& rhs) const
		{
			return lhs == rhs;
		};
	};
}