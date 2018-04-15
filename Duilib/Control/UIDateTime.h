#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace DuiLib {
class CDateTimeWnd;

/// 时间选择控件
class DUILIB_API CDateTimeUI : public CLabelUI {
    friend class CDateTimeWnd;
public:
    CDateTimeUI();
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;
    HWND GetNativeWindow() const;

    SYSTEMTIME& GetTime();
    void SetTime(SYSTEMTIME* pst);

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;

    void UpdateText();

    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void Move(SIZE szOffset, bool bNeedInvalidate = true);

    void DoEvent(TEventUI& event);

    void SetCustomFormat(LPCTSTR lpCustomFormt);
    CDuiString GetCustomFormat();
    void SetShowUpDown(bool bUpDown);
    bool GetShowUpDown();
    int GetRepeatTimes(int nPos, TCHAR chPart);
    CDuiString GetFormatTime();

    virtual void Init() override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
protected:
    SYSTEMTIME m_sysTime;
    int        m_nDTUpdateFlag;
    bool       m_bReadOnly;
    bool	   m_bShowUpDown;
    CDuiString	m_CustomFormat;
    CDateTimeWnd* m_pWindow;
};
}
#endif // __UIDATETIME_H__