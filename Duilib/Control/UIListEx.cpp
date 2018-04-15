#include "stdafx.h"
#include "UIListEx.h"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//
CListExUI::CListExUI() :
    m_pEditUI(NULL), m_pComboUI(NULL), m_bAddMessageFilter(FALSE),
    m_nRow(-1), m_nColum(-1), m_pXCallback(NULL) {
}

LPCTSTR CListExUI::GetClass() const {
    return _T("ListExUI");
}

UINT CListExUI::GetControlFlags() const {
    return UIFLAG_TABSTOP;
}

LPVOID CListExUI::GetInterface(LPCTSTR pstrName) {
    if( _tcsicmp(pstrName, _T("ListEx")) == 0 ) return static_cast<IListOwnerUI*>(this);
    return CListUI::GetInterface(pstrName);
}
BOOL CListExUI::CheckColumEditable(int nColum) {
    CListContainerHeaderItemUI* pHItem = static_cast<CListContainerHeaderItemUI*>(m_pHeader->GetItemAt(nColum));
    return pHItem != NULL ? pHItem->GetColumeEditable() : FALSE;
}
void CListExUI::InitListCtrl() {
    if (!m_bAddMessageFilter) {
        GetManager()->AddNotifier(this);
        m_bAddMessageFilter = TRUE;
    }
}
CEditUI* CListExUI::GetEditUI() {
    if (m_pEditUI == NULL) {
        m_pEditUI = new CEditUI;
        m_pEditUI->SetName(_T("ListEx_Edit"));
        LPCTSTR pDefaultAttributes = GetManager()->GetDefaultAttributeList(_T("Edit"));
        if( pDefaultAttributes ) {
            m_pEditUI->SetAttributeList(pDefaultAttributes);
        }

        Add(m_pEditUI);
    }
    if (m_pComboUI) {
        RECT rc = {0, 0, 0, 0};
        m_pComboUI->SetPos(rc);
    }

    return m_pEditUI;
}

BOOL CListExUI::CheckColumComboBoxable(int nColum) {
    CListContainerHeaderItemUI* pHItem = static_cast<CListContainerHeaderItemUI*>(m_pHeader->GetItemAt(nColum));
    return pHItem != NULL ? pHItem->GetColumeComboable() : FALSE;
}

CComboUI* CListExUI::GetComboUI() {
    if (m_pComboUI == NULL) {
        m_pComboUI = new CComboUI;
        m_pComboUI->SetName(_T("ListEx_Combo"));
        LPCTSTR pDefaultAttributes = GetManager()->GetDefaultAttributeList(_T("Combo"));
        if( pDefaultAttributes ) {
            m_pComboUI->SetAttributeList(pDefaultAttributes);
        }

        Add(m_pComboUI);
    }
    if (m_pEditUI) {
        RECT rc = {0, 0, 0, 0};
        m_pEditUI->SetPos(rc);
    }

    return m_pComboUI;
}

BOOL CListExUI::CheckColumCheckBoxable(int nColum) {
    CControlUI* p = m_pHeader->GetItemAt(nColum);
    CListContainerHeaderItemUI* pHItem = static_cast<CListContainerHeaderItemUI*>(p->GetInterface(_T("ListContainerHeaderItem")));
    return pHItem != NULL ? pHItem->GetColumeCheckable() : FALSE;
}

