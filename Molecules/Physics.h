#pragma once
#include "Quaternion.h"

class CTwister
{
public:
	CTwister();
	virtual ~CTwister() {};

	void Init(GLfloat max_speed, GLfloat acceleration);
	void DoFreeRotation(DWORD delta);
	void ResetState();
	void CTwister::GetRotationMatrix(GLfloat *m);

private:
	bool m_slowing_down;
	GLfloat m_max_speed;
	CQuaternionf m_rot;
	DWORD m_last_changed_acc;
	GLfloat m_axis[3], m_axis_new[3];
	GLfloat m_speed, m_acc;
	bool Interpolate(GLfloat delta);
};