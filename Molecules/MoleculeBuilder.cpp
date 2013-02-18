#include "StdAfx.h"
#include "MoleculeBuilder.h"
#include "atlwfile.h"

CMoleculeBuilder::CMoleculeBuilder(void)
{
}

CMoleculeBuilder::~CMoleculeBuilder(void)
{
}

CMolecule*
CMoleculeBuilder::LoadFromFile(LPCTSTR filename)
{
	CFile f;
	if(filename == NULL)
		return NULL;
	if(f.Open(filename) != TRUE)
		return NULL;

	DWORD size = f.GetSize();
	m_FileBuff = new TCHAR[size+1];
	f.Read(m_FileBuff, size);
	m_FileBuff[size] = '\0';
	f.Close();
	CMolecule *mol = new CMolecule();
	BuildMolecule(mol);
	mol->CalculateBoundingBox();
	delete [] m_FileBuff;
	return mol->GetAtomsCount()>0 ? mol : NULL;
}

void
CMoleculeBuilder::BuildMolecule(CMolecule *mol)
{
	TCHAR delims[] = "\n";
	TCHAR *context;
	LPSTR sp = _tcstok_s(m_FileBuff, delims, &context);
	while(sp != NULL)
	{
		int len = _tcslen(sp);
		TCHAR *lineBuff = new TCHAR[len+1];
		TCHAR *bufptr = lineBuff;
		_tcsncpy_s(lineBuff, len+1, sp, len);
		ParseLine(lineBuff, mol);
		sp = _tcstok_s(NULL, delims, &context);
		delete [] bufptr;
	}
}

void
CMoleculeBuilder::ParseLine(TCHAR *line, CMolecule *mol)
{
	int len = _tcslen(line);
	if(len < 6)
		return;

	if(!_tcsncicmp("HETATM", line, 6) || !_tcsncicmp("ATOM", line, 4))
	{
		// got atom entry
		int number;
		TCHAR *name_wrk = new TCHAR[4];
		TCHAR *name = new TCHAR[4];
		if(1 != sscanf_s (line+7, " %d ", &number))
            MessageBox(NULL, "PARSE ERROR", "ERROR", MB_OK);
		_tcsncpy_s(name_wrk, 4, line+12, 3);
		name_wrk[3] = '\0';
		TCHAR *safe = name;
		for(int i=0;i<3;i++)
			if(!isspace(name_wrk[i]) && !isdigit(name_wrk[i]))
				*(name++) = name_wrk[i];
		*name = '\0';
		name = safe;
		GLfloat x = -999, y = -999, z = -999;
		if(3 != sscanf_s (line + 31, " %f %f %f ", &x, &y, &z))
			MessageBox(NULL, "COORD PARSE ERROR", "ERROR", MB_OK);

		mol->CreateAtom(x, y, z, name, number);
		delete [] name_wrk;
		delete [] name;
		return;
	}
	if(!_tcsncicmp("CONECT", line, 6))
  {
		int atoms[11];
    TCHAR buf[5];
		
		if(len < 11)	// CONECTsdddd (11 chars)
			return;

		line += 7;
		int i = 0;
		for(i=0;i<11;i++)
		{
			memset(buf, 0, 5);
			_tcsncpy_s(buf, 5, line, 4);
			buf[4] = '\0';
			int val = atoi(buf);
			if(val > 0)
			{
				atoms[i] = val;
				if(_tcslen(line) >= 9)		// 4 characters were read, 1 is the space
					line += 5;							// and we need at least 4 characters more to continue reading (4+1+4=9)
				else
				{
					i++;
					break;
				}
			}
			else
				break;
		}
		int j;
    for (j = 1; j < i; j++)
		{
      if(atoms[j] > 0)
				mol->PutLink(atoms[0], atoms[j]);
		}    
		return;
	}
	if(!_tcsncicmp("HEADER", line, 6))
	{
		if(_tcslen(line) < 11)
			return;
		mol->SetDescription(line+10);
	}
}