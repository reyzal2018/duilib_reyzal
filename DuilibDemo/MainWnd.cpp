#include "stdafx.h"
#include "MainWnd.h"
#include "NewWnd.h"
#include <sstream>
#include <algorithm>
using namespace std;


#define WM_ADDLISTITEM WM_USER + 101
struct NormalDataItem {
    string name;
    int gender;
    int age;
    int score;
};
vector<NormalDataItem> g_vecListview1Data;
//操作列表时线程内容结构体
struct ThreadParam {
    HWND hWnd;
    CListUI* pList;
    CButtonUI* pButton;
};


DUI_BEGIN_MESSAGE_MAP(CMainWnd, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnItemSelect)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMACTIVATE, OnItemActivate)
DUI_ON_MSGTYPE(DUI_MSGTYPE_HEADERCLICK, OnHeaderClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_LINK, OnLink)
DUI_END_MESSAGE_MAP()

//重载消息接管函数
LRESULT CMainWnd::HandleMessage(UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam) {
    BOOL bHandled;
    LRESULT lRet = 0;
    if (uMsg == WM_ADDLISTITEM) {
        lRet = OnAddListNormalItem(uMsg, wParam, lParam, bHandled);
        if (bHandled) {
            return lRet;
        }
    } else if (WM_MENUCLICK == uMsg) {
        CDuiString strFormat;
        MenuCmd* pCmd = (MenuCmd*)wParam;
        strFormat.Format(_T("点击菜单[%s--%s]"), pCmd->szName, pCmd->szText);
        ShowMessage(strFormat);
        bHandled = TRUE;
    }
    return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}
//重载消息响应函数
void CMainWnd::Notify(TNotifyUI& msg) {
    return WindowImplBase::Notify(msg);
}

void CMainWnd::InitWindow() {
    srand(GetTickCount());
    InitControl();
    GenerateListFixData();
    m_tabMain->SelectItem(0);
    m_optListview1->Selected(true);
    //初始化事件
    m_listexNormal->InitListCtrl();
    GenerateListexNormalData();
    GenerateTileData();

}
void CMainWnd::OnSelectChanged(TNotifyUI& msg) {
    if (msg.pSender == m_optNormal) {
        m_tabMain->SelectItem(0);
    } else if (msg.pSender == m_optListview) {
        m_tabMain->SelectItem(1);
    } else if (msg.pSender == m_optTreeview) {
        m_tabMain->SelectItem(2);
    } else if (msg.pSender == m_optNormal1) {
        m_tabMain->SelectItem(3);
    } else if (msg.pSender == m_optTestRadio1 ||
               msg.pSender == m_optTestRadio2 ||
               msg.pSender == m_optTestRadio3 ||
               msg.pSender == m_optTestRadio4 ||
               msg.pSender == m_optTestRadio5
              ) {
        ShowMessage(msg.pSender->GetText() + _T("选中"));
    } else if (msg.pSender == m_chkTestCheck1 ||
               msg.pSender == m_chkTestCheck2 ||
               msg.pSender == m_chkTestCheck3 ||
               msg.pSender == m_chkTestCheck4 ||
               msg.pSender == m_chkTestCheck5
              ) {
        ShowMessage(msg.pSender->GetText() + CDuiString((static_cast<CCheckBoxUI*>(msg.pSender))->IsSelected() ? _T("选中") : _T("未选中")));
    } else if (msg.pSender == m_optListview1) {
        m_tabListview->SelectItem(0);
    } else if (msg.pSender == m_optListview2) {
        m_tabListview->SelectItem(1);
    } else if (msg.pSender == m_optListview3) {
        m_tabListview->SelectItem(2);
    } else if (msg.pSender == m_optListview4) {
        m_tabListview->SelectItem(3);
    }

}

void CMainWnd::OnClick(TNotifyUI& msg) {
    if (msg.pSender == m_btnMenu) {
        CMenuWnd* pMenu;
        CDuiPoint point = msg.ptMouse;
        ClientToScreen(m_hWnd, &point);
        pMenu = CMenuWnd::CreateMenu(NULL, _T("menu.xml"), point, &m_pm);
    } else if (msg.pSender == m_btnNormal ||
               msg.pSender == m_btnTooltip ||
               msg.pSender == m_btnFont
              ) {
        ShowMessage(msg.pSender->GetText());
    } else if (msg.pSender == m_btnDisabled) {
        m_btnDisabled->SetEnabled(false);
        ShowMessage(msg.pSender->GetText());
    } else if (msg.pSender == m_btnWindowNormal) {
        CNewWnd* pNewWnd = new CNewWnd();
        pNewWnd->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        pNewWnd->CenterWindow();
        pNewWnd->ShowWindow();
    } else if (msg.pSender == m_btnWindowDomodal) {
        CNewWnd* pNewWnd = new CNewWnd();
        pNewWnd->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
        pNewWnd->CenterWindow();
        pNewWnd->ShowModal();
    } else if (msg.pSender == m_btnAddListview1Data) {
        OnAddListview1Data();
    } else if (msg.pSender->GetName() == _T("lab_item_install")) {
        CDuiString strMsg;
        strMsg.Format(_T("点击了第%d项"), msg.pSender->GetTag() + 1);
        ShowMessage(strMsg);
    } else if (msg.pSender->GetName() == _T("btn_tile_soft")) {
        CDuiString strMsg;
        strMsg.Format(_T("点击了第%d项"), msg.pSender->GetTag() + 1);
        ShowMessage(strMsg);
    }
    return WindowImplBase::OnClick(msg);
}

