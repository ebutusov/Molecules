#include "StdAfx.h"
#include "MoleculeList.h"

CMoleculeList::CMoleculeList(void)
{
	m_nCurrentPosition = -1;
}

CMoleculeList::~CMoleculeList(void)
{
	if(m_Molecules.GetCount() > 0)
		m_Molecules.RemoveAll();
}

BOOL
CMoleculeList::LoadList()
{
	ProcessDir("molecules");	// moge tak, bo ustawilem katalog roboczy
	return m_Molecules.GetCount()>0 ? true : false;
}

void
CMoleculeList::ClearList()
{
	for(size_t i=0;i<m_Molecules.GetCount();i++)
		delete m_Molecules.GetAt(i);

	m_Molecules.RemoveAll();
	m_nCurrentPosition = -1;
}

LPCTSTR
CMoleculeList::GetNextMolecule()
{
	if(m_Molecules.GetCount() == 0)
		return NULL;
	if(m_nCurrentPosition < m_Molecules.GetCount()-1)
		return m_Molecules.GetAt(++m_nCurrentPosition);
	else
	{
		m_nCurrentPosition = 0;
		return m_Molecules.GetAt(0);
	}
}

LPCTSTR
CMoleculeList::GetRandomMolecule()
{
	if(m_Molecules.GetCount() == 0)
		return NULL;
	if(m_Molecules.GetCount() == 1)
		return m_Molecules.GetAt(0);
	srand(GetTickCount());
	m_nCurrentPosition = rand()%(m_Molecules.GetCount());
	return m_Molecules.GetAt(m_nCurrentPosition);
}

void
CMoleculeList::ProcessDir(LPCTSTR dir)
{
	CFindFile finder;
	CString pattern;
	pattern.Format("%s\\*.*", dir);
	if(finder.FindFile(pattern))
	{
		do
		{
			if(finder.IsDots())
				continue;
			if(finder.IsDirectory())
				ProcessDir((LPTSTR)(LPCSTR)finder.GetFilePath());
			else
			{
				CString path = finder.GetFilePath();
				int len = path.GetLength();
				if(len > 4 && path.Right(3) == "pdb")
				{
					LPTSTR file = new TCHAR[len+1];
					_tcsncpy_s(file, len+1, finder.GetFilePath(), len);
					this->m_Molecules.Add(file);		
				}
			}
		} while(finder.FindNextFileA());
	}
	finder.Close();
}