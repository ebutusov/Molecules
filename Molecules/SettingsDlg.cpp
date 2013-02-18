#include "StdAfx.h"
#include "SettingsDlg.h"
#include "AboutDlg.h"
#include "SettingsSheet.h"
#include <xstring>

CSettingsDlg::CSettingsDlg(void)
{
	m_bModalDlg = false;
}

CSettingsDlg::~CSettingsDlg(void)
{
}

int CSettingsDlg::DoModal(HWND hWndParent , LPARAM dwInitParam)
{
  m_bModalDlg = true;
  return CDialogImpl<CSettingsDlg>::DoModal(hWndParent, dwInitParam);
}


LRESULT 
CSettingsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CenterWindow();
	LoadSettings();
	m_showTime = m_sPrefs.dShowTime/1000;
	m_exitAfter = m_sPrefs.dRunTime;
	m_bShowFps = m_sPrefs.ShowFPS;
	m_bShowDesc = m_sPrefs.ShowDesc;
	m_bLinks = m_sPrefs.ShowLinks;
	m_bWire = m_sPrefs.bWire;
	m_bBuilding = m_sPrefs.bAnimateBuild;
	DoDataExchange(DDX_LOAD);
	UpdateControls();
	return TRUE;
}

void
CSettingsDlg::UpdateControls()
{
	m_trackDelay.SetRange(5, 40, TRUE);
	m_trackXAcc.SetRange(0, 500, TRUE);
	m_trackXAcc.SetTicFreq(10);
	m_trackYAcc.SetRange(0, 500, TRUE);
	m_trackYAcc.SetTicFreq(10);
	m_trackZAcc.SetRange(0, 500, TRUE);
	m_trackZAcc.SetTicFreq(10);
	m_trackXAcc.SetPos((int)(m_sPrefs.fXspeed*100));
	m_trackYAcc.SetPos((int)(m_sPrefs.fYspeed*100));
	m_trackZAcc.SetPos((int)(m_sPrefs.fZspeed*100));
	m_trackDelay.SetPos(m_sPrefs.dMoveDelay);
	DoDataExchange(DDX_LOAD);
}

LRESULT 
CSettingsDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(!DoDataExchange(DDX_SAVE))
		return FALSE;
	m_sPrefs.dMoveDelay = m_trackDelay.GetPos();
	m_sPrefs.fXspeed = (GLfloat)m_trackXAcc.GetPos()/100;
	m_sPrefs.fYspeed = (GLfloat)m_trackYAcc.GetPos()/100;
	m_sPrefs.fZspeed = (GLfloat)m_trackZAcc.GetPos()/100;
	m_sPrefs.dShowTime = m_showTime*1000;
	m_sPrefs.dRunTime = m_exitAfter;
	m_sPrefs.ShowDesc = m_bShowDesc;
	m_sPrefs.ShowFPS = m_bShowFps;
	m_sPrefs.ShowLinks = m_bLinks;
	m_sPrefs.bWire = m_bWire;
	m_sPrefs.bAnimateBuild = m_bBuilding;
	SaveSettings();
	if(m_bModalDlg)
		EndDialog(wID);
	else
	{
		DestroyWindow();
		::PostQuitMessage(wID);
	}
	return 0;
}

LRESULT 
CSettingsDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_bModalDlg)
		EndDialog(wID);
	else
	{
		DestroyWindow();
		::PostQuitMessage(wID);
	}
	return 0;
}

BOOL
CSettingsDlg::SaveSettings()
{
	BOOL ok = TRUE;
	/*try
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
	return ok;
}

BOOL
CSettingsDlg::LoadSettings()
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
//	}
	return ok;
}

LRESULT 
CSettingsDlg::OnAbout(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal(this->m_hWnd);
	//CSettingsSheet sheet(_T("Options", 0));
	//sheet.DoModal();
	//sheet.Create(this->m_hWnd);
	return 0;
}

