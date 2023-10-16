#pragma once

#include <atlddx.h>
#include <atlstr.h>
#include "w_a_conv.h"

template<typename MocapPanel>
class CommandPanelT 
    : public CAxDialogImpl<CommandPanelT<MocapPanel>>
    , public CWinDataExchange<CommandPanelT<MocapPanel>>
{
public:
    enum {
        IDD = IDD_DIALOG_COMMAND
    };
    explicit CommandPanelT(MocapPanel& mocapPanel) 
        : CAxDialogImpl<CommandPanelT<MocapPanel>>()
        , _mocapPanel(mocapPanel)
        , _items({
            {IDC_COMMAND_TAG_STATIC,IDC_COMMAND_TAG_COMBO},
            {IDC_AVATAR_NAME_RADIO,IDC_DEVICE_RADIO_RADIO},
            {IDC_STATIC_DEVICE_RADIO, IDC_EDIT_DEVICE_RADIO},
            {IDC_STATIC_CHARACTER_NAME, IDC_EDIT_CHARACTER_NAME},
            {IDC_STATIC_STOP_CAPTURE_EXTRA, IDC_COMBO_STOP_CAPTURE_EXTRA},
            {IDC_STATIC_RECORD_TAKE_NAME,IDC_EDIT_RECORD_TAKE_NAME} })
        , _deviceRadio(2471)
        , _enableDeviceRadio(TRUE)
        , _enableCharacterName(FALSE)
        , _pendingCommand(0) 
    {}

    BEGIN_MSG_MAP(CommandPanelT)
        CHAIN_MSG_MAP_ALT(CAxDialogImpl<CommandPanelT<MocapPanel>>, dwMsgMapID)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDC_SEND_BUTTON, OnSendCommand)
        COMMAND_HANDLER(IDC_COMMAND_TAG_COMBO, CBN_SELCHANGE, OnCommandTagComboSelChange)
        COMMAND_ID_HANDLER(IDC_AVATAR_NAME_RADIO, OnDeviceOrAvatar)
        COMMAND_ID_HANDLER(IDC_DEVICE_RADIO_RADIO, OnDeviceOrAvatar)
        MESSAGE_HANDLER(WM_SIZE, OnSize)

    END_MSG_MAP()

    BEGIN_DDX_MAP(CommandPanelT)
        DDX_UINT_RANGE(IDC_EDIT_DEVICE_RADIO, _deviceRadio, 2400u, 2500u)
        DDX_CHECK(IDC_AVATAR_NAME_RADIO, _enableCharacterName)
        DDX_CHECK(IDC_DEVICE_RADIO_RADIO, _enableDeviceRadio)
        DDX_TEXT_LEN(IDC_EDIT_CHARACTER_NAME, _characterName, sizeof(_characterName)/sizeof(_characterName[0]))
        DDX_TEXT_LEN(IDC_EDIT_RECORD_TAKE_NAME, _takeName, sizeof(_takeName) / sizeof(_takeName[0]))
    END_DDX_MAP()

    void handleCommandRespond(const MocapApi::MCPEvent_CommandRespond_t& commandRespond) 
    {
        VERIFY(commandRespond._commandHandle == _pendingCommand);
        switch (commandRespond._replay)
        {
        case MocapApi::MCPReplay_Response:
            handleResponseMessage();
            break;
        case MocapApi::MCPReplay_Running:
            handleRunningMessage();
            break;
        case MocapApi::MCPReplay_Result:
            handleResultMessage();
            break;
        default:
            // Brian.Wang todo [30/03/2023] 
            ATLASSERT(false);
            break;
        }
    }

private:
    MocapPanel& _mocapPanel;
    UINT _deviceRadio;
    BOOL _enableDeviceRadio;
    BOOL _enableCharacterName;
    wchar_t _characterName[128] = {0};
    wchar_t _takeName[256] = {0};
    MocapApi::MCPCommandHandle_t _pendingCommand;

