#pragma once

#include "stdafx.h"
#include "Molecule.h"
#include "GLDrawHelper.h"
#include "MoleculeBuilder.h"
#include "MoleculeList.h"
#include "atlwfile.h"
#include "SPrefs.h"
#include "Physics.h"
#include "Helpers.h"
#include <direct.h>

class CSaverWindow : public CWindowImpl<CSaverWindow>, 
	public COpenGL<CSaverWindow> //,public CIdleHandler
{
private:
	GLuint m_font_base,	m_textureFloor;
	GLfloat m_fZoom, m_fFloorPos;
	
	bool m_bTouchExit;	// exits when touched (key or mouse)
	int m_nFps, m_nFpsCount;
	DWORD m_dFrameTime;	// time it took to render single frame
	DWORD m_dLastFrameDrawn;
	DWORD m_dShowTimeStart; // time since start of render state
	LONG m_lTextHeight;
	bool m_bShowDesc, m_bScreenTooSmall, m_bShowFps, m_bError;
	LPARAM m_paramCoord;
	bool m_bHaveCoord;
	CString m_csErrorText;
	CString m_headerText;
	DWORD m_dLastMove;
	CMolecule *m_pMolecule;			// current displayed molecule
	CMoleculeList m_MManager;		// manages molecule building and loading
	typedef enum Oper { opRender, opRenderBegin, opZoomIn, opZoomOut, opBlank };
	Oper m_State;		// rendering state
	DWORD m_StopTime;
	SPreferences m_Settings;
	CStringBlender m_Blender;
	CTwister m_Twister;						// rotation logic
	
	bool LoadMolecule();
	void LoadPreferences();
  void DrawFloor();
	void UpdateFloor(DWORD delta);

public:
	DECLARE_WND_CLASS(_T("ScreenSaverClass"))
	CSaverWindow(void);

	void OnInit();
	void OnRender();
	void OnResize(int cx, int cy);
	//BOOL OnIdle();
	bool RunSaver();
	HWND Create(HWND hwndParent, bool bExitOnTouch, LPRECT lprc=NULL);
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