void CListExUI::Notify(TNotifyUI& msg) {
    CDuiString strName = msg.pSender->GetName();

    //复选框
    if(_tcsicmp(msg.sType, DUI_MSGTYPE_LISTHEADITEMCHECKED) == 0) {
        BOOL bCheck = (BOOL)msg.lParam;
        int  nIndex = msg.wParam;
        //判断是否是本LIST发送的notify
        CListHeaderUI* pHeader = GetHeader();
        for (int i = 0; i < pHeader->GetCount(); i++) {
            if (pHeader->GetItemAt(i) == msg.pSender) {
                for (int i = 0; i < GetCount(); ++i) {
                    CControlUI* p = GetItemAt(i);
                    CListExTextElementUI* pLItem = static_cast<CListExTextElementUI*>(p->GetInterface(_T("ListTextExElement")));
                    if (pLItem != NULL) {
                        pLItem->SetCheck(bCheck);
                    }
                }
                break;
            }
        }
    } else if (_tcsicmp(msg.sType, DUI_MSGTYPE_LISTITEMCHECKED) == 0) {
        for (int i = 0; i < GetCount(); ++i) {
            CControlUI* p = GetItemAt(i);
            CListExTextElementUI* pLItem = static_cast<CListExTextElementUI*>(p->GetInterface(_T("ListTextExElement")));
            if (pLItem != NULL && pLItem == msg.pSender) {
                OnListItemChecked(LOWORD(msg.wParam), HIWORD(msg.wParam), msg.lParam);
                break;
            }
        }
    }

    //编辑框、组合框
    if (_tcsicmp(strName, _T("ListEx_Edit")) == 0 && m_pEditUI && m_nRow >= 0 && m_nColum >= 0) {
        if(_tcsicmp(msg.sType, DUI_MSGTYPE_SETFOCUS) == 0) {

        } else if(_tcsicmp(msg.sType, DUI_MSGTYPE_KILLFOCUS) == 0) {
            CDuiString sText = m_pEditUI->GetText();
            CListExTextElementUI* pRowCtrl = (CListExTextElementUI*)GetItemAt(m_nRow);
            if (pRowCtrl) {
                pRowCtrl->SetText(m_nColum, sText);
            }

            //重置当前行列
            SetEditRowAndColum(-1, -1);

            //隐藏编辑框
            RECT rc = {0, 0, 0, 0};
            m_pEditUI->SetPos(rc);

        }
    } else if (_tcsicmp(strName, _T("ListEx_Combo")) == 0 && m_pComboUI && m_nRow >= 0 && m_nColum >= 0) {
        int  iCurSel, iOldSel;
        iCurSel = msg.wParam;
        iOldSel = msg.lParam;

        if(_tcsicmp(msg.sType, DUI_MSGTYPE_SETFOCUS) == 0) {

        } else if(_tcsicmp(msg.sType, DUI_MSGTYPE_KILLFOCUS) == 0) {
        } else if(_tcsicmp(msg.sType, DUI_MSGTYPE_LISTITEMSELECT) == 0 && iOldSel >= 0) {
            CListExTextElementUI* pRowCtrl = (CListExTextElementUI*)GetItemAt(m_nRow);
            if (pRowCtrl) {
                pRowCtrl->SetText(m_nColum, m_pComboUI->GetText());
            }

            //隐藏组合框
            RECT rc = {0, 0, 0, 0};
            m_pComboUI->SetPos(rc);
        }
    } else if(_tcsicmp(msg.sType, DUI_MSGTYPE_SCROLL) == 0 && (m_pComboUI || m_pEditUI) && m_nRow >= 0 && m_nColum >= 0) {
        HideEditAndComboCtrl();
    }
}
void CListExUI::HideEditAndComboCtrl() {
    //隐藏编辑框
    RECT rc = {0, 0, 0, 0};
    if(m_pEditUI) {
        m_pEditUI->SetPos(rc);
    }

    if(m_pComboUI) {
        m_pComboUI->SetPos(rc);
    }
}
IListComboCallbackUI* CListExUI::GetTextArrayCallback() const {
    return m_pXCallback;
}

void CListExUI::SetTextArrayCallback(IListComboCallbackUI* pCallback) {
    m_pXCallback = pCallback;
}
void CListExUI::OnListItemClicked(int nIndex, int nColum, RECT* lpRCColum, LPCTSTR lpstrText) {
    RECT rc = {0, 0, 0, 0};
    if (nColum < 0) {
        if (m_pEditUI) {
            m_pEditUI->SetPos(rc);
        }
        if (m_pComboUI) {
            m_pComboUI->SetPos(rc);
        }
    } else {
        if (CheckColumEditable(nColum) && GetEditUI()) {
            //保存当前行列
            SetEditRowAndColum(nIndex, nColum);

            //设置文字
            m_pEditUI->SetText(lpstrText);

            //移动位置
            m_pEditUI->SetVisible(TRUE);
            m_pEditUI->SetPos(*lpRCColum);
        } else if(CheckColumComboBoxable(nColum) && GetComboUI()) {
            //重置组合框
            m_pComboUI->RemoveAll();

            //保存当前行列
            SetEditRowAndColum(nIndex, nColum);

            //设置文字
            m_pComboUI->SetText(lpstrText);

            //获取
            if (m_pXCallback) {
                m_pXCallback->GetItemComboTextArray(m_pComboUI, nIndex, nColum);
            }

            //移动位置
            m_pComboUI->SetPos(*lpRCColum);
            m_pComboUI->SetVisible(TRUE);
        } else {
            if (m_pEditUI) {
                m_pEditUI->SetPos(rc);
            }
            if (m_pComboUI) {
                m_pComboUI->SetPos(rc);
            }
        }
    }
}
void CListExUI::OnListItemChecked(int nIndex, int nColum, BOOL bChecked) {
    CControlUI* p = m_pHeader->GetItemAt(nColum);
    CListContainerHeaderItemUI* pHItem = static_cast<CListContainerHeaderItemUI*>(p->GetInterface(_T("ListContainerHeaderItem")));
    if (pHItem == NULL) {
        return;
    }

    //如果选中，那么检查是否全部都处于选中状态
    if (bChecked) {
        BOOL bCheckAll = TRUE;
        for(int i = 0; i < GetCount(); i++) {
            CControlUI* p = GetItemAt(i);
            CListExTextElementUI* pLItem = static_cast<CListExTextElementUI*>(p->GetInterface(_T("ListTextExElement")));
            if( pLItem != NULL && !pLItem->GetCheck()) {
                bCheckAll = FALSE;
                break;
            }
        }
        if (bCheckAll) {
            pHItem->SetCheck(TRUE);
        } else {
            pHItem->SetCheck(FALSE);
        }
    } else {
        pHItem->SetCheck(FALSE);
    }
}
void CListExUI::DoEvent(TEventUI& event) {
    if (event.Type == UIEVENT_BUTTONDOWN) {
        HideEditAndComboCtrl();
    }

    CListUI::DoEvent(event);
}
void CListExUI::SetColumItemColor(int nIndex, int nColum, DWORD iBKColor) {
    CControlUI* p = GetItemAt(nIndex);
    CListExTextElementUI* pLItem = static_cast<CListExTextElementUI*>(p->GetInterface(_T("ListTextExElement")));
    if( pLItem != NULL) {
        DWORD iTextBkColor = iBKColor;
        pLItem->SetColumItemColor(nColum, iTextBkColor);
    }
}

