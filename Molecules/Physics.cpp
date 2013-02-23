#include "StdAfx.h"
#include "math.h"
#include "Physics.h"

#define RANDOMSIGN() (rand()%2 > 0 ? 1 : -1)
#define COINTHROW(pool, val) ((rand()%pool+1) == val)

CTwister::CTwister()
{
	m_fXAngle = m_fYAngle = m_fZAngle = 0.0f;
	m_fXSpeed = m_fYSpeed = m_fZSpeed = 0.0f;
	m_fXSpeedMax = m_fYSpeedMax = m_fZSpeedMax = 0.0f;
	m_fXAcc = m_fYAcc = m_fZAcc = 0.0f;
	m_xdir = RANDOMSIGN();
	m_ydir = RANDOMSIGN();
	m_zdir = RANDOMSIGN();
}
CTwister::~CTwister()
{
}

void
CTwister::Init(GLfloat xmax, GLfloat ymax, GLfloat zmax)
{
	m_fXSpeed = m_fXSpeedMax = xmax;
	m_fYSpeed = m_fYSpeedMax = ymax;
	m_fZSpeed = m_fZSpeedMax = zmax;

	// small initial accelerations
	m_fXAcc = (m_fXSpeedMax/100)*((rand()%5)+1);
	m_fYAcc = (m_fYSpeedMax/100)*((rand()%5)+1);
	m_fZAcc = (m_fZSpeedMax/100)*((rand()%5)+1);
}

void
CTwister::GetRotation(GLfloat &xang, GLfloat &yang, GLfloat &zang)
{
	xang = m_fXAngle;
	yang = m_fYAngle;
	zang = m_fZAngle;
}

void
CTwister::DoFreeRotation2(DWORD delta)
{
	float speed = 180.0f; // per second
	float acc = 0.01f; // acceleration

	speed *= (float)delta/1000; 

	// normalization
	m_fXAngle += speed;
	m_fYAngle += speed;
	//m_fZAngle += speed;

	/*if(m_fXAngle > 360.0f)
		m_fXAngle = m_fXAngle - 360.0f;
	else if(m_fXAngle < -360.0f)
		m_fXAngle = m_fXAngle+360.0f;
	m_fYAngle += speed;
	if(m_fYAngle > 360.0f)
		m_fYAngle = m_fYAngle - 360.0f;
	else if(m_fYAngle < -360.0f)
		m_fYAngle = m_fYAngle+ 360.0f;
	m_fZAngle += speed;
	if(m_fZAngle > 360.0f)
		m_fZAngle = m_fZAngle - 360.0f;
	else if(m_fZAngle < -360.0f)
		m_fZAngle = m_fZAngle+360.0f;*/
}

void
CTwister::DoFreeRotation()
{
	m_fXAcc = (m_fXSpeedMax/100)*((rand()%5)+1);
	m_fYAcc = (m_fYSpeedMax/100)*((rand()%5)+1);
	m_fZAcc = (m_fZSpeedMax/100)*((rand()%5)+1);
	if(fabs(m_fXSpeed) < m_fXSpeedMax)
		m_fXSpeed += m_xdir*m_fXAcc;
	else if(COINTHROW(200,1))
	{
		m_xdir = -m_xdir;
		
		m_fXSpeed += m_xdir*m_fXAcc;
	}
	if(fabs(m_fYSpeed) < m_fYSpeedMax)
		m_fYSpeed += m_ydir*m_fYAcc;
	else if(COINTHROW(200, 1))
	{
		m_ydir = -m_ydir;
		m_fYSpeed += m_ydir*m_fYAcc;
	}
	if(fabs(m_fZSpeed) < m_fZSpeedMax)
		m_fZSpeed += m_zdir*m_fZAcc;
	else if(COINTHROW(200, 1))
	{	
		m_zdir = -m_zdir;
		m_fZSpeed += m_zdir*m_fZAcc;
	}

	m_fXAngle += m_fXSpeed;
	if(m_fXAngle > 360.0f)
		m_fXAngle = m_fXAngle - 360.0f;
	else if(m_fXAngle < -360.0f)
		m_fXAngle = m_fXAngle+360.0f;
	m_fYAngle += m_fYSpeed;
	if(m_fYAngle > 360.0f)
		m_fYAngle = m_fYAngle - 360.0f;
	else if(m_fYAngle < -360.0f)
		m_fYAngle = m_fYAngle+ 360.0f;
	m_fZAngle += m_fZSpeed;
	if(m_fZAngle > 360.0f)
		m_fZAngle = m_fZAngle - 360.0f;
	else if(m_fZAngle < -360.0f)
		m_fZAngle = m_fZAngle+360.0f;

	if(COINTHROW(50, 25))
	{
		m_xdir = RANDOMSIGN();
		m_ydir = RANDOMSIGN();
		m_zdir = RANDOMSIGN();
	}

}