#include "stdafx.h"
#include "UIDateTime.h"

namespace DuiLib {
//CDateTimeUI::m_nDTUpdateFlag
#define DT_NONE   0
#define DT_UPDATE 1
#define DT_DELETE 2
#define DT_KEEP   3

class CDateTimeWnd : public CWindowWnd {
public:
    CDateTimeWnd();

    void Init(CDateTimeUI* pOwner);
    RECT CalPos();

    LPCTSTR GetWindowClassName() const;
    LPCTSTR GetSuperClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    CDateTimeUI* m_pOwner;
    HBRUSH m_hBkBrush;
    bool m_bInit;
    CDuiString m_CustomFormat;
};

CDateTimeWnd::CDateTimeWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false) {
}

void CDateTimeWnd::Init(CDateTimeUI* pOwner) {
    m_pOwner = pOwner;
    m_pOwner->m_nDTUpdateFlag = DT_NONE;

    if (m_hWnd == NULL) {
        RECT rcPos = CalPos();
        UINT uStyle = WS_CHILD;
        if (m_pOwner->m_bShowUpDown)
            uStyle |= DTS_UPDOWN;
        Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
        SetWindowFont(m_hWnd, m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->hFont, TRUE);
    }
    if (m_pOwner->m_CustomFormat != m_CustomFormat) {
        ::SendMessage(m_hWnd, DTM_SETFORMAT, 0, (LPARAM)m_pOwner->m_CustomFormat.GetData());
        m_CustomFormat = m_pOwner->m_CustomFormat;
    }
    if (m_pOwner->GetText().IsEmpty())
        ::GetLocalTime(&m_pOwner->m_sysTime);

    ::SendMessage(m_hWnd, DTM_SETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    ::SetFocus(m_hWnd);

    m_bInit = true;

}

RECT CDateTimeWnd::CalPos() {
    CDuiRect rcPos = m_pOwner->GetPos();

    CControlUI* pParent = m_pOwner;
    RECT rcParent;
    while( pParent = pParent->GetParent() ) {
        if( !pParent->IsVisible() ) {
            rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
            break;
        }
        rcParent = pParent->GetClientPos();
        if( !::IntersectRect(&rcPos, &rcPos, &rcParent) ) {
            rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
            break;
        }
    }

    return rcPos;
}

LPCTSTR CDateTimeWnd::GetWindowClassName() const {
    return _T("DateTimeWnd");
}

LPCTSTR CDateTimeWnd::GetSuperClassName() const {
    return DATETIMEPICK_CLASS;
}

void CDateTimeWnd::OnFinalMessage(HWND hWnd) {
    // Clear reference and die
    if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
    m_pOwner->GetManager()->RemoveNativeWindow(hWnd);
    m_pOwner->m_pWindow = NULL;
    delete this;
}

LRESULT CDateTimeWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;
    if( uMsg == WM_CREATE ) {
        m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
        bHandled = FALSE;
    } else if( uMsg == WM_KILLFOCUS ) {
        lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
    } else if (uMsg == WM_KEYUP && (wParam == VK_DELETE || wParam == VK_BACK)) {
        LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        m_pOwner->m_nDTUpdateFlag = DT_DELETE;
        m_pOwner->UpdateText();
        PostMessage(WM_CLOSE);
        return lRes;
    } else if (uMsg == WM_KEYUP && wParam == VK_ESCAPE) {
        LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        m_pOwner->m_nDTUpdateFlag = DT_KEEP;
        PostMessage(WM_CLOSE);
        return lRes;
    } else if( uMsg == WM_PAINT) {
        if (m_pOwner->GetManager()->IsLayered()) {
            m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
        }
        bHandled = FALSE;
    } else bHandled = FALSE;
    if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    return lRes;
}

LRESULT CDateTimeWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    if (m_pOwner->m_nDTUpdateFlag == DT_NONE) {
        ::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
        m_pOwner->m_nDTUpdateFlag = DT_UPDATE;
        m_pOwner->UpdateText();
    }
    if ((HWND)wParam != m_pOwner->GetManager()->GetPaintWindow()) {
        ::SendMessage(m_pOwner->GetManager()->GetPaintWindow(), WM_KILLFOCUS, wParam, lParam);
    }
    SendMessage(WM_CLOSE);
    return lRes;
}

//////////////////////////////////////////////////////////////////////////
//
CDateTimeUI::CDateTimeUI() {
    m_bShowUpDown = false;
    m_CustomFormat = _T("yyyy-MM-dd");
    m_bReadOnly = false;
    m_pWindow = NULL;
    //m_nDTUpdateFlag=DT_UPDATE;
    //UpdateText();		// add by:daviyang35 初始化界面时显示时间
    //m_nDTUpdateFlag = DT_NONE;
}

LPCTSTR CDateTimeUI::GetClass() const {
    return DUI_CTR_DATETIME;
}

LPVOID CDateTimeUI::GetInterface(LPCTSTR pstrName) {
    if( _tcscmp(pstrName, DUI_CTR_DATETIME) == 0 ) return static_cast<CDateTimeUI*>(this);
    return CLabelUI::GetInterface(pstrName);
}