BOOL CListExUI::GetColumItemColor(int nIndex, int nColum, DWORD& iBKColor) {
    CControlUI* p = GetItemAt(nIndex);
    CListExTextElementUI* pLItem = static_cast<CListExTextElementUI*>(p->GetInterface(_T("ListTextExElement")));
    if( pLItem == NULL) {
        return FALSE;
    }
    pLItem->GetColumItemColor(nColum, iBKColor);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
CListContainerHeaderItemUI::CListContainerHeaderItemUI() :
    CListHeaderItemUI(), m_bEditable(FALSE), m_bComboable(FALSE),
    m_bCheckBoxable(FALSE), m_uCheckBoxState(0), m_bChecked(FALSE), m_pOwner(NULL) {
    SetTextPadding(CDuiRect(2, 0, 2, 0));
    ptLastMouse.x = ptLastMouse.y = 0;
    SetMinWidth(16);
}

LPCTSTR CListContainerHeaderItemUI::GetClass() const {
    return _T("ListContainerHeaderItem");
}

LPVOID CListContainerHeaderItemUI::GetInterface(LPCTSTR pstrName) {
    if( _tcsicmp(pstrName, _T("ListContainerHeaderItem")) == 0 ) return this;
    return CContainerUI::GetInterface(pstrName);
}


void CListContainerHeaderItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    if( _tcsicmp(pstrName, _T("editable")) == 0 ) SetColumeEditable(_tcsicmp(pstrValue, _T("true")) == 0);
    else if( _tcsicmp(pstrName, _T("comboable")) == 0 ) SetColumeComboable(_tcsicmp(pstrValue, _T("true")) == 0);
    else if( _tcsicmp(pstrName, _T("checkable")) == 0 ) SetColumeCheckable(_tcsicmp(pstrValue, _T("true")) == 0);
    else if( _tcsicmp(pstrName, _T("checkboxwidth")) == 0 ) SetCheckBoxWidth(_ttoi(pstrValue));
    else if( _tcsicmp(pstrName, _T("checkboxheight")) == 0 ) SetCheckBoxHeight(_ttoi(pstrValue));
    else if( _tcsicmp(pstrName, _T("checkboxnormalimage")) == 0 ) SetCheckBoxNormalImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxhotimage")) == 0 ) SetCheckBoxHotImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxpushedimage")) == 0 ) SetCheckBoxPushedImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxfocusedimage")) == 0 ) SetCheckBoxFocusedImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxdisabledimage")) == 0 ) SetCheckBoxDisabledImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxselectedimage")) == 0 ) SetCheckBoxSelectedImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxforeimage")) == 0 ) SetCheckBoxForeImage(pstrValue);

    else CListHeaderItemUI::SetAttribute(pstrName, pstrValue);
}

void CListContainerHeaderItemUI::DoEvent(TEventUI& event) {
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CContainerUI::DoEvent(event);
        return;
    }
    //CheckBoxAble
    if (m_bCheckBoxable) {
        RECT rcCheckBox;
        GetCheckBoxRect(rcCheckBox);

        if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK ) {
            if( ::PtInRect(&rcCheckBox, event.ptMouse)) {
                m_uCheckBoxState |= UISTATE_PUSHED | UISTATE_CAPTURED;
                Invalidate();
            }
        } else if( event.Type == UIEVENT_MOUSEMOVE ) {
            if( (m_uCheckBoxState & UISTATE_CAPTURED) != 0 ) {
                if( ::PtInRect(&rcCheckBox, event.ptMouse) )
                    m_uCheckBoxState |= UISTATE_PUSHED;
                else
                    m_uCheckBoxState &= ~UISTATE_PUSHED;
                Invalidate();
            } else if (::PtInRect(&rcCheckBox, event.ptMouse)) {
                m_uCheckBoxState |= UISTATE_HOT;
                Invalidate();
            } else {
                m_uCheckBoxState &= ~UISTATE_HOT;
                Invalidate();
            }
        } else if( event.Type == UIEVENT_BUTTONUP ) {
            if( (m_uCheckBoxState & UISTATE_CAPTURED) != 0 ) {
                if( ::PtInRect(&rcCheckBox, event.ptMouse) ) {
                    SetCheck(!GetCheck());
                    CContainerUI* pOwner = (CContainerUI*)m_pParent;
                    if (pOwner) {
                        m_pManager->SendNotify(this, DUI_MSGTYPE_LISTHEADITEMCHECKED, pOwner->GetItemIndex(this), m_bChecked);
                    }

                }
                m_uCheckBoxState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
                Invalidate();
            } else if (::PtInRect(&rcCheckBox, event.ptMouse)) {

            }
        } else if( event.Type == UIEVENT_MOUSEENTER ) {
            if( ::PtInRect(&rcCheckBox, event.ptMouse) ) {
                m_uCheckBoxState |= UISTATE_HOT;
                Invalidate();
            }
        } else if( event.Type == UIEVENT_MOUSELEAVE ) {
            m_uCheckBoxState &= ~UISTATE_HOT;
            Invalidate();
        }
    }

    CListHeaderItemUI::DoEvent(event);
}