private:

    void moveDlgItemOffset(CSize offset, UINT id, BOOL repaint)
    {
        auto win = GetDlgItem(id);
        CRect rc;
        win.GetWindowRect(rc);
        ScreenToClient(rc);
        rc.left += offset.cx;
        rc.top += offset.cy;
        rc.right += offset.cx;
        rc.bottom += offset.cy;
        win.MoveWindow(rc, repaint);
    }

    void setCommandDeviceRadioAndCharacterName(MocapApi::MCPCommandHandle_t cmdHandle)
    {
        if (_enableDeviceRadio) {
            DoDataExchange(TRUE, IDC_EDIT_DEVICE_RADIO);
            MCP_VERIFY(MocapApiService::Get()
                .commandInterface()
                .SetCommandExtraLong(MocapApi::CommandExtraLong_DeviceRadio,
                    _deviceRadio, cmdHandle));
        }
        else if (_enableCharacterName) {
            DoDataExchange(TRUE, IDC_EDIT_CHARACTER_NAME);
            if (_characterName[0] != L'\0') {
                MCP_VERIFY(MocapApiService::Get()
                    .commandInterface()
                    .SetCommandExtraLong(MocapApi::CommandExtraLong_AvatarName,
                        reinterpret_cast<intptr_t>(WToUtf8(_characterName).data()), cmdHandle));
            }
            else {
                MessageBox(L"Avatar can't be empty!");
            }
        }
    }

    void startCapture()
    {
        DoDataExchange(TRUE, IDC_EDIT_DEVICE_RADIO);

        MocapApi::MCPCommandHandle_t cmdHandle = 0;
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .CreateCommand(MocapApi::CommandStartCapture, &cmdHandle));
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .SetCommandExtraLong(MocapApi::CommandExtraLong_DeviceRadio, 
                _deviceRadio, cmdHandle));
        queuedCommand(cmdHandle);
    }

    void stopCapture()
    {
        MocapApi::MCPCommandHandle_t cmdHandle = 0;
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .CreateCommand(MocapApi::CommandStopCapture, &cmdHandle));
        setCommandDeviceRadioAndCharacterName(cmdHandle);
        static const std::array<MocapApi::EMCPCommandStopCatpureExtraFlag, 2> kStopCatpureExtraFlags = {
            MocapApi::StopCatpureExtraFlag_SensorsModulesHibernate,
            MocapApi::StopCatpureExtraFlag_SensorsModulesPowerOff,
        };
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .SetCommandExtraFlags(
                kStopCatpureExtraFlags[CComboBox(GetDlgItem(IDC_COMBO_STOP_CAPTURE_EXTRA)).GetCurSel()],
                cmdHandle));
        queuedCommand(cmdHandle);
    }

    void zeroPosition()
    {
        MocapApi::MCPCommandHandle_t cmdHandle = 0;
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .CreateCommand(MocapApi::CommandZeroPosition, &cmdHandle));
        setCommandDeviceRadioAndCharacterName(cmdHandle);
        queuedCommand(cmdHandle);
    }

    void calibrateMotion()
    {
        MocapApi::MCPCommandHandle_t cmdHandle = 0;
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .CreateCommand(MocapApi::CommandCalibrateMotion, &cmdHandle));
        setCommandDeviceRadioAndCharacterName(cmdHandle);
        queuedCommand(cmdHandle);
    }

    void startRecored()
    {
        MocapApi::MCPCommandHandle_t cmdHandle = 0;
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .CreateCommand(MocapApi::CommandStartRecored, &cmdHandle));
        DoDataExchange(TRUE, IDC_EDIT_RECORD_TAKE_NAME);
        if (_takeName[0] != L'\0') {
            MCP_VERIFY(MocapApiService::Get()
                .commandInterface()
                .SetCommandExtraLong(MocapApi::CommandExtraLong_Extra0,
                    reinterpret_cast<intptr_t>(WToUtf8(_takeName).data()), cmdHandle));
        }
        queuedCommand(cmdHandle);
    }

    void stopRecored()
    {
        MocapApi::MCPCommandHandle_t cmdHandle = 0;
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .CreateCommand(MocapApi::CommandStopRecored, &cmdHandle));
        setCommandDeviceRadioAndCharacterName(cmdHandle);
        queuedCommand(cmdHandle);
    }

    void resumeOriginalPosture()
    {
        MocapApi::MCPCommandHandle_t cmdHandle = 0;
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .CreateCommand(MocapApi::CommandResumeOriginalPosture, &cmdHandle));

        queuedCommand(cmdHandle);
    }

    void relayout() 
    {
        CRect rcCommandGroup;
        GetDlgItem(IDC_COMMAND_STATIC).GetWindowRect(rcCommandGroup);
        ScreenToClient(rcCommandGroup);

        CRect rcClient;
        GetClientRect(rcClient);
        const int kSettingsWidth = rcCommandGroup.Width();
        rcCommandGroup.left = (rcClient.Width() - kSettingsWidth) / 2;
        rcCommandGroup.right = rcCommandGroup.left + kSettingsWidth;
        rcCommandGroup.top = MocapPanel::kControllerPanelConfig.SettingsGroupOffset.y;

        auto offset = rcCommandGroup.top + MocapPanel::kControllerPanelConfig.TopSpace;
        std::for_each(_items.begin(), _items.end(), 
            [this, &offset](auto & item) {
                if (isItemVisible(item)) {
                    CRect rc0;
                    auto win0 = GetDlgItem(item._id0);
                    win0.GetWindowRect(rc0);
                    ScreenToClient(rc0);
                    CRect rc1;
                    auto win1 = GetDlgItem(item._id1);
                    win1.GetWindowRect(rc1);
                    ScreenToClient(rc1);
                    rc0.MoveToY(offset);
                    rc1.MoveToY(offset);
                    win0.MoveWindow(rc0);
                    win1.MoveWindow(rc1);
                    offset = rc1.bottom + MocapPanel::kControllerPanelConfig.VSpace;
                }
            });

        auto winSend = GetDlgItem(IDC_SEND_BUTTON);
        CRect rcSend;
        winSend.GetWindowRect(rcSend);
        ScreenToClient(rcSend);
        rcSend.MoveToY(offset);
        winSend.MoveWindow(rcSend, FALSE);
        offset = rcSend.bottom + MocapPanel::kControllerPanelConfig.VSpace;
        offset + MocapPanel::kControllerPanelConfig.BottomSpace;

        
        rcCommandGroup.bottom = offset;
        GetDlgItem(IDC_COMMAND_STATIC)
            .MoveWindow(rcCommandGroup, FALSE);

        CRect rcWin;
        GetWindowRect(rcWin);
        GetParent().ScreenToClient(rcWin);
        rcWin.bottom = rcWin.top + offset;
        MoveWindow(rcWin);
        _mocapPanel.resizeEventPanel();
    }

    struct LAYOUT_ITEM
    {
        UINT _id0;
        UINT _id1;
        UINT _h;
        UINT _w;
    };
    std::vector<LAYOUT_ITEM> _items;

    bool isItemVisible(const LAYOUT_ITEM& item)
    {
        return isVisible(item._id0) && isVisible(item._id1);
    }

    bool isVisible(UINT id)
    {
        return (GetDlgItem(id).GetWindowLong(GWL_STYLE) & WS_VISIBLE) == WS_VISIBLE;
    }

    void SetupStartCaptureUI()
    {
        GetDlgItem(IDC_AVATAR_NAME_RADIO).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_DEVICE_RADIO_RADIO).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_STOP_CAPTURE_EXTRA).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_STOP_CAPTURE_EXTRA).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_CHARACTER_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_CHARACTER_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_RECORD_TAKE_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_RECORD_TAKE_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_DEVICE_RADIO).ShowWindow(SW_SHOW);
        GetDlgItem(IDC_EDIT_DEVICE_RADIO).ShowWindow(SW_SHOW);
    }

    void SetupDeviceRadioAndCharacterNameUI()
    {
        GetDlgItem(IDC_AVATAR_NAME_RADIO).ShowWindow(SW_SHOW);
        GetDlgItem(IDC_DEVICE_RADIO_RADIO).ShowWindow(SW_SHOW);
        GetDlgItem(IDC_STATIC_STOP_CAPTURE_EXTRA).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_STOP_CAPTURE_EXTRA).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_RECORD_TAKE_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_RECORD_TAKE_NAME).ShowWindow(SW_HIDE);
        if (CButton(GetDlgItem(IDC_AVATAR_NAME_RADIO)).GetCheck()) {
            GetDlgItem(IDC_STATIC_CHARACTER_NAME).ShowWindow(SW_SHOW);
            GetDlgItem(IDC_EDIT_CHARACTER_NAME).ShowWindow(SW_SHOW);
        }
        if (CButton(GetDlgItem(IDC_DEVICE_RADIO_RADIO)).GetCheck()) {
            GetDlgItem(IDC_STATIC_DEVICE_RADIO).ShowWindow(SW_SHOW);
            GetDlgItem(IDC_EDIT_DEVICE_RADIO).ShowWindow(SW_SHOW);
        }
    }

    void SetupStopCaptureUI() 
    {
        SetupDeviceRadioAndCharacterNameUI();
        GetDlgItem(IDC_COMBO_STOP_CAPTURE_EXTRA).ShowWindow(SW_SHOW);
        GetDlgItem(IDC_STATIC_STOP_CAPTURE_EXTRA).ShowWindow(SW_SHOW);
    }

    void SetupStartRecoredUI()
    {
        GetDlgItem(IDC_AVATAR_NAME_RADIO).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_DEVICE_RADIO_RADIO).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_STOP_CAPTURE_EXTRA).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_STOP_CAPTURE_EXTRA).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_CHARACTER_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_CHARACTER_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_RECORD_TAKE_NAME).ShowWindow(SW_SHOW);
        GetDlgItem(IDC_EDIT_RECORD_TAKE_NAME).ShowWindow(SW_SHOW);
        GetDlgItem(IDC_STATIC_DEVICE_RADIO).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_DEVICE_RADIO).ShowWindow(SW_HIDE);
    }

    void SetupStopRecoredUI()
    {
        GetDlgItem(IDC_AVATAR_NAME_RADIO).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_DEVICE_RADIO_RADIO).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_STOP_CAPTURE_EXTRA).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_STOP_CAPTURE_EXTRA).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_CHARACTER_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_CHARACTER_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_RECORD_TAKE_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_RECORD_TAKE_NAME).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_DEVICE_RADIO).ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_DEVICE_RADIO).ShowWindow(SW_HIDE);
    }

    using CommandHandleProc = void(CommandPanelT::*)();

    void queuedCommand(MocapApi::MCPCommandHandle_t command) 
    {
        _mocapPanel.queuedCommand(command);
        _pendingCommand = command;
        EnableWindow(false);
    }

    void handleResponseMessage() 
    {
        _mocapPanel.appendEvent(L"Mocap-Server received command, will be running.");
    }

    void handleCalibrateMotionProgress(MocapApi::MCPCalibrateMotionProgressHandle_t handle_)
    {
        auto& caliMoProgress = MocapApiService::Get().calibrateMotionSettings();
        std::string pose;
        pose.resize(256);
        uint32_t step = 0, size = pose.size();
        MCP_VERIFY(caliMoProgress.GetCalibrateMotionProgressStepOfCurrentPose(
            &step, pose.data(), &size, handle_));
        pose.resize(size);
        std::wostringstream ostrstrm;
        switch (step)
        {
        case MocapApi::CalibrateMotionProgressStep_Prepare:
        {
            ostrstrm << L"Prepare " << AToW(pose.data()) << "calibration!";
        }   break;
        case MocapApi::CalibrateMotionProgressStep_Countdown:
        {
            uint32_t countdown = 0;
            caliMoProgress.GetCalibrateMotionProgressCountdownOfPose(&countdown, pose.data(), handle_);
            ostrstrm << L"Calibrate " << AToW(pose.data()) << " Countdown " << countdown << "!";
        }  break;
        case MocapApi::CalibrateMotionProgressStep_Progress:
        {
            uint32_t progress = 0;
            caliMoProgress.GetCalibrateMotionProgressProgressOfPose(&progress, pose.data(), handle_);
            ostrstrm << L"Calibrate " << AToW(pose.data()) << " progress " << progress << "!";
        }   break;
        default:
            // Brian.Wang todo [30/03/2023] 
            ATLASSERT(false);
            break;
        }
        _mocapPanel.appendEvent(ostrstrm.str().data());
    }

    void handleRunningMessage() 
    {
        MocapApi::MCPCalibrateMotionProgressHandle_t calibrateMotionProgressHandle = 0;
        if (MocapApi::Error_None == MocapApiService::Get()
            .commandInterface()
            .GetCommandProgress(MocapApi::EMCPCommandProgress::CommandProgress_CalibrateMotion
                , reinterpret_cast<intptr_t>(&calibrateMotionProgressHandle)
                , _pendingCommand)) {
            handleCalibrateMotionProgress(calibrateMotionProgressHandle);
        }
        else {
            _mocapPanel.appendEvent(L"Unsupported running message!");
        }
    }

    void handleResultMessage() 
    {
        uint32_t res = 0;
        MCP_VERIFY(MocapApiService::Get().commandInterface()
            .GetCommandResultCode(&res, _pendingCommand));
        const char* msg = nullptr;
        MCP_VERIFY(MocapApiService::Get().commandInterface()
            .GetCommandResultMessage(&msg, _pendingCommand));
        std::wostringstream ostrstrm;
        ostrstrm << L"Received result message : " << AToW(msg) << L";" << L"result code : " << res;
        _mocapPanel.appendEvent(ostrstrm.str().data());
        MCP_VERIFY(MocapApiService::Get()
            .commandInterface()
            .DestroyCommand(_pendingCommand));
        _pendingCommand = 0;
        EnableWindow(TRUE);
    }

