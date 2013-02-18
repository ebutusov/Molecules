#pragma once
#include "stdafx.h"
#include "resource.h"
#include "SPrefs.h"
#include "File.h"
#include "Archive.h"
#include "Exception.h"

class CSettingsDlg : public CDialogImpl<CSettingsDlg>,
	public CWinDataExchange<CSettingsDlg>
{
public:
	CSettingsDlg(void);
	enum { IDD = IDD_SETTINGS };
public:
	virtual ~CSettingsDlg(void);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAbout(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	int DoModal(HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = NULL);

	// to z DDX_CONTROL_HANDLE wcale nie bylo takie oczywiste. ;-)
	BEGIN_MSG_MAP(CSettingsDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDABOUT, OnAbout)
	END_MSG_MAP()
	BEGIN_DDX_MAP(CSettingsDlg)
		DDX_CONTROL_HANDLE(IDOK, m_btOK)
		DDX_CONTROL_HANDLE(IDC_DELAY, m_trackDelay)
		DDX_CONTROL_HANDLE(IDC_XACCEL, m_trackXAcc)
		DDX_CONTROL_HANDLE(IDC_YACCEL, m_trackYAcc)
		DDX_CONTROL_HANDLE(IDC_ZACCEL, m_trackZAcc)
		DDX_UINT_RANGE(IDC_EDITSHOW, m_showTime, 15, 600)
		DDX_UINT_RANGE(IDC_EDITEXITAFTER, m_exitAfter, 0, 60) // exit rendering mode control
		DDX_CHECK(IDC_CHKFPS, m_bShowFps)
		DDX_CHECK(IDC_CHKDESC, m_bShowDesc)
		DDX_CHECK(IDC_CHKWIRE, m_bWire)
		DDX_CHECK(IDC_CHKLINKS, m_bLinks)
		DDX_CHECK(IDC_CHKBUILD, m_bBuilding)
	END_DDX_MAP()

private:
	BOOL m_bModalDlg;
	SPreferences m_sPrefs;
	CButton m_btOK;
	CTrackBarCtrl m_trackDelay;
	CTrackBarCtrl m_trackXAcc;
	CTrackBarCtrl m_trackYAcc;
	CTrackBarCtrl m_trackZAcc;
	int m_showTime;
	int m_exitAfter;
	BOOL m_bShowDesc;
	BOOL m_bShowFps;
	BOOL m_bWire;
	BOOL m_bLinks;
	BOOL m_bBuilding;
	BOOL SaveSettings();
	BOOL LoadSettings();
	void UpdateControls();
};
