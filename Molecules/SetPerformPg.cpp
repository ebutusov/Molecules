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
		m_pPrefs->fMaxSpeed = (GLfloat)m_maxSpeed.GetPos()/100.0f;
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
	m_maxSpeed.SetRange(0, 1000, TRUE);
	m_maxSpeed.SetTicFreq(100);
	m_maxSpeed.SetPos((int)(m_pPrefs->fMaxSpeed*100));
	m_trackDelay.SetPos(m_pPrefs->dMoveDelay);
	//DoDataExchange(DDX_LOAD);
}

