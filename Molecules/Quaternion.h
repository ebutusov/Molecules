// glQuaternion.h: interface for the glQuaternion class.
#pragma once
#include <math.h>

#define PI			3.14159265358979323846

template <typename T>
class CQuaternion
{
private:
	T m_x, m_y, m_z, m_w;
public:  
	CQuaternion() :
		m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(0.0f) { };

	CQuaternion(T x, T y, T z, T w):
		m_x(x), m_y(y), m_z(z), m_w(w) { };

	virtual ~CQuaternion() {};

	void CreateWA(T *arr4)
	{
		m_w = arr4[0];
		m_x = arr4[1];
		m_y = arr4[2];
		m_z = arr4[3];
	}


	void CreateW(T w, T x, T y, T z)
	{
		m_x = x;
		m_y = y;
		m_z = z;
		m_w = w;
		Normalize();
	}

	void Create(T x, T y, T z, T w)
	{
		m_x = x;
		m_y = y;
		m_z = z;
		m_w = w;
		Normalize();
	}

	void CreateA(T *arr4)
	{
		m_x = arr4[0];
		m_y = arr4[1];
		m_z = arr4[2];
		m_w = arr4[3];
	}

	void GetQuaternionW(T *out) const
	{
		out[0] = m_w;
		out[1] = m_x;
		out[2] = m_y;
		out[3] = m_z;
	}

	void GetQuaternion(T *out) const
	{
		out[0] = m_x;
		out[1] = m_y;
		out[2] = m_z;
		out[3] = m_w;
	}

	void Normalize()
	{
		const T TOLERANCE = 0.0001f;
		T mag2 = m_w * m_w + m_x * m_x + m_y * m_y + m_z * m_z;
		if (fabs(mag2 - 1.0f) > TOLERANCE)
		{
			T mag = sqrt(mag2);
			m_w /= mag;
			m_x /= mag;
			m_y /= mag;
			m_z /= mag;
		}
	}

	static CQuaternion<T> NewFromAxisAndAngle(T x, T y, T z, T degrees)
	{
		CQuaternion<T> q;
		q.CreateFromAxisAngle(x, y, z, degrees);
		return q;
	}

	void CreateFromAxisAngle(T x, T y, T z, T degrees)
	{
		// First we want to convert the degrees to radians 
		// since the angle is assumed to be in radians
		T angle = T((degrees / 180.0f) * PI);

		// Here we calculate the sin( theta / 2) once for optimization
		T result = (T)sin( angle / 2.0f );
			
		// Calcualte the w value by cos( theta / 2 )
		m_w = (T)cos( angle / 2.0f );

		// Calculate the x, y and z of the quaternion
		m_x = T(x * result);
		m_y = T(y * result);
		m_z = T(z * result);

		Normalize();
	}

	CQuaternion<T> operator *(const CQuaternion<T> &q)
	{
		CQuaternion<T> r;

		r.m_w = m_w*q.m_w - m_x*q.m_x - m_y*q.m_y - m_z*q.m_z;
		r.m_x = m_w*q.m_x + m_x*q.m_w + m_y*q.m_z - m_z*q.m_y;
		r.m_y = m_w*q.m_y + m_y*q.m_w + m_z*q.m_x - m_x*q.m_z;
		r.m_z = m_w*q.m_z + m_z*q.m_w + m_x*q.m_y - m_y*q.m_x;
		
		r.Normalize();
		return(r);
	}

	void CreateMatrix(T *pMatrix)
	{
		// Make sure the matrix has allocated memory to store the rotation data
		if(!pMatrix) return;

		// First row
		pMatrix[ 0] = 1.0f - 2.0f * ( m_y * m_y + m_z * m_z ); 
		pMatrix[ 1] = 2.0f * (m_x * m_y + m_z * m_w);
		pMatrix[ 2] = 2.0f * (m_x * m_z - m_y * m_w);
		pMatrix[ 3] = 0.0f;  

		// Second row
		pMatrix[ 4] = 2.0f * ( m_x * m_y - m_z * m_w );  
		pMatrix[ 5] = 1.0f - 2.0f * ( m_x * m_x + m_z * m_z ); 
		pMatrix[ 6] = 2.0f * (m_z * m_y + m_x * m_w );  
		pMatrix[ 7] = 0.0f;  

		// Third row
		pMatrix[ 8] = 2.0f * ( m_x * m_z + m_y * m_w );
		pMatrix[ 9] = 2.0f * ( m_y * m_z - m_x * m_w );
		pMatrix[10] = 1.0f - 2.0f * ( m_x * m_x + m_y * m_y );  
		pMatrix[11] = 0.0f;  

		// Fourth row
		pMatrix[12] = 0;  
		pMatrix[13] = 0;  
		pMatrix[14] = 0;  
		pMatrix[15] = 1.0f;

		// Now pMatrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
	}
};


template CQuaternion<float>;
template CQuaternion<double>;

typedef CQuaternion<float> CQuaternionf;
typedef CQuaternion<double> CQuaterniond;