void CListContainerHeaderItemUI::PaintStatusImage(HDC hDC) {
    CListHeaderItemUI::PaintStatusImage(hDC);

    if(m_bCheckBoxable) {
        m_uCheckBoxState &= ~UISTATE_PUSHED;

        if( (m_uCheckBoxState & UISTATE_SELECTED) != 0 ) {
            if( !m_sCheckBoxSelectedImage.IsEmpty() ) {
                if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxSelectedImage) ) {}
                else goto Label_ForeImage;
            }
        }

        if( IsFocused() ) m_uCheckBoxState |= UISTATE_FOCUSED;
        else m_uCheckBoxState &= ~ UISTATE_FOCUSED;
        if( !IsEnabled() ) m_uCheckBoxState |= UISTATE_DISABLED;
        else m_uCheckBoxState &= ~ UISTATE_DISABLED;

        if( (m_uCheckBoxState & UISTATE_DISABLED) != 0 ) {
            if( !m_sCheckBoxDisabledImage.IsEmpty() ) {
                if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxDisabledImage) ) {}
                else return;
            }
        } else if( (m_uCheckBoxState & UISTATE_PUSHED) != 0 ) {
            if( !m_sCheckBoxPushedImage.IsEmpty() ) {
                if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxPushedImage) ) {}
                else return;
            }
        } else if( (m_uCheckBoxState & UISTATE_HOT) != 0 ) {
            if( !m_sCheckBoxHotImage.IsEmpty() ) {
                if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxHotImage) ) {}
                else return;
            }
        } else if( (m_uCheckBoxState & UISTATE_FOCUSED) != 0 ) {
            if( !m_sCheckBoxFocusedImage.IsEmpty() ) {
                if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxFocusedImage) ) {}
                else return;
            }
        }

        if( !m_sCheckBoxNormalImage.IsEmpty() ) {
            if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxNormalImage) ) {}
            else return;
        }

Label_ForeImage:
        if( !m_sCheckBoxForeImage.IsEmpty() ) {
            if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxForeImage) ) {}
        }
    }
}

void CListContainerHeaderItemUI::PaintText(HDC hDC) {
    if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();

    RECT rcText = m_rcItem;
    rcText.left += m_rcTextPadding.left;
    rcText.top += m_rcTextPadding.top;
    rcText.right -= m_rcTextPadding.right;
    rcText.bottom -= m_rcTextPadding.bottom;
    if (m_bCheckBoxable) {
        RECT rcCheck;
        GetCheckBoxRect(rcCheck);
        rcText.left += (rcCheck.right - rcCheck.left);
    }

    CDuiString sText = GetText();
    if( sText.IsEmpty() ) return;

    int nLinks = 0;
    if (m_bShowHtml)
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, m_dwTextColor, \
                                    NULL, NULL, nLinks, m_iFont, m_uTextStyle);
    else
        CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, m_dwTextColor, \
                                m_iFont, m_uTextStyle);
}

BOOL CListContainerHeaderItemUI::GetColumeEditable() {
    return m_bEditable;
}

void CListContainerHeaderItemUI::SetColumeEditable(BOOL bEnable) {
    m_bEditable = bEnable;
}

BOOL CListContainerHeaderItemUI::GetColumeComboable() {
    return m_bComboable;
}

void CListContainerHeaderItemUI::SetColumeComboable(BOOL bEnable) {
    m_bComboable = bEnable;
}

BOOL CListContainerHeaderItemUI::GetColumeCheckable() {
    return m_bCheckBoxable;
}
void CListContainerHeaderItemUI::SetColumeCheckable(BOOL bEnable) {
    m_bCheckBoxable = bEnable;
}
void CListContainerHeaderItemUI::SetCheck(BOOL bCheck) {
    if( m_bChecked == bCheck ) return;
    m_bChecked = bCheck;
    if( m_bChecked ) m_uCheckBoxState |= UISTATE_SELECTED;
    else m_uCheckBoxState &= ~UISTATE_SELECTED;
    Invalidate();
}

BOOL CListContainerHeaderItemUI::GetCheck() {
    return m_bChecked;
}
BOOL CListContainerHeaderItemUI::DrawCheckBoxImage(HDC hDC, LPCTSTR pStrImage) {
    RECT rcCheckBox;
    GetCheckBoxRect(rcCheckBox);
    TDrawInfo drawInfo;
    drawInfo.sDrawString = pStrImage;
    return CRenderEngine::DrawImage(hDC, m_pManager, rcCheckBox, m_rcPaint, drawInfo);
}
LPCTSTR CListContainerHeaderItemUI::GetCheckBoxNormalImage() {
    return m_sCheckBoxNormalImage;
}

void CListContainerHeaderItemUI::SetCheckBoxNormalImage(LPCTSTR pStrImage) {
    m_sCheckBoxNormalImage = pStrImage;
}

LPCTSTR CListContainerHeaderItemUI::GetCheckBoxHotImage() {
    return m_sCheckBoxHotImage;
}

void CListContainerHeaderItemUI::SetCheckBoxHotImage(LPCTSTR pStrImage) {
    m_sCheckBoxHotImage = pStrImage;
}

LPCTSTR CListContainerHeaderItemUI::GetCheckBoxPushedImage() {
    return m_sCheckBoxPushedImage;
}

