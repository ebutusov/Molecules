#include "StdAfx.h"
#include "MoleculeBuilder.h"
#include "atlwfile.h"
#include <memory>

#define BOM L'\xFEFF' 

TCHAR* ReadFile(LPCTSTR filename)
{
	USES_CONVERSION;
	CFile f;
	if(filename == NULL)
		return NULL;
	if(f.Open(filename) != TRUE)
		return NULL;

	DWORD size = f.GetSize();
	if (size == 0) return NULL;

	wchar_t mark;
	TCHAR *fptr = NULL, *ret = NULL;

	f.Read(&mark, sizeof(wchar_t));
	if (mark == BOM)
	{
		/* unicode file XXX NOT TESTED */
		std::unique_ptr<wchar_t[]> fb(new wchar_t[size+1]);
		f.Read(fb.get(), size - sizeof(wchar_t));
		fb[(size - sizeof(wchar_t)) / sizeof(wchar_t)] = L'\0';
		fptr = W2T(fb.get());
		size_t len = _tcslen(fptr);
		ret = new TCHAR[len+1];
		_tcsncpy_s(ret, len+1, fptr, len);
	}
	else
	{
		// ansi file
		f.Seek(0,FILE_BEGIN); // read from the begi
		std::unique_ptr<char[]> fb(new char[size+1]);
		f.Read(fb.get(), size);
		fb[size] = '\0';
		fptr = A2T(fb.get());
		size_t len = _tcslen(fptr);
		ret = new TCHAR[len+1];
		_tcsncpy_s(ret, len+1, fptr, len);
	}
	
	return ret; // caller must free this!
}

CMolecule*
CMoleculeBuilder::LoadFromFile(LPCTSTR filename)
{
	TCHAR *fptr = ReadFile(filename);
	std::unique_ptr<TCHAR[]> ufptr(fptr);
	std::unique_ptr<CMolecule> mol(new CMolecule());
	BuildMolecule(mol.get(), fptr);
	if (mol->GetAtomsCount() > 0)
		return mol.release();
	else
		return NULL;

	//std::unique_ptr<TCHAR[]> filebuff(new TCHAR[size+1]);
	//TCHAR *fb = filebuff.get();
	//f.Read(fb, size);
	//fb[size] = '\0';
	//f.Close();
	//CMolecule *mol = new CMolecule();
	//BuildMolecule(mol, fb);
	//return mol->GetAtomsCount()>0 ? mol : NULL;
}

void
CMoleculeBuilder::BuildMolecule(CMolecule *mol, TCHAR *filebuf)
{
	TCHAR delims[] = _T("\n");
	TCHAR *context;
	LPTSTR sp = _tcstok_s(filebuf, delims, &context);
	while(sp != NULL)
	{
		int len = _tcslen(sp);
		std::unique_ptr<TCHAR[]> lineBuff(new TCHAR[len+1]);
		TCHAR *bufptr = lineBuff.get();
		_tcsncpy_s(lineBuff.get(), len+1, sp, len);
		ParseLine(lineBuff.get(), mol);
		sp = _tcstok_s(NULL, delims, &context);
	}
	mol->CalculateBoundingBox();
}

void
CMoleculeBuilder::ParseLine(TCHAR *line, CMolecule *mol)
{
	int len = _tcslen(line);
	if(len < 6)
		return;

	int scanned = 0;

	if(!_tcsncicmp(_T("HETATM"), line, 6) || !_tcsncicmp(_T("ATOM"), line, 4))
	{
		// got atom entry
		int number;
		TCHAR name_wrk[4], name[4];

		scanned = _stscanf_s(line+7, _T(" %d "), &number);
		ATLASSERT(scanned == 1);
		_tcsncpy_s(name_wrk, 4, line+12, 3);
		name_wrk[3] = '\0';
		TCHAR *it = name;
		for(int i=0;i<3;i++)
			if(!isspace(name_wrk[i]) && !isdigit(name_wrk[i]))
				*(it++) = name_wrk[i];
		*it = '\0';
		GLfloat x = -999, y = -999, z = -999;
		scanned = _stscanf_s (line + 31, _T(" %f %f %f "), &x, &y, &z);
		ATLASSERT(scanned == 3);
		mol->CreateAtom(x, y, z, name, number);
		return;
	}
	else if(!_tcsncicmp(_T("CONECT"), line, 6))
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
			int val = _ttoi(buf);
			if(val > 0)
			{
				atoms[i] = val;
				if(_tcslen(line) >= 9)		// 4 characters were read, 1 is the space
					line += 5;							// and we need at least 4 characters more to continue reading (4+1+4=9)
				else
				{
					// nothing more to read
					++i;
					break;
				}
			}
			else
				break;
		}
    for (int j = 1; j < i; j++)
		{
      if(atoms[j] > 0)
				mol->PutLink(atoms[0], atoms[j]);
		}    
		return;
	}
	if(!_tcsncicmp(_T("HEADER"), line, 6))
	{
		if(_tcslen(line) < 11)
			return;
		mol->SetDescription(line+10);
	}
}