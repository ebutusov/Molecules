#pragma once
#include "stdafx.h"
#include "resource.h"
#include "SPrefs.h"

class CSetOptionsPg : public CPropertyPageImpl<CSetOptionsPg>,
	public CWinDataExchange<CSetOptionsPg>
{
public:
	CSetOptionsPg(void);
	enum { IDD = IDD_SETOPTSPG };
	~CSetOptionsPg(void);

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTextChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	BOOL OnKillActive();
	BOOL OnApply();

	BEGIN_MSG_MAP(CSetOptionsPg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_HANDLER(IDC_CHKFPS, IDC_CHKBUILD, OnClick)
		COMMAND_RANGE_CODE_HANDLER(IDC_EDITSHOW, IDC_EDITEXITAFTER, EN_CHANGE, OnTextChanged)
		CHAIN_MSG_MAP(CPropertyPageImpl<CSetOptionsPg>)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CSetOptionsPg)
		DDX_UINT_RANGE(IDC_EDITSHOW, m_showTime, 15, 600)
		DDX_UINT_RANGE(IDC_EDITEXITAFTER, m_exitAfter, 0, 60) // exit rendering mode control
		DDX_CHECK(IDC_CHKFPS, m_bShowFps)
		DDX_CHECK(IDC_CHKDESC, m_bShowDesc)
		DDX_CHECK(IDC_CHKWIRE, m_bWire)
		DDX_CHECK(IDC_CHKLINKS, m_bLinks)
		DDX_CHECK(IDC_CHKBUILD, m_bBuilding)
		DDX_CHECK(IDC_CHK_TT_TEXT, m_bTeleType)
    DDX_CHECK(IDC_CHKREFLECTION, m_bReflection);
	END_DDX_MAP()

	SPreferences *m_pPrefs;
private:
	int m_showTime;
	int m_exitAfter;
	BOOL m_bShowDesc;
	BOOL m_bShowFps;
	BOOL m_bWire;
	BOOL m_bLinks;
	BOOL m_bBuilding;
	BOOL m_bTeleType;
  BOOL m_bReflection;

	BOOL m_bChanged;

	void DisplayError();
	void EventChkDescChanged();
};
