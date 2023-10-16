#pragma once
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlmisc.h>
#include <MocapApi.h>
#include "Aero.h"
#include "resource.h"
#include "mocap_panel.h"
#include <atlcrack.h>
#include "mocap_api_service.h"
#include "debug.h"


class CMainFrame
    : public CAeroFrameImpl<CMainFrame> 
{
public:
    DECLARE_FRAME_WND_CLASS_EX(NULL, IDR_MAINFRAME, 0, -1)

    BEGIN_MSG_MAP(CMainFrame)
        CHAIN_MSG_MAP(CAeroFrameImpl<CMainFrame>)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MSG_WM_CLOSE(OnClose)
    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled) 
    {
        HWND hWndCmdBar = _cmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
        _cmdBar.AttachMenu(GetMenu());
        _cmdBar.LoadImages(IDR_MAINFRAME);
        SetMenu(NULL);

        CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
        AddSimpleReBarBand(hWndCmdBar);

        CreateSimpleStatusBar();

        _mocapPanel.Create(m_hWnd, NULL);
        CRect rcMocapPanel;
        _mocapPanel.GetWindowRect(&rcMocapPanel);
        RECT rc;
        GetClientRect(&rc);
        CRect rcWndToolbar;
        CWindow(m_hWndToolBar).GetWindowRect(rcWndToolbar);
        ScreenToClient(rcWndToolbar);
        rc.top += rcWndToolbar.Height();
        rc.bottom -= 22;
        _mocapPanel.MoveWindow(&rc);
        return 0;
    }

    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
    {
        bHandled = FALSE;
        return 1;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
    {
        if (::IsWindow(m_hWndToolBar)) {
            CSize sz(LOWORD(lParam), HIWORD(lParam));
            CRect rcWndToolbar;
            CWindow(m_hWndToolBar).GetWindowRect(rcWndToolbar);
            ScreenToClient(rcWndToolbar);
            sz.cy -= rcWndToolbar.Height();
            CRect rcWndStatusBar;
            CWindow(m_hWndStatusBar).GetWindowRect(rcWndStatusBar);
            sz.cy -= rcWndStatusBar.Height();
            CRect rcMocapPanel;
            _mocapPanel.GetWindowRect(rcMocapPanel);
            ScreenToClient(rcMocapPanel);
            rcMocapPanel.right = rcMocapPanel.left + sz.cx;
            rcMocapPanel.bottom = rcMocapPanel.top + sz.cy;
            _mocapPanel.MoveWindow(rcMocapPanel);
            bHandled = TRUE;
            return 1;
        }
        else {
            bHandled = FALSE;
            return 1;
        }
    }

    void OnClose()
    {
        if (_mocapPanel.GetIsOpenFlag())
        {
            MCP_VERIFY(MocapApiService::Get()
                .application()
                .CloseApplication(_mocapPanel.GetAppHandleInstance()));
            MCP_VERIFY(MocapApiService::Get()
                .application()
                .DestroyApplication(_mocapPanel.GetAppHandleInstance()));
        }
        DestroyWindow();
        ::PostQuitMessage(0);
    }

    MocapPanel _mocapPanel;
    CCommandBarCtrl _cmdBar;
};