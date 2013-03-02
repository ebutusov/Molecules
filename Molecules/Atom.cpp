#include "StdAfx.h"
#include "Atom.h"
#include <math.h>

CAtom::atom_defaults CAtom::m_AtomDefaults[] = 
{
	{ _T("H"),	_T("Hydrogen"), 1.17f, /* white */ { 0.7f, 0.7f, 0.7f, 1.0f } },
	{ _T("C"),	_T("Carbon"), 1.75f, /* gray */ { 0.5f, 0.5f, 0.5f, 1.0f } },
	{ _T("CA"),	_T("Calcium"), 1.80f, /* blue */ { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ _T("N"),	_T("Nitrogen"), 1.55f, /* violet */ { 0.5f, 0.0f, 1.0f, 1.0f } },
	{ _T("O"),	_T("Oxygen"), 1.40f, /* red */ { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ _T("P"),	_T("Phosphorus"), 1.28f, /* purple */ { 0.8f, 0.2f, 0.7f, 1.0f } },
	{ _T("S"),	_T("Sulfur"),   1.80f, /* yellow */ { 1.0f, 0.9f, 0.0f, 1.0f } },
	{ _T("Cl"),	_T("Chlorine"), 0.97f, /* light green */ { 0.2f, 0.8f, 0.2f, 1.0f } },
	{ _T("F"),	_T("Fluorine"), 0.57f, /* cyan */ { 0.0f, 0.8f, 0.8f, 1.0f } }
};

CAtom::CAtom(TCHAR *name)
{
	m_XCoord = 0.0f;
	m_YCoord = 0.0f;
	m_ZCoord = 0.0f;
	m_ScaledSize = 1.0f;
	_tcsncpy_s(m_Name, name, 3);
  m_bSkip = false;
	memset(m_FullName, 0, sizeof(m_FullName));
	// TODO: check if defauts were loaded, if not, remove this atom
	// and its links from the molecule
	LoadDefaults();
	m_IsOnTheMove = false;
}

CAtom::CAtom(void)
{
	m_XCoord = 0.0f;
	m_YCoord = 0.0f;
	m_ZCoord = 0.0f;
  m_bSkip = false;
	m_ScaledSize = 1.0f;
	_tcsncpy_s(m_Name, _T("?"), 3);
	_tcsncpy_s(m_FullName, 20, _T("UNKNOWN"), 7);
}

bool CAtom::SetName(TCHAR *name)
{
	_tcsncpy_s(m_Name, name, 3);
	return LoadDefaults();
}

LPCTSTR CAtom::GetName()
{
	return m_Name;
}

LPCTSTR CAtom::GetFullName()
{
	return m_FullName;
}


void CAtom::SetCoords(GLfloat x, GLfloat y, GLfloat z)
{
	m_XCur = m_XCoordOut = m_XCoord = x;
	m_YCur = m_YCoordOut = m_YCoord = y;
	m_ZCur = m_ZCoordOut = m_ZCoord = z;
}

void CAtom::GetCoords(GLfloat &x, GLfloat &y, GLfloat &z)
{
	x = m_XCoord;
	y = m_YCoord;
	z = m_ZCoord;
}

void CAtom::GetOutCoords(GLfloat &x, GLfloat &y, GLfloat &z)
{
	x = m_XCoordOut;
	y = m_YCoordOut;
	z = m_ZCoordOut;
}

void CAtom::SetOutCoords(GLfloat x, GLfloat y, GLfloat z)
{
	m_XCoordOut = x;
	m_YCoordOut = y;
	m_ZCoordOut = z;
}

void CAtom::GetCurrentCoords(GLfloat &x, GLfloat &y, GLfloat &z)
{
	x = m_XCur; y = m_YCur; z = m_ZCur;
}

void CAtom::SetCurrentCoords(GLfloat x, GLfloat y, GLfloat z)
{
	m_XCur = x; m_YCur = y; m_ZCur = z;
}


void CAtom::SetColor(GLfloat r, GLfloat g, GLfloat b)
{
	m_ColorR = r;
	m_ColorG = g;
	m_ColorB = b;
}

void CAtom::GetColor(GLfloat *rColor)
{
	rColor[0] = m_ColorR;
	rColor[1] = m_ColorG;
	rColor[2] = m_ColorB;
}

void CAtom::SetScaledSize(GLfloat size)
{
	m_ScaledSize = size;
}

GLfloat CAtom::GetScaledSize()
{
	return m_ScaledSize;
}

void CAtom::SetNormalAsCurrent()
{
	m_XCur = m_XCoord;
	m_YCur = m_YCoord;
	m_ZCur = m_ZCoord;
}

void CAtom::SetOutAsCurrent()
{
	m_XCur = m_XCoordOut;
	m_YCur = m_YCoordOut;
	m_ZCur = m_ZCoordOut;
}

bool CAtom::NearToNormal(GLfloat tolerance)
{
	GLfloat xs = m_XCur - m_XCoord;
	xs *= xs;
	GLfloat ys = m_YCur - m_YCoord;
	ys *= ys;
	GLfloat zs = m_ZCur - m_ZCoord;
	zs *= zs;
	GLfloat dist = sqrt(xs+ys+zs);
	if(dist <= tolerance)
		return true;
	else
		return false;
}

bool CAtom::NearToOut(GLfloat tolerance)
{
	GLfloat xs = m_XCur - m_XCoordOut;
	xs *= xs;
	GLfloat ys = m_YCur - m_YCoordOut;
	ys *= ys;
	GLfloat zs = m_ZCur - m_ZCoordOut;
	zs *= zs;
	GLfloat dist = sqrt(xs+ys+zs);
	if(dist <= tolerance)
		return true;
	else
		return false;
}

void CAtom::SizeLimits(float &max_size, float &min_size)
{
	max_size = 100.0f;
	atom_defaults *p = m_AtomDefaults;
	max_size = min_size = p->size;
	int i = 0;
	while (i++ < _countof(m_AtomDefaults))
	{
		min_size = min(p->size, min_size);
		max_size = max(p->size, max_size);
		++p;
	}
}


bool CAtom::LoadDefaults()
{
	int len = _tcslen(m_Name);
	if(len == 0)
	{
		_tcsncpy_s(m_Name, _T("???"), 3);
		return false;
	}

	bool found = false;
	for(int i=0;i<_countof(m_AtomDefaults);i++)
	{
		int deflen = _tcslen(m_AtomDefaults[i].name);
		int namelen = _tcslen(m_Name);
		if(deflen == namelen
			&& !_tcsncicmp(m_AtomDefaults[i].name, m_Name, deflen > namelen ? namelen : deflen))
		{
			found = true;
			_tcscpy_s(m_FullName, 20, m_AtomDefaults[i].full_name);
			m_Size = m_AtomDefaults[i].size;
			m_ColorR = m_AtomDefaults[i].color[0];
			m_ColorG = m_AtomDefaults[i].color[1];
			m_ColorB = m_AtomDefaults[i].color[2];
		}	
	}
	if(!found)
	{
		if (len < 4)
			_tcscat_s(m_Name, _T("*")); 
		//_tcsncpy_s(m_Name, 4, _T("?"), 3);
		_tcsncpy_s(m_FullName, 20, _T("UNKNOWN"), 7);
		m_Size = 1.0f;
		m_ColorR = 0.0f;
		m_ColorG = 1.0f;
		m_ColorB = 0.0f;
	}
	return found;
}