void CListContainerHeaderItemUI::SetCheckBoxPushedImage(LPCTSTR pStrImage) {
    m_sCheckBoxPushedImage = pStrImage;
}

LPCTSTR CListContainerHeaderItemUI::GetCheckBoxFocusedImage() {
    return m_sCheckBoxFocusedImage;
}

void CListContainerHeaderItemUI::SetCheckBoxFocusedImage(LPCTSTR pStrImage) {
    m_sCheckBoxFocusedImage = pStrImage;
}

LPCTSTR CListContainerHeaderItemUI::GetCheckBoxDisabledImage() {
    return m_sCheckBoxDisabledImage;
}

void CListContainerHeaderItemUI::SetCheckBoxDisabledImage(LPCTSTR pStrImage) {
    m_sCheckBoxDisabledImage = pStrImage;
}
LPCTSTR CListContainerHeaderItemUI::GetCheckBoxSelectedImage() {
    return m_sCheckBoxSelectedImage;
}

void CListContainerHeaderItemUI::SetCheckBoxSelectedImage(LPCTSTR pStrImage) {
    m_sCheckBoxSelectedImage = pStrImage;
}
LPCTSTR CListContainerHeaderItemUI::GetCheckBoxForeImage() {
    return m_sCheckBoxForeImage;
}

void CListContainerHeaderItemUI::SetCheckBoxForeImage(LPCTSTR pStrImage) {
    m_sCheckBoxForeImage = pStrImage;
}
int CListContainerHeaderItemUI::GetCheckBoxWidth() const {
    return m_cxyCheckBox.cx;
}

void CListContainerHeaderItemUI::SetCheckBoxWidth(int cx) {
    if( cx < 0 ) return;
    m_cxyCheckBox.cx = cx;
}

int CListContainerHeaderItemUI::GetCheckBoxHeight()  const {
    return m_cxyCheckBox.cy;
}

void CListContainerHeaderItemUI::SetCheckBoxHeight(int cy) {
    if( cy < 0 ) return;
    m_cxyCheckBox.cy = cy;
}
void CListContainerHeaderItemUI::GetCheckBoxRect(RECT &rc) {
    memset(&rc, 0x00, sizeof(rc));
    int nItemHeight = m_rcItem.bottom - m_rcItem.top;
    rc.left = m_rcItem.left + 6;
    rc.top = m_rcItem.top + (nItemHeight - GetCheckBoxHeight()) / 2;
    rc.right = rc.left + GetCheckBoxWidth();
    rc.bottom = rc.top + GetCheckBoxHeight();
}

void CListContainerHeaderItemUI::SetOwner(CContainerUI* pOwner) {
    m_pOwner = pOwner;
}
CContainerUI* CListContainerHeaderItemUI::GetOwner() {
    return m_pOwner;
}
/////////////////////////////////////////////////////////////////////////////////////
//
//

CListExTextElementUI::CListExTextElementUI() :
    m_nLinks(0), m_nHoverLink(-1), m_pOwner(NULL), m_uCheckBoxState(0), m_bChecked(FALSE) {
    ::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
    m_cxyCheckBox.cx = m_cxyCheckBox.cy = 0;

    ::ZeroMemory(&ColumCorlorArray, sizeof(ColumCorlorArray));
}

CListExTextElementUI::~CListExTextElementUI() {
    CDuiString* pText;
    for( int it = 0; it < m_aTexts.GetSize(); it++ ) {
        pText = static_cast<CDuiString*>(m_aTexts[it]);
        if( pText ) delete pText;
    }
    m_aTexts.Empty();
}

LPCTSTR CListExTextElementUI::GetClass() const {
    return _T("ListTextExElementUI");
}

LPVOID CListExTextElementUI::GetInterface(LPCTSTR pstrName) {
    if( _tcsicmp(pstrName, _T("ListTextExElement")) == 0 ) return static_cast<CListExTextElementUI*>(this);
    return CListLabelElementUI::GetInterface(pstrName);
}

UINT CListExTextElementUI::GetControlFlags() const {
    return UIFLAG_WANTRETURN | ( (IsEnabled() && m_nLinks > 0) ? UIFLAG_SETCURSOR : 0);
}

LPCTSTR CListExTextElementUI::GetText(int iIndex) const {
    CDuiString* pText = static_cast<CDuiString*>(m_aTexts.GetAt(iIndex));
    if( pText ) return pText->GetData();
    return NULL;
}

void CListExTextElementUI::SetText(int iIndex, LPCTSTR pstrText) {
    if( m_pOwner == NULL ) return;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
    while( m_aTexts.GetSize() < pInfo->nColumns ) {
        m_aTexts.Add(NULL);
    }

    CDuiString* pText = static_cast<CDuiString*>(m_aTexts[iIndex]);
    if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

    if ( pText )
        pText->Assign(pstrText);
    else
        m_aTexts.SetAt(iIndex, new CDuiString(pstrText));
    Invalidate();
}

void CListExTextElementUI::SetOwner(CControlUI* pOwner) {
    CListElementUI::SetOwner(pOwner);
    m_pOwner = static_cast<CListUI*>(pOwner->GetInterface(_T("List")));
}

