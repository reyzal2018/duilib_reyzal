#ifndef __UIGROUPBOX_H__
#define __UIGROUPBOX_H__

#pragma once

namespace DuiLib {

class DUILIB_API CGroupBoxUI : public CVerticalLayoutUI {
public:
    CGroupBoxUI();
    ~CGroupBoxUI();
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void SetTextColor(DWORD dwTextColor);
    DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
    DWORD GetDisabledTextColor() const;
    void SetFont(int index);
    int GetFont() const;


protected:
    //Paint
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
    SIZE CalcRectSize(SIZE szAvailable);
    void SetTextHeight(int height);
    int GetTextHeight();
    int m_iTextHeight;
protected:
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
    int m_iFont;
    UINT m_uTextStyle;
};
}
#endif // __UIGROUPBOX_H__