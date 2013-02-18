// Molecules.cpp : main source file for Molecules.exe
//

#include "stdafx.h"
#include "resource.h"
#include "aboutdlg.h"
#include "SaverWindow.h"
#include "SettingsDlg.h"
#include "SettingsSheet.h"
#include <direct.h>

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	// set normal priority
	DWORD pid = ::GetCurrentProcessId();
	HANDLE hProc = ::OpenProcess(PROCESS_SET_INFORMATION, TRUE, pid);
	::SetPriorityClass(hProc, NORMAL_PRIORITY_CLASS);
	::CloseHandle(hProc);
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CSaverWindow wndMain;
	if(wndMain.Create(NULL, true) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(SW_SHOW);
	Sleep(10);
	wndMain.RunSaver();

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int Preview(HWND parent)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
	CSaverWindow wndMain;
	if(wndMain.Create(parent, false) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}
	wndMain.ShowWindow(SW_SHOW);
	Sleep(10);
	wndMain.RunSaver();
	int nRet = theLoop.Run();
	_Module.RemoveMessageLoop();
	return nRet;
}

int Settings(HWND hwndParent)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	//CSettingsDlg dlg;
	CSettingsSheet dlg;
	int nRet = 0;

	if(hwndParent) 
	{
		dlg.DoModal(hwndParent);
	} 
	else
	{
		if(dlg.Create(NULL) == NULL)
		{
				ATLTRACE(_T("Settings dialog creation failed!\n"));
				return 0;
		}
		_Module.Lock();
		dlg.ShowWindow(SW_SHOW);
		Sleep(10);
		nRet = theLoop.Run();
		_Module.RemoveMessageLoop();
	}
  return nRet;
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	// to powinno zalatwic problem z uruchamianiem kilku instancji na raz
	// moj sterownik do nvidii czasem reaguje na to crashem.
	HANDLE mutex = ::CreateMutex(NULL, TRUE, "MoleculesInitMTX");
	if(mutex && ::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		DWORD state = WaitForSingleObject(mutex, 500);
		if(state == WAIT_TIMEOUT || state == WAIT_FAILED)
		{
			_Module.Term();
			::CoUninitialize();
			return 0;
		}
	}

	// ustalamy polozenie binarki, po czym ustawiamy jako biezacy katalog (i dysk)
	// ignoruje w ten sposob katalog roboczy narzucony przez srodowisko
	TCHAR *mod = new TCHAR[MAX_PATH];
	::GetModuleFileName(NULL, mod, MAX_PATH-1);
	*(_tcsrchr(mod, '\\')+1) = '\0';
	TCHAR drive;
	if(islower((int)mod[0]))
		drive = _toupper(mod[0]);
	else
		drive = mod[0];
	int drv = (int)drive-64;
	_chdrive(drv);
	_tchdir(mod);
  delete [] mod;

	int nRet = 0;
  TCHAR szTokens[] = _T("-/");
  bool bRun = false;
  bool bPreview = false;
  HWND hwndParent = NULL;

  LPCTSTR lpszToken = _Module.FindOneOf(::GetCommandLine(), szTokens);
  while(lpszToken != NULL)
  {
    if(_tcsnicmp(lpszToken, _T("c"), 1) == 0)
    {
			// control panel - settings
      bRun = bPreview = false;
      hwndParent = GetForegroundWindow();
    }
    else if(_tcsnicmp(lpszToken, _T("p "), 2) == 0)
    {
			// preview
      bPreview = true;
      bRun = false;
      int n = 0;
      _stscanf_s(lpszToken+1, _T("%i%n"), &hwndParent, &n);
      lpszToken += n+1;
    }
    else if(_tcsnicmp(lpszToken, _T("s"), 1) == 0)
    {
			// normal screen save
      bPreview = false;
      bRun = true;
    }
    lpszToken = _Module.FindOneOf(lpszToken, szTokens);
  }

	if(bRun)
		nRet = Run(lpstrCmdLine, nCmdShow);
	else if(bPreview)
		nRet = Preview(hwndParent);
	else
	{
#ifdef DEBUG
		nRet = Run(lpstrCmdLine, nCmdShow);
#else
		nRet = Settings(hwndParent);
#endif
	}
	//MessageBox(NULL, "SETTINGS NOT IMPLEMENTED YET ;-)", "Molecules", MB_OK | MB_ICONINFORMATION);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
