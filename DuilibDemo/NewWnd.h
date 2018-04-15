#pragma once
class CNewWnd : public WindowImplBase {
public:
    CLabelUI* m_labCaption;

public:
    CNewWnd();
    ~CNewWnd();
    virtual LRESULT		HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LPCTSTR		GetWindowClassName() const {
        return _T("DuilibClass");
    }
    virtual CDuiString	GetSkinFile() {
        return _T("NewWnd.xml");
    }
    virtual LPCTSTR GetResourceID() const {
        return  MAKEINTRESOURCE(IDR_ZIPRES1);
    }
    virtual UILIB_RESOURCETYPE GetResourceType() const {
        return UILIB_ZIPRESOURCE;
    };
    virtual CDuiString GetSkinFolder() {
        return _T("");
    }
    void OnFinalMessage(HWND /*hWnd*/) {
        delete this;
    };
    virtual void InitWindow();
    void Notify(TNotifyUI& msg);
private:
    //³õÊ¼»¯¿Ø¼þ
    bool InitControl(void);
};

