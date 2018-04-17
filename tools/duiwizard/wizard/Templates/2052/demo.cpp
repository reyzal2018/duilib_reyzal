#include "stdafx.h"
#include "MainWnd.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	if (FAILED(CoInitialize(NULL)))
		return 0;
	CPaintManagerUI::SetInstance(hInstance);
	CMainWnd* pMainWnd = new CMainWnd();
	DWORD dwStyle = UI_WNDSTYLE_DIALOG|WS_MINIMIZEBOX;

	if( pMainWnd != NULL ) 
	{
		pMainWnd->Create(NULL, APP_NAME, dwStyle, 0x100);
		pMainWnd->CenterWindow();
		pMainWnd->ShowModal();
	}
	CoUninitialize();
	return 0;
}

