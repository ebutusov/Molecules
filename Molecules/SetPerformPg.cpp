#include "StdAfx.h"
#include "SettingsDlg.h"
#include "AboutDlg.h"
#include "SettingsSheet.h"
#include <xstring>
#include "SetPerformPg.h"

CSetPerformPg::CSetPerformPg(void)
{
	m_bChanged = FALSE;
}

CSetPerformPg::~CSetPerformPg(void)
{
}

BOOL
CSetPerformPg::OnApply()
{
	if(!m_pPrefs)
		return 0;

	if(m_bChanged)	// dodatkowe rozpoznanie zmian
	{
		m_pPrefs->dMoveDelay = m_trackDelay.GetPos();
		m_pPrefs->fXspeed = (GLfloat)m_trackXSpeed.GetPos()/100;
		m_pPrefs->fYspeed = (GLfloat)m_trackYSpeed.GetPos()/100;
		m_pPrefs->fZspeed = (GLfloat)m_trackZSpeed.GetPos()/100;
		m_bChanged = FALSE;
	}
	return 0;
}

LRESULT 
CSetPerformPg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	DoDataExchange(DDX_LOAD);
	UpdateControls();
	return TRUE;
}

LRESULT
CSetPerformPg::OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bChanged = TRUE;
	SetModified(1);
	return TRUE;
}

void
CSetPerformPg::UpdateControls()
{
	m_trackDelay.SetRange(5, 40, TRUE);
	m_trackXSpeed.SetRange(0, 500, TRUE);
	m_trackXSpeed.SetTicFreq(10);
	m_trackYSpeed.SetRange(0, 500, TRUE);
	m_trackYSpeed.SetTicFreq(10);
	m_trackZSpeed.SetRange(0, 500, TRUE);
	m_trackZSpeed.SetTicFreq(10);
	m_trackXSpeed.SetPos((int)(m_pPrefs->fXspeed*100));
	m_trackYSpeed.SetPos((int)(m_pPrefs->fYspeed*100));
	m_trackZSpeed.SetPos((int)(m_pPrefs->fZspeed*100));
	m_trackDelay.SetPos(m_pPrefs->dMoveDelay);
	DoDataExchange(DDX_LOAD);
}

