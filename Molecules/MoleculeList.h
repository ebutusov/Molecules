#pragma once
#include "stdafx.h"
#include "Molecule.h"

class CMoleculeList
{
public:
	CMoleculeList(void);
	BOOL LoadList();
	void ClearList();
	LPCTSTR GetRandomMolecule();
	LPCTSTR GetFistMolecule();
	LPCTSTR GetNextMolecule();
	LPCTSTR GetPrevMolecule();
	virtual ~CMoleculeList(void);
private:
	size_t m_nCurrentPosition;
	void ProcessDir(LPCTSTR dir);
	CAtlArray<LPCTSTR> m_Molecules;
};
