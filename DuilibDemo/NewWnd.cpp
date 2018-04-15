#include "stdafx.h"
#include "NewWnd.h"


CNewWnd::CNewWnd() {
}


CNewWnd::~CNewWnd() {
}

//重载消息接管函数
LRESULT CNewWnd::HandleMessage(UINT uMsg,
                               WPARAM wParam,
                               LPARAM lParam) {
    return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}
//重载消息响应函数
void CNewWnd::Notify(TNotifyUI& msg) {
    return WindowImplBase::Notify(msg);
}

void CNewWnd::InitWindow() {
    InitControl();
}

//初始化控件
bool CNewWnd::InitControl(void) {
    m_labCaption = static_cast<CLabelUI*>(m_pm.FindControl(_T("lab_caption")));
    assert(m_labCaption);
    return true;
}
