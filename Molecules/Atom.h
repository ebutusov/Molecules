#pragma once

class CAtom
{
public:
	CAtom(void);
	CAtom(TCHAR * symbol);
	void SetColor(GLfloat r, GLfloat g, GLfloat b);
	void GetColor(GLfloat *rColor);
	BOOL SetName(TCHAR *name);
	LPCTSTR GetName();
	LPCTSTR GetFullName();
	void SetScaledSize(GLfloat size);
	GLfloat GetScaledSize();
	GLfloat GetX() { return m_XCoord; }
	GLfloat GetY() { return m_YCoord; }
	GLfloat GetZ() { return m_ZCoord; }
	void SetCoords(GLfloat x, GLfloat y, GLfloat z);
	void GetCoords(GLfloat &x, GLfloat &y, GLfloat &z);
	void SetOutCoords(GLfloat x, GLfloat y, GLfloat z);
	void GetOutCoords(GLfloat &x, GLfloat &y, GLfloat &z);
	void SetCurrentCoords(GLfloat x, GLfloat y, GLfloat z);
	void GetCurrentCoords(GLfloat &x, GLfloat &y, GLfloat &z);
	void SetNormalAsCurrent();
	void SetOutAsCurrent();
	BOOL GetMoveStatus() { return m_IsOnTheMove; }
	GLfloat GetSpeed() { return m_fSpeed; }
	void SetSpeed(GLfloat speed) { m_fSpeed = speed; }
	void SetMoveStatus(BOOL s) { m_IsOnTheMove = s; }
	BOOL NearToOut(GLfloat tolerance);
	BOOL NearToNormal(GLfloat tolerance);
	GLfloat GetSize() { return m_Size; } 
  BOOL GetSkip() { return m_bSkip; }
  void SetSkip(BOOL skip) { m_bSkip = skip; }
	static void SizeLimits(float &max_size, float &min_size);
public:
	virtual ~CAtom() {};
private:
	// non copyable, non assignable, non moveable
	CAtom(CAtom &other);
	CAtom& operator=(CAtom &other);
	CAtom(CAtom &&other);
	CAtom& operator=(CAtom &&other);

	TCHAR m_Name[5];
	TCHAR m_FullName[20];

	BOOL m_IsOnTheMove;
  BOOL m_bSkip; // skip this atom during drawing;

	// atom coordinates
	GLfloat m_XCoord;
	GLfloat m_YCoord;
	GLfloat m_ZCoord;

	// "outer space" coordinates for explosion/implosion purposes
	GLfloat m_XCoordOut;
	GLfloat m_YCoordOut;
	GLfloat m_ZCoordOut;

	GLfloat m_fSpeed;	// implosion/explosion movement speed for this atom

	// current atom position
	GLfloat m_XCur;
	GLfloat m_YCur;
	GLfloat m_ZCur;

	GLfloat m_Size;
	GLfloat m_ScaledSize;

	GLfloat m_ColorR;
	GLfloat m_ColorG;
	GLfloat m_ColorB;

	typedef struct _ATOMDEFAULTS
	{
		TCHAR name[4];
		TCHAR full_name[20];
		GLfloat size;
		GLfloat color[4];
	} atom_defaults;

	// shared default atom parameters
	static atom_defaults m_AtomDefaults[];

	BOOL LoadDefaults();

};
