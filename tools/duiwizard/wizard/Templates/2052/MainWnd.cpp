#include "stdafx.h"
#include "MainWnd.h"
//重载消息接管函数
LRESULT CMainWnd::HandleMessage(UINT uMsg, 
									   WPARAM wParam,
									   LPARAM lParam)
{
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
//重载消息响应函数
void CMainWnd::Notify(TNotifyUI& msg)
{
	return WindowImplBase::Notify(msg);
}
void CMainWnd::InitWindow()
{
	InitControl();
	labCaption->SetText(APP_NAME);
}


//初始化控件
bool CMainWnd::InitControl(void)
{
	labCaption = (CLabelUI*)m_pm.FindControl(_T("caption"));//标题
	return false;
}
