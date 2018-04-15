#include "stdafx.h"
#include "MainWnd.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow) {
    if (FAILED(CoInitialize(NULL)))
        return 0;
    CPaintManagerUI::SetInstance(hInstance);
    CMainWnd* pMainWnd = new CMainWnd();
    DWORD dwStyle = UI_WNDSTYLE_FRAME;

    if( pMainWnd != NULL ) {
        pMainWnd->Create(NULL, APP_NAME, dwStyle, WS_EX_STATICEDGE | WS_EX_APPWINDOW);
        pMainWnd->CenterWindow();
        pMainWnd->ShowModal();
    }
    CoUninitialize();
    return 0;
}

