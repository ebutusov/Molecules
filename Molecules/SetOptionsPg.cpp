#include "StdAfx.h"
#include "SetOptionsPg.h"

CSetOptionsPg::CSetOptionsPg(void)
{
	m_bChanged = FALSE;
}

CSetOptionsPg::~CSetOptionsPg(void)
{
}

void 
CSetOptionsPg::EventChkDescChanged()
{
	CButton chk;
	chk.Attach(GetDlgItem(IDC_CHKDESC));
	BOOL enabled = chk.GetCheck() > 0;
	chk.Attach(GetDlgItem(IDC_CHKFPS));
	chk.EnableWindow(enabled);
	chk.Attach(GetDlgItem(IDC_CHK_TT_TEXT));
	chk.EnableWindow(enabled);
  chk.Detach();
}


LRESULT
CSetOptionsPg::OnClick(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_bChanged = TRUE;
	if(wID == IDC_CHKDESC)
		EventChkDescChanged();
	SetModified(1);
	return 0;
}

LRESULT 
CSetOptionsPg::OnTextChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_bChanged = TRUE;
	SetModified(1);
	return 0;
}

BOOL
CSetOptionsPg::OnApply()
{
	if(!m_pPrefs)
		return 1;
	
	if(m_bChanged)
	{
		if(!DoDataExchange(DDX_SAVE))
		{
			DisplayError();
			return 1;
		}
		m_pPrefs->dShowTime = m_showTime*1000;
		m_pPrefs->dRunTime = m_exitAfter;
		m_pPrefs->bShowDesc = m_bShowDesc;
		m_pPrefs->bShowFPS = m_bShowFps;
		m_pPrefs->bShowLinks = m_bLinks;
		m_pPrefs->bWire = m_bWire;
		m_pPrefs->bAnimateBuild = m_bBuilding;
		m_pPrefs->bTeleType = m_bTeleType;
    m_pPrefs->bReflection = m_bReflection;
		m_bChanged = FALSE;
	}
	return 0;
}

BOOL
CSetOptionsPg::OnKillActive()
{
	if(!DoDataExchange(DDX_SAVE))
	{
		DisplayError();
		return TRUE;
	}
	else
		return FALSE;
}

void
CSetOptionsPg::DisplayError()
{
	::Beep(440, 100); ::Beep(600, 50);
	MessageBox("You've entered incorrect value!", "Error", MB_OK|MB_ICONERROR);
}


LRESULT
CSetOptionsPg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_showTime = m_pPrefs->dShowTime/1000;
	m_exitAfter = m_pPrefs->dRunTime;
	m_bShowFps = m_pPrefs->bShowFPS;
	m_bShowDesc = m_pPrefs->bShowDesc;
	m_bLinks = m_pPrefs->bShowLinks;
	m_bWire = m_pPrefs->bWire;
	m_bBuilding = m_pPrefs->bAnimateBuild;
	m_bTeleType = m_pPrefs->bTeleType;
  m_bReflection = m_pPrefs->bReflection;
	DoDataExchange(DDX_LOAD);
	EventChkDescChanged();
	return 0;
}
