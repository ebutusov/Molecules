#pragma once

#include "stdafx.h"
#include "Molecule.h"

class CMoleculeBuilder
{
public:
	CMoleculeBuilder(void) {};
	virtual ~CMoleculeBuilder(void) {};
	CMolecule* LoadFromFile(LPCTSTR filename);
private:
	void BuildMolecule(CMolecule *mol, TCHAR *filebuf);
	void ParseLine(TCHAR *line, CMolecule *mol);
};
