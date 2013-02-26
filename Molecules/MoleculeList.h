#pragma once
#include "stdafx.h"
#include "Molecule.h"
#include <vector>

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
	std::vector<LPCTSTR> m_Molecules;
};
