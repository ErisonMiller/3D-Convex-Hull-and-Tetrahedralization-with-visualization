#pragma once
#ifndef __LINEAR_ALGEBRA_H_
#define __LINEAR_ALGEBRA_H_

#include <math.h>

using namespace std;

#define M_PI 3.14159265359f
#define SMALL_NUMBER 0.000001f

struct Vector3Df
{
	union {
		struct { float x, y, z; };
		float _v[3];
	};

	inline float length() { return sqrtf(x*x + y * y + z * z); }
	inline float lengthsq() { return x * x + y * y + z * z; }
	inline void normalize() { float norm = 1.0f / sqrtf(x*x + y * y + z * z); x *= norm; y *= norm; z *= norm; }
	inline Vector3Df to_unitary() { float norm = 1.0f / sqrtf(x*x + y * y + z * z); return Vector3Df{ x *norm, y * norm, z *= norm }; }
	inline void operator+=(const Vector3Df& v) { x += v.x; y += v.y; z += v.z; }
	inline void operator-=(const Vector3Df& v) { x -= v.x; y -= v.y; z -= v.z; }
	inline void operator*=(const float& a) { x *= a; y *= a; z *= a; }
	inline void operator*=(const Vector3Df& v) { x *= v.x; y *= v.y; z *= v.z; }
	inline void operator/=(const float& a) { x /= a; y /= a; z /= a; }
	inline Vector3Df operator*(float a) const { return Vector3Df{ x*a, y*a, z*a }; }
	inline Vector3Df operator/(float a) const { return Vector3Df{ x / a, y / a, z / a }; }
	inline Vector3Df operator*(const Vector3Df& v) const { return Vector3Df{ x * v.x, y * v.y, z * v.z }; }
	inline Vector3Df operator+(const Vector3Df& v) const { return Vector3Df{ x + v.x, y + v.y, z + v.z }; }
	inline Vector3Df operator-(const Vector3Df& v) const { return Vector3Df{ x - v.x, y - v.y, z - v.z }; }
	inline bool operator!=(const Vector3Df& v) { return x != v.x || y != v.y || z != v.z; }
	inline bool operator==(const Vector3Df& v) { return abs(x-v.x) < SMALL_NUMBER && abs(y - v.y) < SMALL_NUMBER && abs(z - v.z) < SMALL_NUMBER; }
};

inline Vector3Df cross(const Vector3Df& v1, const Vector3Df& v2) { return Vector3Df{ v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x }; }
inline float dot(const Vector3Df& v1, const Vector3Df& v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
inline float distancesq(const Vector3Df& v1, const Vector3Df& v2) { return (v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y) + (v1.z - v2.z)*(v1.z - v2.z); }
inline float distance(const Vector3Df& v1, const Vector3Df& v2) { return sqrtf((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y) + (v1.z - v2.z)*(v1.z - v2.z)); }


struct Matrix4 {
	//eachone is a line
	float m[16];

	Vector3Df operator *(const Vector3Df &v) {
		float x, y, z, r = 1.0f / (m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15]);
		x = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
		y = m[4] * v.x + m[5] * v.y + m[6] * v.z + m[7];
		z = m[8] * v.x + m[9] * v.y + m[10] * v.z + m[11];
		return Vector3Df{ x*r,y*r,z };
	}
};


inline
Matrix4 translate(const Vector3Df &v)
{
	return Matrix4{
		1, 0, 0, v.x,
		0, 1, 0, v.y,
		0, 0, 1, v.z,
		0, 0, 0, 1 };
}

inline
Matrix4 resizeMatrix(float x, float y, float z)
{
	return Matrix4{
	x,	0,	0,	0,
	0,	y,	0,	0,
	0,	0,	z,	0,
	0,	0,	0,	1 };

}

inline
Matrix4 rotateX(float ang)
{
	ang = (ang* M_PI) / 180;

	return Matrix4{
	1,	0,	0,	0,
	0,	cosf(ang),	-sinf(ang),	0,
	0,	sinf(ang),	cosf(ang),	0,
	0,	0,	0,	1 };
}

inline
Matrix4 rotateY(float ang)
{
	ang = (ang* M_PI) / 180;

	return Matrix4{
	cosf(ang),	0,	sinf(ang),	0,
	0,	1,	0,	0,
	-sinf(ang),	0,	cosf(ang),	0,
	0,	0,	0,	1 };
}

inline
Matrix4 rotateZ(float ang)
{
	ang = (ang* M_PI) / 180;

	return Matrix4{
	cosf(ang),	-sinf(ang),	0,	0,
	sinf(ang),	cosf(ang),	0,	0,
	0,	0,	1,	0,
	0,	0,	0,	1 };

}

inline
Matrix4 rotateArbitrary(float ang, Vector3Df &u)
{
	u.normalize();

	ang = (ang* M_PI) / 180;
	return Matrix4{
		cosf(ang) + u.x*u.x*(1 - cosf(ang)), u.y*u.x*(1 - cosf(ang)) - u.z*sinf(ang),
			   u.z*u.x*(1 - cosf(ang)) + u.y*sinf(ang),	0,

		u.y*u.x*(1 - cosf(ang)) + u.z*sinf(ang), cosf(ang) + u.y*u.y*(1 - cosf(ang)),
			   u.z*u.y*(1 - cosf(ang)) - u.x*sinf(ang),	0,

		u.z*u.x*(1 - cosf(ang)) - u.y*sinf(ang), u.z*u.y*(1 - cosf(ang)) + u.x*sinf(ang),
			   cosf(ang) + u.z*u.z*(1 - cosf(ang)),	0,

		0,0,0,1
	};
}

struct float2 {
	float x, y;
};

struct Camera
{

	Vector3Df position;
	Vector3Df view;
	Vector3Df up;

	float2 resolution;
	float2 fov;

	void Transform(Matrix4 m)
	{
		view = m * view;
		up = m * up;
		position = m * position;
	}

	void Transform(Matrix4 m, Vector3Df reference)
	{
		Transform(translate(Vector3Df{ -reference.x, -reference.y, -reference.z }));

		Transform(m);

		Transform(translate(Vector3Df{ reference.x, reference.y, reference.z }));
	}

};


#endif