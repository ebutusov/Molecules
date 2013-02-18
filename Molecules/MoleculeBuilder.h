#pragma once

#include "stdafx.h"
#include "Molecule.h"

class CMoleculeBuilder
{
public:
	CMoleculeBuilder(void);
public:
	virtual ~CMoleculeBuilder(void);
	CMolecule* LoadFromFile(LPCTSTR filename);
private:
	TCHAR *m_FileBuff;
	void BuildMolecule(CMolecule *mol);
	void ParseLine(TCHAR *line, CMolecule *mol);
};