void CMainWnd::OnItemSelect(TNotifyUI& msg) {
    if (msg.pSender == m_comNormal) {
        int index = m_comNormal->GetCurSel();
        if (index >= 0 && index < m_comNormal->GetCount()) {
            ShowMessage(m_comNormal->GetItemAt(index)->GetText());
        }
    }
}

void CMainWnd::OnHeaderClick(TNotifyUI& msg) {
    CListHeaderItemUI* pListItem = static_cast<CListHeaderItemUI *> (msg.pSender);
    int iHeaderIndex = m_listNormal->GetHeader()->GetItemIndex(pListItem);
    if (iHeaderIndex != -1) {
        if (iHeaderIndex == 0)
            sort(g_vecListview1Data.begin(), g_vecListview1Data.end(), [](NormalDataItem & item1, NormalDataItem & item2) {
            return item1.name < item2.name;
        });
        else if (iHeaderIndex == 1)
            sort(g_vecListview1Data.begin(), g_vecListview1Data.end(), [](NormalDataItem & item1, NormalDataItem & item2) {
            return item1.gender < item2.gender;
        });
        else if (iHeaderIndex == 2)
            sort(g_vecListview1Data.begin(), g_vecListview1Data.end(), [](NormalDataItem & item1, NormalDataItem & item2) {
            return item1.age < item2.age;
        });
        else if (iHeaderIndex == 3)
            sort(g_vecListview1Data.begin(), g_vecListview1Data.end(), [](NormalDataItem & item1, NormalDataItem & item2) {
            return item1.score < item2.score;
        });
        m_listNormal->NeedUpdate();
    }
}

void CMainWnd::OnLink(TNotifyUI& msg) {
    CTextUI* pText = static_cast<CTextUI*>(msg.pSender);
    CDuiString* pStr = pText->GetLinkContent(0);
    if(pStr)
        ShowMessage(*pStr);
}

void CMainWnd::OnItemActivate(TNotifyUI& msg) {
    CListElementUI* pListItem = static_cast<CListElementUI*>(msg.pSender);
    if (pListItem && pListItem->GetOwner() == m_listNormal) {
        int iIndex = msg.pSender->GetTag();
        CDuiString strMessage = _T("Click: ");
        NormalDataItem& item = g_vecListview1Data[iIndex];
#ifdef _UNICODE
        int iLen = item.name.length();
        LPWSTR lpText = new WCHAR[iLen + 1];
        ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, item.name.c_str(), -1, (LPWSTR)lpText, iLen);
        strMessage += lpText;
        delete[] lpText;
#else
        sMessage += item.name.c_str();
#endif
        ::MessageBox(NULL, strMessage, _T("提示"), MB_OK);
    }
}

void CMainWnd::ShowMessage(LPCTSTR strMsg) {
    MessageBox(NULL, strMsg, _T("提示"), MB_OK);
}

