#pragma once
#include "stdafx.h"
#include "resource.h"
#include "SPrefs.h"

class CSetPerformPg : public CPropertyPageImpl<CSetPerformPg>,
	public CWinDataExchange<CSetPerformPg>
{
public:
	CSetPerformPg(void);
	enum { IDD = IDD_SETPERFPG };
public:
	~CSetPerformPg(void);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	//int DoModal(HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = NULL);
	
	BOOL OnApply();

	// to z DDX_CONTROL_HANDLE wcale nie bylo takie oczywiste. ;-)
	BEGIN_MSG_MAP(CSetPerformPg)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CPropertyPageImpl<CSetPerformPg>) 
	END_MSG_MAP()
	BEGIN_DDX_MAP(CSetPerformPg)
		DDX_CONTROL_HANDLE(IDC_DELAY, m_trackDelay)
		DDX_CONTROL_HANDLE(IDC_MAXSPEED, m_maxSpeed)
		DDX_CONTROL_HANDLE(IDC_ACCELERATION, m_acceleration)
	END_DDX_MAP()

	SPreferences *m_pPrefs;
private:
	BOOL m_bChanged;
	CTrackBarCtrl m_trackDelay;
	CTrackBarCtrl m_maxSpeed;
	CTrackBarCtrl m_acceleration;
	void UpdateControls();
};