CDuiString* CListExTextElementUI::GetLinkContent(int iIndex) {
    if( iIndex >= 0 && iIndex < m_nLinks ) return &m_sLinks[iIndex];
    return NULL;
}

void CListExTextElementUI::DoEvent(TEventUI& event) {
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else CListLabelElementUI::DoEvent(event);
        return;
    }

    // When you hover over a link
    if( event.Type == UIEVENT_SETCURSOR ) {
        for( int i = 0; i < m_nLinks; i++ ) {
            if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
                ::SetCursor(::LoadCursor(NULL, IDC_HAND));
                return;
            }
        }
    }
    if( event.Type == UIEVENT_BUTTONUP && IsEnabled() ) {
        for( int i = 0; i < m_nLinks; i++ ) {
            if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
                m_pManager->SendNotify(this, DUI_MSGTYPE_LINK, i);
                return;
            }
        }
    }
    if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE ) {
        int nHoverLink = -1;
        for( int i = 0; i < m_nLinks; i++ ) {
            if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
                nHoverLink = i;
                break;
            }
        }

        if(m_nHoverLink != nHoverLink) {
            Invalidate();
            m_nHoverLink = nHoverLink;
        }
    }
    if( m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE ) {
        if(m_nHoverLink != -1) {
            Invalidate();
            m_nHoverLink = -1;
        }
    }

    //检查是否需要显示编辑框或者组合框
    CListExUI * pListCtrl = (CListExUI *)m_pOwner;
    int nColum = HitTestColum(event.ptMouse);
    if(event.Type == UIEVENT_BUTTONDOWN && m_pOwner->IsFocused()) {
        RECT rc = {0, 0, 0, 0};
        if (nColum >= 0) {
            GetColumRect(nColum, rc);
            ::InflateRect(&rc, -2, -2);
        }

        pListCtrl->OnListItemClicked(GetIndex(), nColum, &rc, GetText(nColum));
    }

    //检查是否需要显示CheckBox
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    for( int i = 0; i < pInfo->nColumns; i++ ) {
        if (pListCtrl->CheckColumCheckBoxable(i)) {
            RECT rcCheckBox;
            GetCheckBoxRect(i, rcCheckBox);

            if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK ) {
                if( ::PtInRect(&rcCheckBox, event.ptMouse)) {
                    m_uCheckBoxState |= UISTATE_PUSHED | UISTATE_CAPTURED;
                    Invalidate();
                }
            } else if( event.Type == UIEVENT_MOUSEMOVE ) {
                if( (m_uCheckBoxState & UISTATE_CAPTURED) != 0 ) {
                    if( ::PtInRect(&rcCheckBox, event.ptMouse) )
                        m_uCheckBoxState |= UISTATE_PUSHED;
                    else
                        m_uCheckBoxState &= ~UISTATE_PUSHED;
                    Invalidate();
                }
            } else if( event.Type == UIEVENT_BUTTONUP ) {
                if( (m_uCheckBoxState & UISTATE_CAPTURED) != 0 ) {
                    if( ::PtInRect(&rcCheckBox, event.ptMouse) ) {
                        SetCheck(!GetCheck());
                        if (m_pManager) {
                            m_pManager->SendNotify(this, DUI_MSGTYPE_LISTITEMCHECKED, MAKEWPARAM(GetIndex(), 0), m_bChecked);
                        }
                    }
                    m_uCheckBoxState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
                    Invalidate();
                }
            } else if( event.Type == UIEVENT_MOUSEENTER ) {
                if( ::PtInRect(&rcCheckBox, event.ptMouse) ) {
                    m_uCheckBoxState |= UISTATE_HOT;
                    Invalidate();
                }
            } else if( event.Type == UIEVENT_MOUSELEAVE ) {
                m_uCheckBoxState &= ~UISTATE_HOT;
                Invalidate();
            }
        }
    }

    CListLabelElementUI::DoEvent(event);
}

SIZE CListExTextElementUI::EstimateSize(SIZE szAvailable) {
    TListInfoUI* pInfo = NULL;
    if( m_pOwner ) pInfo = m_pOwner->GetListInfo();

    SIZE cXY = m_cxyFixed;
    if( cXY.cy == 0 && m_pManager != NULL ) {
        cXY.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
        if( pInfo ) cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
    }

    return cXY;
}

