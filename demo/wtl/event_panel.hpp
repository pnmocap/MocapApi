#pragma once
#include <atlgdi.h>

template<typename MocapPanel>
class EventPanelT 
    : public CAxDialogImpl<EventPanelT<MocapPanel>>
{
public:
    enum {
        IDD = IDD_DIALOG_EVENT_LOG
    };

    BEGIN_MSG_MAP(EventPanelT)
        CHAIN_MSG_MAP_ALT(CAxDialogImpl<EventPanelT<MocapPanel>>, dwMsgMapID)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
    END_MSG_MAP()

    void appendEventLog(WTL::CString msg)
    {
        CEdit winEvent(GetDlgItem(IDC_ERROR_EVENT_EDIT));
        winEvent.AppendText(msg);
        winEvent.AppendText(L"\r\n");
    }

private:
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
    {
        CRect rcClient;
        GetClientRect(rcClient);

        auto winGroup = GetDlgItem(IDC_ERROR_STATIC);
        CRect rcGroup;
        winGroup.GetWindowRect(rcGroup);
        ScreenToClient(rcGroup);
        auto wGroup = rcGroup.Width();
        rcGroup.left = (rcClient.Width() - wGroup) / 2;
        rcGroup.right = rcGroup.left + wGroup;
        rcGroup.bottom = rcClient.bottom - 5;
        winGroup.MoveWindow(rcGroup);
        
        
        auto winEdit = GetDlgItem(IDC_ERROR_EVENT_EDIT);
        CRect rcEdit;
        winEdit.GetWindowRect(rcEdit);
        ScreenToClient(rcEdit);
        rcEdit.left = rcGroup.left 
            + MocapPanel::kControllerPanelConfig.LeftSpace;
        rcEdit.right = rcGroup.right 
            - MocapPanel::kControllerPanelConfig.RightSpace;
        rcEdit.top = rcGroup.top +
            MocapPanel::kControllerPanelConfig.TopSpace;
        rcEdit.bottom = rcGroup.bottom -
            MocapPanel::kControllerPanelConfig.BottomSpace;
        winEdit.MoveWindow(rcEdit);



        bHandled = true;
        return 0;
    }
    
};