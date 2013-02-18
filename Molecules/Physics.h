#pragma once

class CTwister
{
public:
	CTwister();
	virtual ~CTwister();

	void Init(GLfloat xmax, GLfloat ymax, GLfloat zmax);
	void DoFreeRotation();
	void ResetState();
	void GetRotation(GLfloat &xang, GLfloat &yang, GLfloat &zang);

private:
	GLfloat m_fXAngle, m_fYAngle, m_fZAngle;
	GLfloat m_fXSpeed, m_fYSpeed, m_fZSpeed;
	GLfloat m_fXSpeedMax, m_fYSpeedMax, m_fZSpeedMax;
	GLfloat m_fXAcc, m_fYAcc, m_fZAcc;
	int m_xdir, m_ydir, m_zdir;
	typedef enum __MOTIONSTATE { msAccelerate, msBrake, msStop } MotionState;
	MotionState m_State;
};