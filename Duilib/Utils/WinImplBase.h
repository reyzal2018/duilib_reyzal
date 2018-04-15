/**
 * @file WinImplBase.h
 * @bref 通用窗口类
 * @details
 */
#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP

namespace DuiLib {
/**
 * @bref 加载的资源类型
 * @details
 */
enum UILIB_RESOURCETYPE {
    UILIB_FILE = 1,			/**< 来自磁盘文件 */
    UILIB_ZIP,				/**< 来自磁盘zip压缩包 */
    UILIB_RESOURCE,			/**< 来自资源 */
    UILIB_ZIPRESOURCE,		/**< 来自资源的zip压缩包 */
};

/**
 * @bref 封装好的一个通用窗口类
 * @details 此类处理了大部分默认消息,创建窗口时只需要继承本类即可
 */
class DUILIB_API WindowImplBase
    : public CWindowWnd
    , public CNotifyPump
    , public INotifyUI
    , public IMessageFilterUI
    , public IDialogBuilderCallback {
public:
    WindowImplBase() {};
    virtual ~WindowImplBase() {};
    /**
     * @bref 初始化窗口完成后调用
     * @details
     * @return 无
     */
    virtual void InitWindow() {};

    /**
    * @bref 窗口关闭之后调用
    * @details
    * @param hWnd 当前窗口句柄
    * @return 无
    */
    virtual void OnFinalMessage(HWND hWnd);

    /**
    * @bref 处理duilib中的消息
    * @details
    * @param msg是一个TNotifyUI结构，包含当前消息的内容
    * @return 无
    */
    virtual void Notify(TNotifyUI& msg);

    DUI_DECLARE_MESSAGE_MAP()

    /**
    * @bref 处理duilib中的click事件
    * @details
    * @param msg是一个TNotifyUI结构，包含当前消息的内容
    */
    virtual void OnClick(TNotifyUI& msg);

protected:
    /**
    * @bref 获取窗口皮肤所在路径
    * @return 窗口皮肤所在路径
    */
    virtual CDuiString GetSkinFolder() = 0;

    /**
    * @bref 获取窗口皮肤文件名
    * @return 窗口皮肤文件名
    */
    virtual CDuiString GetSkinFile() = 0;
    /**
    * @bref 获取当前窗口类名
    * @return 当前窗口类名
    */
    virtual LPCTSTR GetWindowClassName(void) const = 0;
    /**
    * @bref 处理ESE键和Enter键的WM_KEYDOWN消息
    * @param wParam表示当前按下键的VK码
    * @return
    */
    virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

    static LPBYTE m_lpResourceZIPBuffer;
    CPaintManagerUI m_pm;

public:
    virtual UINT GetClassStyle() const;
    virtual UILIB_RESOURCETYPE GetResourceType() const;
    virtual CDuiString GetZIPFileName() const;
    virtual LPCTSTR GetResourceID() const;
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);
    virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
    virtual LRESULT OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
#endif
    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LONG GetStyle();
};
}

#endif // WIN_IMPL_BASE_HPP
