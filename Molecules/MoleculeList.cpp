#include "StdAfx.h"
#include "MoleculeList.h"

CMoleculeList::CMoleculeList(void)
{
	m_nCurrentPosition = -1;
}

CMoleculeList::~CMoleculeList(void)
{
	ClearList();
}

BOOL
CMoleculeList::LoadList()
{
	ProcessDir(_T("molecules"));	// this works because of changed working directory
	return m_Molecules.size() > 0 ? true : false;
}

void
CMoleculeList::ClearList()
{
	for (auto it=std::begin(m_Molecules);it != std::end(m_Molecules); ++it)
		delete *it;
	m_Molecules.clear();
	m_nCurrentPosition = -1;
}

LPCTSTR
CMoleculeList::GetNextMolecule()
{
	if(m_Molecules.size() == 0)
		return NULL;
	if(m_nCurrentPosition < m_Molecules.size() - 1)
		return m_Molecules[++m_nCurrentPosition];
	else
	{
		m_nCurrentPosition = 0;
		return m_Molecules[0];
	}
}

LPCTSTR
CMoleculeList::GetRandomMolecule()
{
	if(m_Molecules.size() == 0)
		return NULL;
	if(m_Molecules.size() == 1)
		return m_Molecules[0];
	srand(GetTickCount());
	m_nCurrentPosition = rand()%(m_Molecules.size());
	return m_Molecules[m_nCurrentPosition];
}

void
CMoleculeList::ProcessDir(LPCTSTR dir)
{
	CFindFile finder;
	CString pattern;
	pattern.Format(_T("%s\\*.*"), dir);
	if(finder.FindFile(pattern))
	{
		do
		{
			if(finder.IsDots())
				continue;
			if(finder.IsDirectory())
				ProcessDir((LPTSTR)(LPCTSTR)finder.GetFilePath());
			else
			{
				CString path = finder.GetFilePath();
				int len = path.GetLength();
				if(len > 4 && path.Right(3) == _T("pdb"))
				{
					LPTSTR file = new TCHAR[len+1];
					_tcsncpy_s(file, len+1, finder.GetFilePath(), len);
					this->m_Molecules.push_back(file);		
				}
			}
		} while(finder.FindNextFile());
	}
	finder.Close();
}