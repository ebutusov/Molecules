#include "StdAfx.h"
#include "SettingsSheet.h"

CSettingsSheet::CSettingsSheet(ATL::_U_STRINGorID title, UINT uStartPage, HWND hWndParent) :
	CPropertySheetImpl<CSettingsSheet>(title, uStartPage, hWndParent)
{
	this->SetTitle("Molecule settings");
	m_bModal = FALSE;
	LoadSettings();
	m_pgPerformance.m_pPrefs = &m_sPrefs;
	AddPage(m_pgPerformance);
	m_pgOptions.m_pPrefs = &m_sPrefs;
	AddPage(m_pgOptions);
	AddPage(m_pgAbout);
}

CSettingsSheet::~CSettingsSheet(void)
{
}


int
CSettingsSheet::DoModal(HWND parent)
{
	m_bModal = TRUE;
	return CPropertySheetImpl<CSettingsSheet>::DoModal(parent);
}

//LRESULT 
//CSettingsSheet::OnSaveData(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//{
//	MessageBox("Saving data!", "Apply");
//	SaveSettings();
//	return 0;
//}


LRESULT
CSettingsSheet::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(!Apply())
		return 0;
	SaveSettings();
	if(m_bModal)
	{
		EndDialog(this->m_hWnd, 0);
		::PostQuitMessage(wID);
	}
	else
	{
		DestroyWindow();
		::PostQuitMessage(wID);
	}
	return 0;
}

LRESULT
CSettingsSheet::OnApply(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Apply();
	SaveSettings();
	return 0;
}

LRESULT
CSettingsSheet::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_bModal)
	{
		// tego nie rozumiem - ale sam EndDialog nie chce dzialac
		// to samo jest w OnOk - to PostQuitMessage mnie denerwuje
		::EndDialog(this->m_hWnd, wID);
		::PostQuitMessage(wID);
	}
	else
	{
		DestroyWindow();
		::PostQuitMessage(wID);
	}
	return 1;
}

BOOL
CSettingsSheet::SaveSettings()
{
	/*BOOL ok = TRUE;
	try
	{
		CXFile file;
		file.Open(SETTINGS_FILE,                  
    GENERIC_WRITE | GENERIC_READ,       
    FILE_SHARE_READ | FILE_SHARE_WRITE,    
    NULL,                  
    CREATE_ALWAYS,              
    FILE_ATTRIBUTE_NORMAL,          
    NULL);
		CXArchive ar(&file, CXArchive::store);
		m_sPrefs.Serialize(ar);
		ar.Close();
	}
	catch(CXException &e)
	{
#ifdef _DEBUG
		MessageBox(e.GetErrorDesc().c_str(), "Error", MB_OK|MB_ICONERROR);
#endif
		ok = FALSE;
	}*/
	return m_sPrefs.Save();
}

BOOL
CSettingsSheet::LoadSettings()
{
	BOOL ok = TRUE;
//	try
//	{
//		CXFile file;
//		file.Open(SETTINGS_FILE,                  
//    GENERIC_WRITE | GENERIC_READ,       
//    FILE_SHARE_READ | FILE_SHARE_WRITE,    
//    NULL,                  
//    OPEN_EXISTING,              
//    FILE_ATTRIBUTE_NORMAL,          
//    NULL);
//		CXArchive ar(&file, CXArchive::load);
//		m_sPrefs.Serialize(ar);
//		ar.Close();
//	}
//	catch(CXException &e)
//	{
//#ifdef _DEBUG
//		MessageBox(e.GetErrorDesc().c_str(), "Error", MB_OK|MB_ICONERROR);
//#endif
//		ok = FALSE;
//  }
  m_sPrefs.Load();
	return ok;
}