private:
    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
    {
        std::for_each(_items.begin(), _items.end(), 
            [this](auto & item) {
                CRect rc0, rc1;
                GetDlgItem(item._id0).GetWindowRect(rc0);
                GetDlgItem(item._id1).GetWindowRect(rc1);
                ScreenToClient(rc0);
                ScreenToClient(rc1);
                CRect rc;
                rc.UnionRect(rc0, rc1);
                item._h = rc.Height();
                item._w = rc.Width();
            });

        {
            CComboBox(GetDlgItem(IDC_COMMAND_TAG_COMBO)).SetCurSel(0);
            BOOL bHandled = FALSE;
            OnCommandTagComboSelChange(0, 0, NULL, bHandled);
        }

        CButton(GetDlgItem(IDC_DEVICE_RADIO_RADIO)).SetCheck(BST_CHECKED);
        CComboBox(GetDlgItem(IDC_STOP_CAPTURE_EXTRA_COMBO)).SetCurSel(0);

        DoDataExchange(FALSE);
        bHandled = true;
        return 0;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
    {

        bHandled = true;
        return 0;
    }
    
    LRESULT OnSendCommand(WORD code, WORD id, HWND hWnd, BOOL& bHandled) 
    {
        static const std::array<CommandHandleProc, 7> kQueuedCommandProcs = {
            & CommandPanelT::startCapture,
            & CommandPanelT::stopCapture,
            & CommandPanelT::zeroPosition,
            & CommandPanelT::calibrateMotion,
            & CommandPanelT::startRecored,
            & CommandPanelT::stopRecored,
            & CommandPanelT::resumeOriginalPosture,
        };
        if (_mocapPanel._opened) {
            (this->*kQueuedCommandProcs[CComboBox(GetDlgItem(IDC_COMMAND_TAG_COMBO)).GetCurSel()])();
        }
        else {
            MessageBox(L"MocapApi Application is not open!");
        }
        bHandled = TRUE;
        return 0;
    }

    LRESULT OnCommandTagComboSelChange(WORD /*code*/, WORD /*id*/, HWND /*hWnd*/, BOOL& bHandled)
    {
        using CommandHandleProc = void(CommandPanelT::*)();
        static const std::array<CommandHandleProc, 7> kQueuedCommandProcs = {
            & CommandPanelT::SetupStartCaptureUI,
            & CommandPanelT::SetupStopCaptureUI,
            & CommandPanelT::SetupDeviceRadioAndCharacterNameUI,
            & CommandPanelT::SetupDeviceRadioAndCharacterNameUI,
            & CommandPanelT::SetupStartRecoredUI,
            & CommandPanelT::SetupStopRecoredUI,
            & CommandPanelT::SetupDeviceRadioAndCharacterNameUI
        };
        (this->*kQueuedCommandProcs[CComboBox(GetDlgItem(IDC_COMMAND_TAG_COMBO)).GetCurSel()])();
        relayout();
        bHandled = TRUE;
        return 1;
    }

    LRESULT OnDeviceOrAvatar(WORD code, WORD id, HWND hWnd, BOOL& bHandled)
    {
        DoDataExchange(TRUE, IDC_DEVICE_RADIO_RADIO);
        if (_enableDeviceRadio) {
            GetDlgItem(IDC_EDIT_DEVICE_RADIO).ShowWindow(SW_SHOW);
            GetDlgItem(IDC_STATIC_DEVICE_RADIO).ShowWindow(SW_SHOW);
            
            GetDlgItem(IDC_EDIT_CHARACTER_NAME).ShowWindow(SW_HIDE);
            GetDlgItem(IDC_STATIC_CHARACTER_NAME).ShowWindow(SW_HIDE);
        }
        else {
            DoDataExchange(TRUE, IDC_AVATAR_NAME_RADIO);
            if (_enableCharacterName) {
                GetDlgItem(IDC_EDIT_DEVICE_RADIO).ShowWindow(SW_HIDE);
                GetDlgItem(IDC_STATIC_DEVICE_RADIO).ShowWindow(SW_HIDE);

                GetDlgItem(IDC_EDIT_CHARACTER_NAME).ShowWindow(SW_SHOW);
                GetDlgItem(IDC_STATIC_CHARACTER_NAME).ShowWindow(SW_SHOW);
            }
            else {
                // error [3/30/2023 Brian.Wang]
                ATLASSERT(false);
            }
        }
        relayout();
        bHandled = TRUE;
        return 0;
    }
};