//初始化控件
bool CMainWnd::InitControl(void) {
    m_btnMenu = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_menu")));

    //tab栏
    m_optNormal = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_normal")));
    m_optListview = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_listview")));
    m_optTreeview = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_treeview")));
    m_optNormal1 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_normal1")));

    m_tabMain = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("tab_main")));

    //按钮
    m_btnNormal = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_normal")));
    m_btnDisabled = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_disabled")));
    m_btnTooltip = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_tooltip")));
    m_btnFont = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_font")));

    //选择框
    m_chkTestCheck1 = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("chk_test_check1")));
    m_chkTestCheck2 = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("chk_test_check2")));
    m_chkTestCheck3 = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("chk_test_check3")));
    m_chkTestCheck4 = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("chk_test_check4")));
    m_chkTestCheck5 = static_cast<CCheckBoxUI*>(m_pm.FindControl(_T("chk_test_check5")));

    //单选
    m_optTestRadio1 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_test_radio1")));
    m_optTestRadio2 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_test_radio2")));
    m_optTestRadio3 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_test_radio3")));
    m_optTestRadio4 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_test_radio4")));
    m_optTestRadio5 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_test_radio5")));

    //combo
    m_comNormal = static_cast<CComboUI*>(m_pm.FindControl(_T("com_normal")));

    //窗口弹出测试
    m_btnWindowNormal = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_window_normal")));
    m_btnWindowDomodal = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_window_domodal")));

    //listview
    m_btnAddListview1Data = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_add_listview1_data")));
    m_tabListview = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("tab_listview")));
    m_optListview1 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_listview1")));
    m_optListview2 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_listview2")));
    m_optListview3 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_listview3")));
    m_optListview4 = static_cast<COptionUI*>(m_pm.FindControl(_T("opt_listview4")));
    m_listNormal = static_cast<CListUI*>(m_pm.FindControl(_T("list_normal")));
    m_listFix = static_cast<CListUI*>(m_pm.FindControl(_T("list_fix")));
    m_listexNormal = static_cast<CListExUI*>(m_pm.FindControl(_T("listex_normal")));
    m_tileSoft = static_cast<CTileLayoutUI*>(m_pm.FindControl(_T("tile_soft")));

    assert(
        m_btnMenu &&
        m_optNormal &&
        m_optListview &&
        m_optTreeview &&
        m_optNormal1 &&
        m_tabMain &&
        m_btnNormal &&
        m_btnDisabled &&
        m_btnTooltip &&
        m_btnFont &&
        m_chkTestCheck1 &&
        m_chkTestCheck2 &&
        m_chkTestCheck3 &&
        m_chkTestCheck4 &&
        m_chkTestCheck5 &&
        m_optTestRadio1 &&
        m_optTestRadio2 &&
        m_optTestRadio3 &&
        m_optTestRadio4 &&
        m_optTestRadio5 &&
        m_comNormal &&
        m_btnWindowNormal &&
        m_btnWindowDomodal &&
        m_btnAddListview1Data &&
        m_optListview1 &&
        m_optListview2 &&
        m_optListview3 &&
        m_optListview4 &&
        m_listNormal &&
        m_listFix &&
        m_listexNormal &&
        m_tileSoft
    );
    return true;
}

void CMainWnd::GenerateTileData() {
    m_tileSoft->RemoveAll();
    CDuiString strTemp;
    for (size_t i = 0; i < 100; i++) {
        CDialogBuilder builder;
        CDuiString strXml = GetSkinFolder();
        strXml += _T("TileItem.xml");
        STRINGorID strXml1(strXml);
        CHorizontalLayoutUI* pLine = (CHorizontalLayoutUI*)(builder.Create(strXml1, (UINT)0));
        if (pLine != NULL) {
            CDuiString strUserData;
            if (!m_tileSoft->AddAt(pLine, i))
                return;
            CButtonUI* btn = (COptionUI*)pLine->FindSubControl(_T("btn_tile_soft"));
            //设置下标
            btn->SetTag(i);
            strTemp.Format(_T("软件 %d"), i + 1);
            btn->SetText(strTemp);
            btn->SetToolTip(strTemp);
            strTemp.Format(_T("bkimage=\"file='icon\\%d.png' dest='16,16,64,64' source='0,0,0,0'\""), i % 7 + 1);
            btn->SetAttributeList(strTemp);
        }
    }
}

void CMainWnd::GenerateListexNormalData() {
    CDuiString strTemp;
    for (int i = 0; i < 500; i++) {
        CListExTextElementUI* pItem = new CListExTextElementUI();
        pItem->SetFixedHeight(30);
        m_listexNormal->Add(pItem);
        pItem->SetAttributeList(_T("checkboxwidth=\"16\" checkboxheight=\"16\" checkboxnormalimage=\"file='unchecked.png'\" checkboxselectedimage=\"file='checked.png'\""));
        strTemp.Format(_T("学生 %d"), i + 1);
        pItem->SetText(1, strTemp);
        strTemp.Format(_T("2000%04d"), i);
        pItem->SetText(2, strTemp);
        strTemp.Format(_T("%d"), rand() % 50 + 50);
        pItem->SetText(3, strTemp);
    }
}