UINT CDateTimeUI::GetControlFlags() const {
    return UIFLAG_TABSTOP;
}

HWND CDateTimeUI::GetNativeWindow() const {
    if (m_pWindow) return m_pWindow->GetHWND();
    return NULL;
}

SYSTEMTIME& CDateTimeUI::GetTime() {
    return m_sysTime;
}

void CDateTimeUI::SetTime(SYSTEMTIME* pst) {
    m_sysTime = *pst;
    Invalidate();
}

void CDateTimeUI::SetReadOnly(bool bReadOnly) {
    m_bReadOnly = bReadOnly;
    Invalidate();
}

bool CDateTimeUI::IsReadOnly() const {
    return m_bReadOnly;
}

void CDateTimeUI::UpdateText() {
    if (m_nDTUpdateFlag == DT_DELETE)
        SetText(_T(""));
    else if (m_nDTUpdateFlag == DT_UPDATE) {
        SetText(GetFormatTime());
    }
}

int CDateTimeUI::GetRepeatTimes(int nPos, TCHAR chPart) {
    int num = 1;
    for (int i = nPos + 1; i < m_CustomFormat.GetLength(); i++) {
        if (m_CustomFormat.GetAt(i) == chPart)
            num++;
        else
            break;
    }
    return num;
}

static TCHAR* const shortDayOfWeek[] = { _T("周日"), _T("周一"), _T("周二"), _T("周三"), _T("周四"), _T("周五"), _T("周六") };
static TCHAR* const longDayOfWeek[] = { _T("星期日"), _T("星期一"), _T("星期二"), _T("星期三"), _T("星期四"), _T("星期五"), _T("星期六") };
static TCHAR* const shortMonth[] = { _T("1月"),
                                     _T("2月"),
                                     _T("3月"),
                                     _T("4月"),
                                     _T("5月"),
                                     _T("6月"),
                                     _T("7月"),
                                     _T("8月"),
                                     _T("9月"),
                                     _T("10月"),
                                     _T("11月"),
                                     _T("12月")
                                   };
static TCHAR* const longMonth[] = { _T("一月"),
                                    _T("二月"),
                                    _T("三月"),
                                    _T("四月"),
                                    _T("五月"),
                                    _T("六月"),
                                    _T("七月"),
                                    _T("八月"),
                                    _T("九月"),
                                    _T("十月"),
                                    _T("十一月"),
                                    _T("十二月")
                                  };
static TCHAR* const longAPM[] = {
    _T("AM"),
    _T("PM")
};
static TCHAR* const shortAPM[] = {
    _T("A"),
    _T("P")
};
//自解析DateTime
CDuiString CDateTimeUI::GetFormatTime() {
    CDuiString strResult;
    CDuiString strFormat, strTemp;
    int num = 1;
    for (int i = 0; i < m_CustomFormat.GetLength(); i += num) {
        num = 1;
        TCHAR chPartt = m_CustomFormat.GetAt(i);
        switch (chPartt) {
        case 'd': {
            num = GetRepeatTimes(i, chPartt);
            if (num == 1) {
                strFormat.Format(_T("%d"), m_sysTime.wDay);
                strResult.Append(strFormat);
            } else if (num == 2) {
                strFormat.Format(_T("%02d"), m_sysTime.wDay);
                strResult.Append(strFormat);
            } else if (num == 3) {
                strResult.Append(shortDayOfWeek[m_sysTime.wDayOfWeek]);
            } else if (num == 4) {
                strResult.Append(longDayOfWeek[m_sysTime.wDayOfWeek]);
            }
            break;
        }
        case 'h': {
            num = GetRepeatTimes(i, chPartt);
            if (num == 1) {
                strFormat.Format(_T("%d"), m_sysTime.wHour % 12);
                strResult.Append(strFormat);
            } else if (num == 2) {
                strFormat.Format(_T("%02d"), m_sysTime.wHour % 12);
                strResult.Append(strFormat);
            }
            break;
        }
        case 'H': {
            num = GetRepeatTimes(i, chPartt);
            if (num == 1) {
                strFormat.Format(_T("%d"), m_sysTime.wHour);
                strResult.Append(strFormat);
            } else if (num == 2) {
                strFormat.Format(_T("%02d"), m_sysTime.wHour);
                strResult.Append(strFormat);
            }
            break;
        }
        case 'm': {
            num = GetRepeatTimes(i, chPartt);
            if (num == 1) {
                strFormat.Format(_T("%d"), m_sysTime.wMinute);
                strResult.Append(strFormat);
            } else if (num == 2) {
                strFormat.Format(_T("%02d"), m_sysTime.wMinute);
                strResult.Append(strFormat);
            }
            break;
        }
        case 's': {
            num = GetRepeatTimes(i, chPartt);
            if (num == 1) {
                strFormat.Format(_T("%d"), m_sysTime.wSecond);
                strResult.Append(strFormat);
            } else if (num == 2) {
                strFormat.Format(_T("%02d"), m_sysTime.wSecond);
                strResult.Append(strFormat);
            }
            break;
        }
        case 'M': {
            num = GetRepeatTimes(i, chPartt);
            if (num == 1) {
                strFormat.Format(_T("%d"), m_sysTime.wMonth);
                strResult.Append(strFormat);
            } else if (num == 2) {
                strFormat.Format(_T("%02d"), m_sysTime.wMonth);
                strResult.Append(strFormat);
            } else if (num == 3) {
                strResult.Append(shortMonth[m_sysTime.wMonth - 1]);
            } else if (num == 4) {
                strResult.Append(longMonth[m_sysTime.wMonth - 1]);
            }
            break;
        }
        case 'y': {
            num = GetRepeatTimes(i, chPartt);
            if (num == 2) {
                strFormat.Format(_T("%d"), m_sysTime.wYear % 100);
                strResult.Append(strFormat);
            } else if (num == 4) {
                strFormat.Format(_T("%d"), m_sysTime.wYear);
                strResult.Append(strFormat);
            }
            break;
        }
        case 't': {
            num = GetRepeatTimes(i, chPartt);
            if (num == 1) {
                strFormat.Format(_T("%s"), m_sysTime.wHour >= 12 ? shortAPM[1] : shortAPM[0]);
                strResult.Append(strFormat);
            } else if (num == 2) {
                strFormat.Format(_T("%s"), m_sysTime.wHour >= 12 ? longAPM[1] : longAPM[0]);
                strResult.Append(strFormat);
            }
            break;
        }
        default: {
            strFormat.Format(_T("%c"), m_CustomFormat.GetAt(i));
            strResult.Append(strFormat);
            break;
        }
        }
    }
    return strResult;
}

