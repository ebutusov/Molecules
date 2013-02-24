#include "StdAfx.h"
#include "math.h"
#include "Physics.h"

#define RANDOMSIGN() (rand()%2 > 0 ? 1 : -1)
#define COINTHROW(pool, val) ((rand()%pool+1) == val)

CTwister::CTwister()
{
	m_axis[0] = 1.0f; m_axis[1] =  1.0f; m_axis[2] = 1.0f;
	m_axis_new[0] = 1.0f; m_axis_new[1] =  1.0f; m_axis_new[2] = 1.0f;
	m_rot.CreateFromAxisAngle(0.0f, 1.0f, 0.0f, 0.0f);
	Init(3.0f, 1.0f, 0.2f);
	m_speed = m_max_speed / 2;
	m_last_changed_acc = ::GetTickCount();
}

void
CTwister::Init(GLfloat max_speed, GLfloat min_speed, GLfloat acceleration)
{
	m_speed = m_max_speed = max_speed;
	m_min_speed = min_speed;
	m_acc = acceleration;
}

void
CTwister::GetRotationMatrix(GLfloat *m)
{
  m_rot.CreateMatrix(m);
}

bool
CTwister::Interpolate()
{
	static GLfloat step = 0.01f;
	bool complete = true;
	for (int i=0;i<2;++i)
	{
		GLfloat dx = m_axis_new[i] - m_axis[i];
		if (fabs(dx) > step)
		{
			m_axis[i] += dx*step;
			complete = false;
		}
	}
	return complete;
}

void
CTwister::DoFreeRotation2(DWORD delta)
{
	m_speed += m_acc * (float)(delta/1000.0f);
	if (m_speed > 360.0f)
		m_speed = m_speed - 360.0f;
	CQuaternionf rx;
	rx.CreateFromAxisAngle(m_axis[0], m_axis[1], m_axis[2], m_speed);
	m_rot = rx * m_rot;

	Interpolate();

	bool invert_acc = false;

	if (m_speed > m_max_speed|| m_speed < m_min_speed)
		invert_acc = true;

	if (::GetTickCount()/1000 - m_last_changed_acc/1000 > 2)
	{
		m_axis_new[0] = RANDOMSIGN() * (GLfloat)(rand()%100+1)/100.0f;
		m_axis_new[1] = RANDOMSIGN() * (GLfloat)(rand()%100+1)/100.0f;
		m_axis_new[2] = RANDOMSIGN() * (GLfloat)(rand()%100+1)/100.0f;
		m_last_changed_acc = ::GetTickCount();
	}

	if (invert_acc)
		m_acc *= -1;
}