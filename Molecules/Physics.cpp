#include "StdAfx.h"
#include "math.h"
#include "Physics.h"

#define RANDOMSIGN() (rand()%2 > 0 ? 1 : -1)
#define COINTHROW(pool, val) ((rand()%pool+1) == val)
#define RANDX(x) !(rand()%x)

CTwister::CTwister(float max_speed)
{
	Init(max_speed);
}

CTwister::CTwister()
{
	Init(3.0f);
}

void CTwister::ResetState()
{
	Init(m_max_speed);
}

void
CTwister::Init(GLfloat max_speed)
{
	m_axis.SetValues(1.0f, 1.0f, 1.0f);
	m_axis_new = m_axis;
	/*m_axis[0] = 1.0f; m_axis[1] =  1.0f; m_axis[2] = 1.0f;
	m_axis_new[0] = 1.0f; m_axis_new[1] =  1.0f; m_axis_new[2] = 1.0f;*/
	m_rot.CreateFromAxisAngle(0.0f, 1.0f, 0.0f, 0.0f);
	m_interp_started = ::GetTickCount();
	m_max_speed = max_speed;
	m_acc = m_max_speed/10.0f;
	m_slowing_down = false;
	if (m_max_speed > 0.0f)
		m_speed = m_max_speed/3.0f;
}

void
CTwister::GetRotationMatrix(GLfloat *m)
{
  m_rot.CreateMatrix(m);
}

bool
CTwister::Interpolate(GLfloat delta)
{
	GLfloat step = 1.0f * delta;
	bool complete = true;
	for (int i=0;i<3;++i)
	{
		GLfloat dx = m_axis_new[i] - m_axis[i];
		if (fabs(dx) > step)
		{
			m_axis[i] += dx*step;
			complete = false;
		}
		else
			m_axis[i] = m_axis_new[i];
	}
	if (!complete)
		m_axis.Normalize();
	return complete;
}

void
CTwister::DoFreeRotation(DWORD delta)
{
	if (m_max_speed == 0.0f)
		return;

	GLfloat dt = (GLfloat)(delta/1000.0f);
	
	m_speed += m_acc * dt;

	CQuaternionf rx;
	rx.CreateFromAxisAngle(m_axis[0], m_axis[1], m_axis[2], m_speed);
	m_rot = rx * m_rot;

	bool complete = Interpolate(dt);
	bool invert_acc = false;

	if (m_speed > m_max_speed)
	{
		m_speed = m_max_speed;
		if (RANDX(200))
		{
			invert_acc = true;
			m_slowing_down = true;
		}
	}
	else if (m_speed < -m_max_speed)
	{
		m_speed = -m_max_speed;
		if (RANDX(200))
		{
			invert_acc = true;
			m_slowing_down = true;
		}
	}

	// it's about to stop, chance to change direction and acceleration
	if (fabs(m_speed) < 0.5f && m_slowing_down)
	{
		if (RANDX(100))
			invert_acc = true;

		// passing 0 point  - accelerating again
		m_slowing_down = false;
	}

	DWORD interp_running = (GetTickCount() - m_interp_started)/1000;
	
	//ATLASSERT(interp_running < 10);

	if (interp_running > 5 && complete || interp_running > 10)
	{
		m_axis_new[0] = RANDOMSIGN() * (GLfloat)(rand()%100+1)/100.0f;
		m_axis_new[1] = RANDOMSIGN() * (GLfloat)(rand()%100+1)/100.0f;
		m_axis_new[2] = RANDOMSIGN() * (GLfloat)(rand()%100+1)/100.0f;
		m_axis_new.Normalize();
		m_interp_started = ::GetTickCount();
	}

	if (invert_acc)
		m_acc *= -1;
}