void CDateTimeUI::SetPos(RECT rc, bool bNeedInvalidate) {
    CControlUI::SetPos(rc, bNeedInvalidate);
    if( m_pWindow != NULL ) {
        RECT rcPos = m_pWindow->CalPos();
        if (::IsRectEmpty(&rcPos)) ::ShowWindow(m_pWindow->GetHWND(), SW_HIDE);
        else {
            ::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
                           rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    }
}

void CDateTimeUI::Move(SIZE szOffset, bool bNeedInvalidate) {
    CControlUI::Move(szOffset, bNeedInvalidate);
    if( m_pWindow != NULL ) {
        RECT rcPos = m_pWindow->CalPos();
        if (::IsRectEmpty(&rcPos)) ::ShowWindow(m_pWindow->GetHWND(), SW_HIDE);
        else {
            ::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
                           rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    }
}

void CDateTimeUI::DoEvent(TEventUI& event) {
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CLabelUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETCURSOR && IsEnabled() ) {
        ::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
        return;
    }
    if( event.Type == UIEVENT_WINDOWSIZE ) {
        if( m_pWindow != NULL ) m_pManager->SetFocusNeeded(this);
    }
    if( event.Type == UIEVENT_SCROLLWHEEL ) {
        if( m_pWindow != NULL ) return;
    }
    if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) {
        if( m_pWindow ) return;
        m_pWindow = new CDateTimeWnd();
        ASSERT(m_pWindow);
        m_pWindow->Init(this);
        m_pWindow->ShowWindow();
    }
    if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) {
        Invalidate();
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) {
        if( IsEnabled() ) {
            GetManager()->ReleaseCapture();
            if( IsFocused() && m_pWindow == NULL ) {
                m_pWindow = new CDateTimeWnd();
                ASSERT(m_pWindow);
            }
            if( m_pWindow != NULL ) {
                m_pWindow->Init(this);
                m_pWindow->ShowWindow();
            }
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE ) {
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP ) {
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU ) {
        return;
    }

    CLabelUI::DoEvent(event);
}
void CDateTimeUI::SetShowUpDown(bool bUpDown) {
    if (m_bShowUpDown != bUpDown) {
        m_bShowUpDown = bUpDown;
        delete m_pWindow;
        m_pWindow = NULL;
    }
}
bool CDateTimeUI::GetShowUpDown() {
    return m_bShowUpDown;
}

void CDateTimeUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    if (_tcscmp(pstrName, _T("showupdown")) == 0) {
        if (_tcscmp(pstrValue, _T("true")) == 0)
            m_bShowUpDown = true;
        else
            m_bShowUpDown = false;
    } else if (_tcscmp(pstrName, _T("customformat")) == 0) {
        m_CustomFormat = pstrValue;
    }
    return CLabelUI::SetAttribute(pstrName, pstrValue);
}
void CDateTimeUI::SetCustomFormat(LPCTSTR lpCustomFormt) {
    m_CustomFormat = lpCustomFormt;
}
CDuiString CDateTimeUI::GetCustomFormat() {
    return m_CustomFormat;
}

void CDateTimeUI::Init() {
    m_nDTUpdateFlag = DT_UPDATE;
    ::GetLocalTime(&m_sysTime);
    UpdateText();
    m_nDTUpdateFlag = DT_NONE;
}
}

