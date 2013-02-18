#pragma once

#include "stdafx.h"
#include "Molecule.h"
#include "GLDrawHelper.h"
#include "MoleculeBuilder.h"
#include "MoleculeList.h"
#include "atlwfile.h"
#include "SPrefs.h"
#include "File.h"
#include "Archive.h"
#include "Physics.h"
#include "Helpers.h"
#include <direct.h>

class CSaverWindow : public CWindowImpl<CSaverWindow>, 
	public COpenGL<CSaverWindow> //,public CIdleHandler
{
private:
	GLuint font_base;
	GLuint m_base_list;
	CMolecule *m_pMolecule;
	//GLfloat m_fXAngle, m_fYAngle, m_fZAngle;
	GLfloat m_fZoom;
	//int m_nMutateFactor;
	BOOL m_bTouchExit;						// exits when touched (key or mouse)
	BOOL m_nFps, m_nFpsCount;
	DWORD m_dFrameTime;
	LONG m_lTextHeight;
	BOOL m_bShowDesc;
	BOOL m_bScreenTooSmall;
	BOOL m_bShowFps;
	BOOL m_bError;
	LPARAM m_paramCoord;
	BOOL m_bHaveCoord;
	CString m_csErrorText;
	CMoleculeList m_MManager;
	CTwister m_Twister;
	BOOL LoadMolecule();
	BOOL LoadPreferences();
	void DoMotion();
  void DrawFloor();

	typedef enum Oper { opRender, opZoomIn, opZoomOut, opBlank, opBuilding };
	Oper m_State;
	DWORD m_StopTime;
	SPreferences m_Settings;
	CStringBlender m_Blender;
  GLuint m_textureFloor;
public:
	DECLARE_WND_CLASS("ScreenSaverClass")
	CSaverWindow(void);
	~CSaverWindow(void);

	void OnInit();
	void OnRender();
	void OnResize(int cx, int cy);
	//BOOL OnIdle();
	BOOL RunSaver();
	HWND Create(HWND hwndParent, BOOL bExitOnTouch, LPRECT lprc=NULL);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	BEGIN_MSG_MAP(CSaverWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		CHAIN_MSG_MAP(COpenGL<CSaverWindow>)
	END_MSG_MAP()
};
