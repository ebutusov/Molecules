#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

template <typename T>
class Vector3d
{
public:
	Vector3d() : x(0.0), y(0.0), z(0.0) {};
	Vector3d(T xi, T yi, T zi): x(xi), y(yi), z(zi) {};
	Vector3d(const T *xyz): x(xyz[0]), y(xyz[1]), z(xyz[2]) {};

	T x, y, z;
	
	void SetValues(T xi, T yi, T zi)
	{
		x = xi;
		y = yi;
		z = zi;
	}

	inline T Magnitude()
	{
		return (T)sqrt(x*x + y*y + z*z);
	}

	void Normalize()
	{
		static T tol = 0.0001;
		T m = Magnitude();
		if(m <= tol) m = 1;
		x /= m;
		y /= m;
		z /= m;
		if(fabs(x) < tol) x = 0.0;
		if(fabs(y) < tol) y = 0.0;
		if(fabs(z) < tol) z = 0.0;
	}
	
	// angle between OY and vector
	T GetAlfa()
	{
		Vector3d<T> oy(0.0, 1.0, 0.0);
		Vector3d<T> v(x, y, z);
		v.Normalize();
		oy.Normalize();
		return acos(v.x * oy.x + v.y * oy.y + v.z * oy.z) * 180/M_PI;
	}

	// angle between OX and vector (it's projection to OXY)
	T GetGamma()
	{
		return atan2(z, x) * 180/M_PI;
	}

	inline void Reverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	inline T& operator[](const int index)
	{
		ATLASSERT(index >= 0 && index <= 3);
		switch(index)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		}
	}

	inline Vector3d<T>& operator+=(const Vector3d &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline Vector3d<T>& operator+=(T v)
	{
		x += v;
		y += v;
		z += v;
		return *this;
	}

	inline Vector3d<T>& operator-=(const Vector3d &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	inline Vector3d<T>& operator*=(T s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	inline Vector3d<T>& operator/=(T s)
	{
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	inline Vector3d<T> operator-()
	{
		return Vector3d<T>(-x, -y, -z);
	}
};

template <typename T>
class Vector2d
{
public:
	T x, y;
	Vector2d() : x(0.0), y(0.0) {};
	Vector2d(T xi, T yi): x(xi), y(yi) {};

	inline T Magnitude()
	{
		return (T)sqrt(x*x + y*y);
	}

	inline Vector2d<T>& operator*=(T s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	inline Vector2d<T>& operator/=(T s)
	{
		x /= s;
		y /= s;
		return *this;
	}

};

// ***
// vector 3D global operators
template <typename T>
inline Vector3d<T> operator+(const Vector3d<T>& u, const Vector3d<T>& v)
{
	return Vector3d<T>(u.x + v.x, u.y + v.y, u.z + v.z);
}

template <typename T>
inline Vector3d<T> operator-(const Vector3d<T>& u, const Vector3d<T>& v)
{
	return Vector3d<T>(u.x - v.x, u.y - v.y, u.z - v.z);
}

// cross product (the result is perpendicular to both u and v)
template <typename T>
inline Vector3d<T> operator^(const Vector3d<T>& u, const Vector3d<T>& v)
{
	/*return Vector3d(u.x * v.z - v.y * u.z,
									u.z * v.x - v.z * u.x,
									u.x * v.y - v.x * u.y);*/
	/*return Vector3d(
		u.x * v.z - u.z * v.y,
		-u.x * v.z + u.z * v.x,
		u.x * v.y - u.y * v.x);*/

	return Vector3d<T>(
	u.y*v.z - u.z*v.y,
		u.z*v.x - u.x*v.z,
		u.x*v.y - u.y*v.x);
}

// dot product - projection of vector u onto vector v
template <typename T>
inline T operator*(const Vector3d<T>& u, const Vector3d<T> &v)
{
	return (u.x * v.x + u.y * v.y + u.z * v.z);
}

// scalar multiplication & division
template <typename T>
inline Vector3d<T> operator*(const Vector3d<T>& u, float s)
{
	return Vector3d<T>(u.x*s, u.y*s, u.z*s);
}

template <typename T>
inline Vector3d<T> operator*(float s, const Vector3d<T>& u)
{
	return Vector3d<T>(u.x*s, u.y*s, u.z*s);
}

template <typename T>
inline Vector3d<T> operator/(const Vector3d<T>& u, float s)
{
	return Vector3d<T>(u.x/s, u.y/s, u.z/s);
}


// ***
// vector 2D global operators

template <typename T>
inline Vector2d<T> operator+(Vector2d<T> &u, Vector2d<T> &v)
{
	return Vector2d(u.x + v.x, u.y + v.y);
}

template <typename T>
inline Vector2d<T> operator-(Vector2d<T> &u, Vector2d<T> &v)
{
	return Vector2d(u.x - v.x, u.y - v.y);
}

template Vector3d<float>;
template Vector3d<double>;

typedef Vector3d<float> Vector3df;
typedef Vector3d<double> Vector3dd;