void CListExTextElementUI::DrawItemText(HDC hDC, const RECT& rcItem) {
    if( m_pOwner == NULL ) return;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    DWORD iTextColor = pInfo->dwTextColor;

    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        iTextColor = pInfo->dwHotTextColor;
    }
    if( IsSelected() ) {
        iTextColor = pInfo->dwSelectedTextColor;
    }
    if( !IsEnabled() ) {
        iTextColor = pInfo->dwDisabledTextColor;
    }
    IListCallbackUI* pCallback = m_pOwner->GetTextCallback();
    //DUIASSERT(pCallback);
    //if( pCallback == NULL ) return;

    CListExUI * pListCtrl = (CListExUI *)m_pOwner;
    m_nLinks = 0;
    int nLinks = lengthof(m_rcLinks);
    for( int i = 0; i < pInfo->nColumns; i++ ) {
        RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };

        DWORD iTextBkColor = 0;
        if (GetColumItemColor(i, iTextBkColor)) {
            CRenderEngine::DrawColor(hDC, rcItem, iTextBkColor);
        }

        rcItem.left += pInfo->rcTextPadding.left;
        rcItem.right -= pInfo->rcTextPadding.right;
        rcItem.top += pInfo->rcTextPadding.top;
        rcItem.bottom -= pInfo->rcTextPadding.bottom;

        //检查是否需要显示CheckBox
        if (pListCtrl->CheckColumCheckBoxable(i)) {
            RECT rcCheckBox;
            GetCheckBoxRect(i, rcCheckBox);
            rcItem.left += (rcCheckBox.right - rcCheckBox.left);
        }

        CDuiString strText;//不使用LPCTSTR，否则限制太多 by cddjr 2011/10/20
        if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);
        else strText.Assign(GetText(i));
        if (pInfo->bShowHtml)
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
                                        &m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], nLinks, pInfo->nFont, pInfo->uTextStyle);
        else
            CRenderEngine::DrawText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
                                    pInfo->nFont, pInfo->uTextStyle);

        m_nLinks += nLinks;
        nLinks = lengthof(m_rcLinks) - m_nLinks;
    }
    for( int i = m_nLinks; i < lengthof(m_rcLinks); i++ ) {
        ::ZeroMemory(m_rcLinks + i, sizeof(RECT));
        ((CDuiString*)(m_sLinks + i))->Empty();
    }
}
void CListExTextElementUI::PaintStatusImage(HDC hDC) {
    CListExUI * pListCtrl = (CListExUI *)m_pOwner;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    for( int i = 0; i < pInfo->nColumns; i++ ) {
        if (pListCtrl->CheckColumCheckBoxable(i)) {
            RECT rcCheckBox;
            GetCheckBoxRect(i, rcCheckBox);

            m_uCheckBoxState &= ~UISTATE_PUSHED;

            if( (m_uCheckBoxState & UISTATE_SELECTED) != 0 ) {
                if( !m_sCheckBoxSelectedImage.IsEmpty() ) {
                    if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxSelectedImage, rcCheckBox) ) {}
                    else goto Label_ForeImage;
                }
            }

            if( IsFocused() ) m_uCheckBoxState |= UISTATE_FOCUSED;
            else m_uCheckBoxState &= ~ UISTATE_FOCUSED;
            if( !IsEnabled() ) m_uCheckBoxState |= UISTATE_DISABLED;
            else m_uCheckBoxState &= ~ UISTATE_DISABLED;

            if( (m_uCheckBoxState & UISTATE_DISABLED) != 0 ) {
                if( !m_sCheckBoxDisabledImage.IsEmpty() ) {
                    if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxDisabledImage, rcCheckBox) ) {}
                    else return;
                }
            } else if( (m_uCheckBoxState & UISTATE_PUSHED) != 0 ) {
                if( !m_sCheckBoxPushedImage.IsEmpty() ) {
                    if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxPushedImage, rcCheckBox) ) {}
                    else return;
                }
            } else if( (m_uCheckBoxState & UISTATE_HOT) != 0 ) {
                if( !m_sCheckBoxHotImage.IsEmpty() ) {
                    if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxHotImage, rcCheckBox) ) {}
                    else return;
                }
            } else if( (m_uCheckBoxState & UISTATE_FOCUSED) != 0 ) {
                if( !m_sCheckBoxFocusedImage.IsEmpty() ) {
                    if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxFocusedImage, rcCheckBox) ) {}
                    else return;
                }
            }

            if( !m_sCheckBoxNormalImage.IsEmpty() ) {
                if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxNormalImage, rcCheckBox) ) {}
                else return;
            }

Label_ForeImage:
            if( !m_sCheckBoxForeImage.IsEmpty() ) {
                if( !DrawCheckBoxImage(hDC, (LPCTSTR)m_sCheckBoxForeImage, rcCheckBox) ) {}
            }
        }
    }
}
BOOL CListExTextElementUI::DrawCheckBoxImage(HDC hDC, LPCTSTR pStrImage, RECT& rcCheckBox) {
    TDrawInfo drawInfo;
    drawInfo.sDrawString = pStrImage;
    return CRenderEngine::DrawImage(hDC, m_pManager, rcCheckBox, m_rcItem, drawInfo);
}
void CListExTextElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    if( _tcsicmp(pstrName, _T("checkboxwidth")) == 0 ) SetCheckBoxWidth(_ttoi(pstrValue));
    else if( _tcsicmp(pstrName, _T("checkboxheight")) == 0 ) SetCheckBoxHeight(_ttoi(pstrValue));
    else if( _tcsicmp(pstrName, _T("checkboxnormalimage")) == 0 ) SetCheckBoxNormalImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxhotimage")) == 0 ) SetCheckBoxHotImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxpushedimage")) == 0 ) SetCheckBoxPushedImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxfocusedimage")) == 0 ) SetCheckBoxFocusedImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxdisabledimage")) == 0 ) SetCheckBoxDisabledImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxselectedimage")) == 0 ) SetCheckBoxSelectedImage(pstrValue);
    else if( _tcsicmp(pstrName, _T("checkboxforeimage")) == 0 ) SetCheckBoxForeImage(pstrValue);
    else CListLabelElementUI::SetAttribute(pstrName, pstrValue);
}
LPCTSTR CListExTextElementUI::GetCheckBoxNormalImage() {
    return m_sCheckBoxNormalImage;
}

