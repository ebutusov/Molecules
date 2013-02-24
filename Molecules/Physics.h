#pragma once
#include "Quaternion.h"

class CTwister
{
public:
	CTwister();
	virtual ~CTwister() {};

	void Init(GLfloat xmax, GLfloat ymax, GLfloat zmax);
	void DoFreeRotation();
	void DoFreeRotation2(DWORD delta);
	void ResetState();
	void GetRotation(GLfloat &xang, GLfloat &yang, GLfloat &zang);
	void CTwister::GetRotationMatrix(GLfloat *m);

private:
	GLfloat m_min_speed, m_max_speed;
	CQuaternionf m_rot;
	DWORD m_last_changed_acc;
	GLfloat m_axis[3], m_axis_new[3];
	GLfloat m_speed, m_acc;
	bool Interpolate();
};