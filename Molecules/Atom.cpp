#include "StdAfx.h"
#include "Atom.h"
#include <math.h>

CAtom::atom_defaults CAtom::m_AtomDefaults[] = 
{
	{ "H", "Hydrogen", 1.17f, { 0.7f, 0.7f, 0.7f, 1.0f } },	// white
	{ "C", "Carbon", 1.75f, { 0.5f, 0.5f, 0.5f, 1.0f } },		// gray
	{ "CA", "Calcium", 1.80f, { 0.0f, 0.0f, 1.0f, 1.0f } },	// blue
	{ "N", "Nitrogen", 1.55f, { 0.5f, 0.0f, 1.0f, 1.0f } },	// violet ???
	{ "O", "Oxygen", 1.40f, { 1.0f, 0.0f, 0.0f, 1.0f } },		// red
	{ "P", "Phosphor", 1.28f, { 0.8f, 0.2f, 0.7f } },				// purple ???
	{ "S", "Siarka",   1.80f, { 1.0f, 0.9f, 0.0f } }					// yellow ???
};

CAtom::CAtom(TCHAR *name)
{
	m_XCoord = 0.0f;
	m_YCoord = 0.0f;
	m_ZCoord = 0.0f;
	m_ScaledSize = 1.0f;
	strncpy_s(m_Name, 4, name, 3);
  m_bSkip = FALSE;
	// TODO: check if defauts were loaded, if not, remove this atom
	// and its links from the molecule
	LoadDefaults();
	m_IsOnTheMove = FALSE;
}

CAtom::CAtom(void)
{
	m_XCoord = 0.0f;
	m_YCoord = 0.0f;
	m_ZCoord = 0.0f;
  m_bSkip = FALSE;
	m_ScaledSize = 1.0f;
	_tcsncpy_s(m_Name, 4, "UNK", 3);
	_tcsncpy_s(m_FullName, 20, "UNKNOWN", 7);
}

CAtom::~CAtom(void)
{
}

BOOL
CAtom::SetName(TCHAR *name)
{
	strncpy_s(m_Name, 4, name, 3);
	return LoadDefaults();
}

void
CAtom::SetCoords(GLfloat x, GLfloat y, GLfloat z)
{
	m_XCur = m_XCoordOut = m_XCoord = x;
	m_YCur = m_YCoordOut = m_YCoord = y;
	m_ZCur = m_ZCoordOut = m_ZCoord = z;
}

void
CAtom::GetCoords(GLfloat &x, GLfloat &y, GLfloat &z)
{
	x = m_XCoord;
	y = m_YCoord;
	z = m_ZCoord;
}

void
CAtom::GetOutCoords(GLfloat &x, GLfloat &y, GLfloat &z)
{
	x = m_XCoordOut;
	y = m_YCoordOut;
	z = m_ZCoordOut;
}

void
CAtom::SetOutCoords(GLfloat x, GLfloat y, GLfloat z)
{
	m_XCoordOut = x;
	m_YCoordOut = y;
	m_ZCoordOut = z;
}

void
CAtom::GetCurrentCoords(GLfloat &x, GLfloat &y, GLfloat &z)
{
	x = m_XCur; y = m_YCur; z = m_ZCur;
}

void
CAtom::SetCurrentCoords(GLfloat x, GLfloat y, GLfloat z)
{
	m_XCur = x; m_YCur = y; m_ZCur = z;
}


void
CAtom::SetColor(GLfloat r, GLfloat g, GLfloat b)
{
	m_ColorR = r;
	m_ColorG = g;
	m_ColorB = b;
}

void
CAtom::GetColor(GLfloat *rColor)
{
	rColor[0] = m_ColorR;
	rColor[1] = m_ColorG;
	rColor[2] = m_ColorB;
}

void
CAtom::SetScaledSize(GLfloat size)
{
	m_ScaledSize = size;
}

GLfloat
CAtom::GetScaledSize()
{
	return m_ScaledSize;
}

void
CAtom::SetNormalAsCurrent()
{
	m_XCur = m_XCoord;
	m_YCur = m_YCoord;
	m_ZCur = m_ZCoord;
}

void
CAtom::SetOutAsCurrent()
{
	m_XCur = m_XCoordOut;
	m_YCur = m_YCoordOut;
	m_ZCur = m_ZCoordOut;
}

BOOL
CAtom::NearToNormal(GLfloat tolerance)
{
	GLfloat xs = m_XCur - m_XCoord;
	xs *= xs;
	GLfloat ys = m_YCur - m_YCoord;
	ys *= ys;
	GLfloat zs = m_ZCur - m_ZCoord;
	zs *= zs;
	GLfloat dist = sqrt(xs+ys+zs);
	if(dist <= tolerance)
		return TRUE;
	else
		return FALSE;
}

BOOL
CAtom::NearToOut(GLfloat tolerance)
{
	GLfloat xs = m_XCur - m_XCoordOut;
	xs *= xs;
	GLfloat ys = m_YCur - m_YCoordOut;
	ys *= ys;
	GLfloat zs = m_ZCur - m_ZCoordOut;
	zs *= zs;
	GLfloat dist = sqrt(xs+ys+zs);
	if(dist <= tolerance)
		return TRUE;
	else
		return FALSE;
}

BOOL
CAtom::LoadDefaults()
{
	if(m_Name == NULL || strlen(m_Name) == 0)
		return FALSE;

	BOOL found = FALSE;
	for(int i=0;i<_countof(m_AtomDefaults);i++)
	{
		int deflen = _tcslen(m_AtomDefaults[i].name);
		int namelen = _tcslen(m_Name);
		if(deflen == namelen
			&& !_tcsncicmp(m_AtomDefaults[i].name, m_Name, deflen > namelen ? namelen : deflen))
		{
			found = TRUE;
			_tcscpy_s(m_FullName, 20, m_AtomDefaults[i].full_name);
			this->m_Size = m_AtomDefaults[i].size;
			this->m_ColorR = m_AtomDefaults[i].color[0];
			this->m_ColorG = m_AtomDefaults[i].color[1];
			this->m_ColorB = m_AtomDefaults[i].color[2];
		}	
	}
	if(!found)
	{
		_tcsncpy(m_Name, "UNK", 3);
		_tcsncpy(m_FullName, "UNKNOWN", 7);
		this->m_Size = 1.0f;
		this->m_ColorR = 0.0f;
		this->m_ColorG = 1.0f;
		this->m_ColorB = 0.0f;
	}
	return found;
}