void CListExTextElementUI::SetCheckBoxNormalImage(LPCTSTR pStrImage) {
    m_sCheckBoxNormalImage = pStrImage;
}

LPCTSTR CListExTextElementUI::GetCheckBoxHotImage() {
    return m_sCheckBoxHotImage;
}

void CListExTextElementUI::SetCheckBoxHotImage(LPCTSTR pStrImage) {
    m_sCheckBoxHotImage = pStrImage;
}

LPCTSTR CListExTextElementUI::GetCheckBoxPushedImage() {
    return m_sCheckBoxPushedImage;
}

void CListExTextElementUI::SetCheckBoxPushedImage(LPCTSTR pStrImage) {
    m_sCheckBoxPushedImage = pStrImage;
}

LPCTSTR CListExTextElementUI::GetCheckBoxFocusedImage() {
    return m_sCheckBoxFocusedImage;
}

void CListExTextElementUI::SetCheckBoxFocusedImage(LPCTSTR pStrImage) {
    m_sCheckBoxFocusedImage = pStrImage;
}

LPCTSTR CListExTextElementUI::GetCheckBoxDisabledImage() {
    return m_sCheckBoxDisabledImage;
}

void CListExTextElementUI::SetCheckBoxDisabledImage(LPCTSTR pStrImage) {
    m_sCheckBoxDisabledImage = pStrImage;
}
LPCTSTR CListExTextElementUI::GetCheckBoxSelectedImage() {
    return m_sCheckBoxSelectedImage;
}

void CListExTextElementUI::SetCheckBoxSelectedImage(LPCTSTR pStrImage) {
    m_sCheckBoxSelectedImage = pStrImage;
}
LPCTSTR CListExTextElementUI::GetCheckBoxForeImage() {
    return m_sCheckBoxForeImage;
}

void CListExTextElementUI::SetCheckBoxForeImage(LPCTSTR pStrImage) {
    m_sCheckBoxForeImage = pStrImage;
}

bool CListExTextElementUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return true;
    DrawItemBk(hDC, m_rcItem);
    PaintStatusImage(hDC);
    DrawItemText(hDC, m_rcItem);
    return true;
}
void CListExTextElementUI::GetCheckBoxRect(int nIndex, RECT &rc) {
    memset(&rc, 0x00, sizeof(rc));
    int nItemHeight = m_rcItem.bottom - m_rcItem.top;
    rc.left = m_rcItem.left + 6;
    rc.top = m_rcItem.top + (nItemHeight - GetCheckBoxHeight()) / 2;
    rc.right = rc.left + GetCheckBoxWidth();
    rc.bottom = rc.top + GetCheckBoxHeight();
}
int CListExTextElementUI::GetCheckBoxWidth() const {
    return m_cxyCheckBox.cx;
}

void CListExTextElementUI::SetCheckBoxWidth(int cx) {
    if( cx < 0 ) return;
    m_cxyCheckBox.cx = cx;
}

int CListExTextElementUI::GetCheckBoxHeight()  const {
    return m_cxyCheckBox.cy;
}

void CListExTextElementUI::SetCheckBoxHeight(int cy) {
    if( cy < 0 ) return;
    m_cxyCheckBox.cy = cy;
}

void CListExTextElementUI::SetCheck(BOOL bCheck) {
    if( m_bChecked == bCheck ) return;
    m_bChecked = bCheck;
    if( m_bChecked ) m_uCheckBoxState |= UISTATE_SELECTED;
    else m_uCheckBoxState &= ~UISTATE_SELECTED;
    Invalidate();
}

BOOL  CListExTextElementUI::GetCheck() const {
    return m_bChecked;
}

int CListExTextElementUI::HitTestColum(POINT ptMouse) {
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    for( int i = 0; i < pInfo->nColumns; i++ ) {
        RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };
        rcItem.left += pInfo->rcTextPadding.left;
        rcItem.right -= pInfo->rcTextPadding.right;
        rcItem.top += pInfo->rcTextPadding.top;
        rcItem.bottom -= pInfo->rcTextPadding.bottom;

        if( ::PtInRect(&rcItem, ptMouse)) {
            return i;
        }
    }
    return -1;
}

BOOL CListExTextElementUI::CheckColumEditable(int nColum) {
    CListExUI* pListExUI = static_cast<CListExUI*>(m_pOwner);
    return pListExUI->CheckColumEditable(nColum);
}

void CListExTextElementUI::GetColumRect(int nColum, RECT &rc) {
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    rc.left = pInfo->rcColumn[nColum].left;
    rc.top  = m_rcItem.top;
    rc.right = pInfo->rcColumn[nColum].right;
    rc.bottom = m_rcItem.bottom;
}

void CListExTextElementUI::SetColumItemColor(int nColum, DWORD iBKColor) {
    ColumCorlorArray[nColum].bEnable = TRUE;
    ColumCorlorArray[nColum].iBKColor = iBKColor;
    Invalidate();
}

BOOL CListExTextElementUI::GetColumItemColor(int nColum, DWORD& iBKColor) {
    if (!ColumCorlorArray[nColum].bEnable) {
        return FALSE;
    }
    iBKColor = ColumCorlorArray[nColum].iBKColor;
    return TRUE;
}

} // namespace DuiLib