void CMainWnd::GenerateListFixData() {
    m_listFix->RemoveAll();
    for (size_t i = 0; i < 1000; i++) {
        CDialogBuilder builder;
        CDuiString strXml = GetSkinFolder();
        strXml += _T("ListviewItem.xml");
        STRINGorID strXml1(strXml);
        CListContainerElementUI* pLine = (CListContainerElementUI*)(builder.Create(strXml1, (UINT)0));
        if (pLine != NULL) {
            CDuiString strTemp;
            CDuiString strUserData;
            pLine->SetFixedHeight(80);
            m_listFix->AddAt(pLine, i);
            CLabelUI* labItemIcon = (CLabelUI*)pLine->FindSubControl(_T("lab_item_icon"));
            CLabelUI* labItemName = (CLabelUI*)pLine->FindSubControl(_T("lab_item_name"));
            CLabelUI* labItemDes = (CLabelUI*)pLine->FindSubControl(_T("lab_item_des"));
            CProgressUI* proItemRankimg = (CProgressUI*)pLine->FindSubControl(_T("lab_item_rankimg"));
            CLabelUI* labItemRanknum = (CLabelUI*)pLine->FindSubControl(_T("lab_item_ranknum"));
            CLabelUI* labItemSize = (CLabelUI*)pLine->FindSubControl(_T("lab_item_size"));
            CButtonUI* btnItemInstall = (CButtonUI*)pLine->FindSubControl(_T("lab_item_install"));
            //设置下标
            int iIndex = i % 7;
            strTemp.Format(_T("icon\\%d.png"), iIndex + 1);
            labItemIcon->SetBkImage(strTemp);
            strUserData.Format(_T("%d"), i);
            strTemp.Format(_T("%.2lfMB"), (rand() % 1000 + 100) * 0.01);
            labItemSize->SetText(strTemp);
            int nRank = rand() % 100;
            strTemp.Format(_T("%.1lf分"), nRank * 0.1);
            labItemRanknum->SetText(strTemp);
            proItemRankimg->SetValue(nRank);
            strTemp.Format(_T("名称 %d"), i + 1);
            labItemName->SetText(strTemp);
            strTemp.Format(_T("描述 %d"), i + 1);
            labItemDes->SetText(strTemp);
            btnItemInstall->SetTag(i);
            if (i == 0) {
                m_listFix->SelectItem(i);
            }
        }
    }
}


static DWORD WINAPI AddListview1DataThread(LPVOID lpParameter) {
    struct ThreadParam* pThreadParam = (struct ThreadParam *)lpParameter;
    CListUI* pList = pThreadParam->pList;
    CButtonUI* pButton = pThreadParam->pButton;
    //循环添加测试数据
    srand(GetTickCount());
    for (int i = 0; i < 100; i++) {
        NormalDataItem item;
        std::stringstream ss;
        ss << "学生 " << (i + 1);
        item.name = ss.str();
        item.gender = rand() % 2;
        item.age = rand() % 90 + 10;
        item.score = rand() % 50 + 50;
        g_vecListview1Data.push_back(item);
        CListTextElementUI* pListElement = new CListTextElementUI;
        pListElement->SetTag(i);
        if (pListElement != NULL) {
            ::PostMessage(pThreadParam->hWnd, WM_ADDLISTITEM, 0L, (LPARAM)pListElement);
        }
        //为了展示添加的动态效果，故放慢了添加速度，同时可以看到添加过程中界面仍然可以响应
        ::Sleep(50);
    }
    //------------------------------------------
    delete pThreadParam;
    pButton->SetEnabled(true);
    return 0;
}

LRESULT CMainWnd::OnAddListNormalItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    CListTextElementUI* pListElement = (CListTextElementUI*)lParam;
    m_listNormal->Add(pListElement);
    bHandled = true;
    return 0;
}

void CMainWnd::OnAddListview1Data() {
    struct ThreadParam *pThreadParam = new ThreadParam;
    m_listNormal->RemoveAll();
    g_vecListview1Data.clear();
    m_listNormal->SetTextCallback(this);

    pThreadParam->hWnd = GetHWND();
    pThreadParam->pList = m_listNormal;
    pThreadParam->pButton = m_btnAddListview1Data;
    m_btnAddListview1Data->SetEnabled(false);

    HANDLE hThread = CreateThread(NULL, 0, AddListview1DataThread, (LPVOID)pThreadParam, 0, NULL);
}

CDuiString g_strTemp;

LPCTSTR CMainWnd::GetItemText(CControlUI* pControl, int iIndex, int iSubItem) {
    if (iIndex < 0 || iIndex >= g_vecListview1Data.size()) {
        return NULL;
    }
    NormalDataItem& itemData = g_vecListview1Data[iIndex];
    switch (iSubItem) {
    case 0: {
#ifdef _UNICODE
        int iLen = itemData.name.length();
        LPWSTR lpText = new WCHAR[iLen + 1];
        ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, itemData.name.c_str(), -1, (LPWSTR)lpText, iLen);
        g_strTemp = lpText;
        delete[] lpText;
#else
        g_strTemp = itemData.name.c_str();
#endif
        break;
    }
    case 1:
        g_strTemp = itemData.gender == 0 ? _T("男") : _T("女");
        break;
    case 2:
        g_strTemp.Format(_T("%d"), itemData.age);
        break;
    case 3:
        g_strTemp.Format(_T("%d"), itemData.score);
        break;
    default:
        break;
    }
    return g_strTemp;
}
