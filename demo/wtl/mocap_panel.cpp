#include "stdafx.h"
#include "mocap_panel.h"
#include <atlctrls.h>
#include <array>
#include <algorithm>
#include <MocapApi.h>
#include <numeric>
#include "mocap_api_service.h"
#include <system_error>
#include "MocapApi.h"
enum {
    kPollingTimer = 100
};

const MocapPanel::CONTROLLER_PANEL_CONFIG MocapPanel::kControllerPanelConfig = {
    {10, 15},
    4,
    4,
    8,
    20,
    8,
};

MocapPanel::MocapPanel()
    : CAxDialogImpl<MocapPanel>()
    , _opened(false)
    , _timerIdPolling(0)
    , _settingsPanel(*this)
    , _commandPanel(*this)
    , _bvhMotionDataView(*this)
    , _avatarSensorModuleDataView(*this)
    , _sensorModuleDataView(*this)
    , _views({& _bvhMotionDataView, &_avatarSensorModuleDataView, &_sensorModuleDataView})
{}

LRESULT MocapPanel::OnContentPanelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled) 
{
    auto curSel = CTabCtrl(GetDlgItem(IDC_TAB_DATA_CONTENT)).GetCurSel();
    int index = 0;
    while(index < curSel){
        _views[index++]->hide();
    }
    _views[index++]->show();
    while (index < _views.size()) {
        _views[index++]->hide();
    }
    bHandled = TRUE;
    return 0;
}

LRESULT MocapPanel::OnControlPanelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)
{
    switch (CTabCtrl(GetDlgItem(IDC_TAB_CONTROL)).GetCurSel()) 
    {
    case 0:
        _settingsPanel.ShowWindow(SW_SHOW);
        _commandPanel.ShowWindow(SW_HIDE);
        break;
    case 1:
        _settingsPanel.ShowWindow(SW_HIDE);
        _commandPanel.ShowWindow(SW_SHOW);
        break;
    default:
        ATLASSERT(false);
        break;
    }
    resizeEventPanel();
    bHandled = TRUE;
    return 0;
}

LRESULT MocapPanel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CTabCtrl tabCtrlDataContent(GetDlgItem(IDC_TAB_DATA_CONTENT));
    tabCtrlDataContent.AddItem(L"Motion Data");
    tabCtrlDataContent.AddItem(L"Calculation Data");
    tabCtrlDataContent.AddItem(L"Sensor Module Data");

    // init listControl
    _bvhMotionDataView.init();
    _avatarSensorModuleDataView.init();
    _sensorModuleDataView.init();

    CTabCtrl tabControl(GetDlgItem(IDC_TAB_CONTROL));
    tabControl.AddItem(L"Settings");
    _eventPanel.Create(tabControl);
    _settingsPanel.Create(tabControl);
    CRect rcSettingsPanel;
    _settingsPanel.GetWindowRect(rcSettingsPanel);
    tabControl.ScreenToClient(rcSettingsPanel);
    if (nullptr != MocapApiService::Get().getCommandInterface()) {
        tabControl.AddItem(L"Command");
        _commandPanel.Create(tabControl);
        CRect rcCommandPanel;
        _commandPanel.GetWindowRect(rcCommandPanel);
        tabControl.ScreenToClient(rcCommandPanel);

        static const int kCurrentTag = 0;
        switch (kCurrentTag){
            case 0:
                tabControl.SetCurSel(0);
                _settingsPanel.ShowWindow(SW_SHOW);
                break;
            case 1:
                tabControl.SetCurSel(1);
                _commandPanel.ShowWindow(SW_SHOW);
                break;
            default:
                // Brian.Wang todo [29/03/2023] 
                ATLASSERT(false);
                break;
        }
    }
    else {
        tabControl.SetCurSel(0);
        _settingsPanel.ShowWindow(SW_SHOW);
    }
    bHandled = true;
    return 1;
}

HWND MocapPanel::GetDataViewParent() 
{
    return GetDlgItem(IDC_TAB_DATA_CONTENT);
}

void MocapPanel::resizeEventPanel() 
{
    CRect rcEvent;
    CTabCtrl tabControl(GetDlgItem(IDC_TAB_CONTROL));
    CRect rcClientTabControl;
    tabControl.GetClientRect(rcClientTabControl);
    switch (tabControl.GetCurSel()) {
    case 0: 
    {
        CRect rcSettingsPanel;
        _settingsPanel.GetWindowRect(rcSettingsPanel);
        tabControl.ScreenToClient(rcSettingsPanel);
        rcEvent = CRect(rcClientTabControl.left,
            rcSettingsPanel.bottom,
            rcClientTabControl.right,
            rcClientTabControl.bottom);
    }   break;
    case 1:
    {
        CRect rcCommandPanel;
        _commandPanel.GetWindowRect(rcCommandPanel);
        tabControl.ScreenToClient(rcCommandPanel);
        rcEvent = CRect(rcClientTabControl.left,
            rcCommandPanel.bottom,
            rcClientTabControl.right,
            rcClientTabControl.bottom);
    }   break;
    default:
        // Brian.Wang todo [04/04/2023] 
        ATLASSERT(false);
        break;
    }
    _eventPanel.MoveWindow(rcEvent);
}

void MocapPanel::pollEvents(std::vector<MocapApi::MCPEvent_t>* pvEvents)
{
    uint32_t sizeOfEvents = 0;
    MocapApi::EMCPError err;
    auto& app = MocapApiService::Get()
        .application();
    do {
        sizeOfEvents = 0;
        MCP_VERIFY(app.PollApplicationNextEvent(nullptr,
            &sizeOfEvents, _appHandle));
        pvEvents->clear();
        pvEvents->resize(sizeOfEvents);
        std::for_each(pvEvents->begin(), pvEvents->end(),
            [](MocapApi::MCPEvent_t& ev) {
                ev.size = sizeof(MocapApi::MCPEvent_t);
            });
        err = app.PollApplicationNextEvent(pvEvents->data(), &sizeOfEvents, _appHandle);
    } while (err == MocapApi::EMCPError::Error_MoreEvent);
    MCP_VERIFY(err);
    pvEvents->resize(sizeOfEvents);
}

void MocapPanel::switchCalcPanel() 
{
    _handleAvatarData = [this](MocapApi::MCPAvatarHandle_t avatarHandle) {
        _avatarSensorModuleDataView.update(avatarHandle);
    };
}

void MocapPanel::switchBvhPanel() 
{
    _handleAvatarData = [this](MocapApi::MCPAvatarHandle_t avatarHandle) {
        _bvhMotionDataView.update(avatarHandle);
    };
}

void MocapPanel::relayoutControllerPanel() 
{
    static const int kTabControllerBottomSpace = 2;
    CRect rcClient;
    GetClientRect(rcClient);

    CTabCtrl tabControl(GetDlgItem(IDC_TAB_CONTROL));
    CRect rcTabControl;
    tabControl.GetWindowRect(rcTabControl);
    ScreenToClient(rcTabControl);
    rcTabControl.bottom = rcClient.bottom - kTabControllerBottomSpace;
    tabControl.MoveWindow(rcTabControl);

    // [3/29/2023 Brian.Wang]
    static const int kWinInTabControllerLeftAndRightSpace = 2;
    static const int kWinInTabControllerTopSpace = 2;
    CRect rcClientTabControl;
    tabControl.GetClientRect(rcClientTabControl);
    CRect rcItem;
    tabControl.GetItemRect(0, rcItem);
    rcItem.bottom;
    CRect rcSettingsPanel;
    _settingsPanel.GetWindowRect(rcSettingsPanel);
    tabControl.ScreenToClient(rcSettingsPanel);
    const int kWidthInTabController = rcClientTabControl.Width() 
        - kWinInTabControllerLeftAndRightSpace 
        - kWinInTabControllerLeftAndRightSpace;
    int hSettingsPanel = rcSettingsPanel.Height();
    
    rcSettingsPanel.top = rcItem.bottom + kWinInTabControllerTopSpace;
    rcSettingsPanel.bottom = rcSettingsPanel.top + hSettingsPanel;
    rcSettingsPanel.left = rcClientTabControl.left + kWinInTabControllerLeftAndRightSpace;
    rcSettingsPanel.right = rcSettingsPanel.left + kWidthInTabController;
    _settingsPanel.MoveWindow(rcSettingsPanel);

    if (NULL != _commandPanel.m_hWnd) {
        CRect rcCommandPanel;
        _commandPanel.GetWindowRect(rcCommandPanel);
        tabControl.ScreenToClient(rcCommandPanel);
        int hCommandPanel = rcCommandPanel.Height();
        rcCommandPanel.top = rcItem.bottom + kWinInTabControllerTopSpace;
        rcCommandPanel.bottom = rcCommandPanel.top + hCommandPanel;
        rcCommandPanel.left = rcClientTabControl.left + kWinInTabControllerLeftAndRightSpace;
        rcCommandPanel.right = rcCommandPanel.left + kWidthInTabController;
        _commandPanel.MoveWindow(rcCommandPanel);
        CRect rcEvent;
        switch (tabControl.GetCurSel()) {
        case 0:
        {
            rcEvent = CRect(rcClientTabControl.left,
                rcSettingsPanel.bottom,
                rcClientTabControl.right,
                rcClientTabControl.bottom);
        }   break;
        case 1:
        {
            rcEvent = CRect(rcClientTabControl.left,
                rcCommandPanel.bottom,
                rcClientTabControl.right,
                rcClientTabControl.bottom);
        }   break;
        default:
            // Brian.Wang todo [29/03/2023] 
            ATLASSERT(false);
            break;
        }
        _eventPanel.MoveWindow(rcEvent);
    }
    else {
        CRect rcEvent(rcClientTabControl.left,
                rcSettingsPanel.bottom,
                rcClientTabControl.right,
                rcClientTabControl.bottom);
        _eventPanel.MoveWindow(rcEvent);
    }
}

void MocapPanel::relayoutContentPanel() 
{
    static const int kTabDataContentRightSpace = 2;
    static const int kTabDataContentBottomSpace = 2;
    static const int kWinInTabDataContentLeftSpace = 2;
    static const int kWinInTabDataContentTopSpace = 2;
    static const int kWinInTabDataContentRightSpace = 2;
    static const int kWinInTabDataContentBottomSpace = 2;

    CRect  rcTabDataContent;
    CTabCtrl winTabDataContent(GetDlgItem(IDC_TAB_DATA_CONTENT));
    winTabDataContent.GetWindowRect(rcTabDataContent);
    ScreenToClient(rcTabDataContent);
    CRect rcClient;
    GetClientRect(rcClient);
    rcTabDataContent.right = rcClient.right - kTabDataContentRightSpace;
    rcTabDataContent.bottom = rcClient.bottom - kTabDataContentBottomSpace;
    winTabDataContent.MoveWindow(rcTabDataContent);

    CRect rcItem;
    winTabDataContent.GetItemRect(0, rcItem);
    CRect rcDataContentClient;
    winTabDataContent.GetClientRect(rcDataContentClient);
    CRect rcWinInTabDataContent;
    rcWinInTabDataContent.left = rcDataContentClient.left + kWinInTabDataContentLeftSpace;
    rcWinInTabDataContent.top = rcDataContentClient.top + kWinInTabDataContentTopSpace;
    rcWinInTabDataContent.right = rcItem.left - kWinInTabDataContentRightSpace;
    rcWinInTabDataContent.bottom = rcDataContentClient.bottom - kWinInTabDataContentBottomSpace;

    std::for_each(_views.begin(), _views.end(), 
        [&rcWinInTabDataContent](AbstractView* view) {
        view->move(rcWinInTabDataContent);
        });
}

LRESULT MocapPanel::OnOpenOrClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
    _settingsPanel.EnableWindow(!_opened);
    if (!_opened){
        MCP_VERIFY(MocapApiService::Get()
            .application()
            .CreateApplication(&_appHandle));
        _settingsPanel.initSettings(_appHandle);
        _settingsPanel.initRenderSettings(_appHandle);
        MCP_VERIFY(MocapApiService::Get()
            .application()
            .OpenApplication(_appHandle));
        _opened = true;
        _timerIdPolling = SetTimer(10, kPollingTimer);
    }
    else
    {
        MCP_VERIFY(MocapApiService::Get()
            .application()
            .CloseApplication(_appHandle));
        MCP_VERIFY(MocapApiService::Get()
            .application()
            .DestroyApplication(_appHandle));
        _appHandle = 0;
        GetDlgItem(IDC_OPEN_AND_CLOSE_BUTTON)
            .SetWindowTextW(L"Open");
        _opened = false;
        KillTimer(_timerIdPolling);
    }
    bHandled = true;
    return 0;
}

LRESULT MocapPanel::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (wParam == _timerIdPolling) {
        pollMocapApiEvent();
    }
    bHandled = true;
    return 0;
}

LRESULT MocapPanel::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{
    relayoutControllerPanel();
    relayoutContentPanel();
    bHandled = TRUE;
    return 0;
}

void MocapPanel::pollMocapApiEvent()
{
    std::vector<MocapApi::MCPEvent_t> vEvents;
    pollEvents(&vEvents);
    std::for_each(vEvents.begin(), vEvents.end(),
        [this](const auto& e) {
            switch (e.eventType) {
            case MocapApi::MCPEvent_AvatarUpdated:
                updateMotionData(e.eventData.motionData);
                break;
            case MocapApi::MCPEvent_RigidBodyUpdated:
                updateRigidBody(e.eventData.motionData);
                break;
            case MocapApi::MCPEvent_Error:
                updateSystemError(e.eventData.systemError);
                break;
            case MocapApi::MCPEvent_SensorModulesUpdated:
                updateSensorModules(e.eventData.sensorModuleData);
                break;
            case MocapApi::MCPEvent_TrackerUpdated:
                updateTrackerData(e.eventData.trackerData);
                break;
            case MocapApi::MCPEvent_CommandReply:
                updateCommandRespond(e.eventData.commandRespond);
                break;
            case MocapApi::MCPEvent_Notify:
                updateNotify(e.eventData.notifyData);
                break;
            default:
                // Brian.Wang todo [27/03/2023] 
                ATLASSERT(false);
                break;
            }
        });
}

void MocapPanel::updateMotionData(const MocapApi::MCPEvent_MotionData_t& motionData)
{
    _handleAvatarData(motionData.avatarHandle);
}

void MocapPanel::updateRigidBody(const MocapApi::MCPEvent_MotionData_t& motionData)
{
    // Brian.Wang todo [27/03/2023] 
    ATLASSERT(false);
}

void MocapPanel::updateSensorModules(const MocapApi::MCPEvent_SensorModuleData_t& sensorModuleData)
{
    _sensorModuleDataView.update(
        sensorModuleData._sensorModuleHandle);
}

void MocapPanel::updateTrackerData(const MocapApi::MCPEvent_TrackerData_t& trackerData)
{
#if 0
    // Brian.Wang todo [27/03/2023] 
    ATLASSERT(false);
#endif
}

void MocapPanel::updateCommandRespond(const MocapApi::MCPEvent_CommandRespond_t & commandRespond)
{
    _commandPanel.handleCommandRespond(commandRespond);
}
void MocapPanel::updateNotify(const MocapApi::MCPEvent_NotifyData_t& notifyData)
{
    switch (notifyData._notify) {
    case MocapApi::Notify_RecordStarted:
        appendEvent(L"Recv Record Started Notify:");
        updateRecordNotify(notifyData._notifyHandle);
        break;
    case MocapApi::Notify_RecordStoped:
        appendEvent(L"Recv Record Stoped Notify:");
        updateRecordNotify(notifyData._notifyHandle);
        break;
    case MocapApi::Notify_RecordFinished:
        appendEvent(L"Recv Record Finished Notify:");
        updateRecordNotify(notifyData._notifyHandle);
        break;
    default:
        // Brian.Wang todo [27/03/2023] 
        ATLASSERT(false);
        break;
    }
}

void MocapPanel::updateSystemError(const MocapApi::MCPEvent_SystemError_t& systemError)
{
    switch (systemError.error) {
    case MocapApi::Error_ServerNotReady:
        appendEvent(L"Axis Server is not ready!\r\n");
        break;
    case MocapApi::Error_MoreEvent:
    case MocapApi::Error_InsufficientBuffer:
    case MocapApi::Error_InvalidObject:
    case MocapApi::Error_InvalidHandle:
    case MocapApi::Error_InvalidParameter:
    case MocapApi::Error_NotSupported:
    case MocapApi::Error_IgnoreUDPSetting:
    case MocapApi::Error_IgnoreTCPSetting:
    case MocapApi::Error_IgnoreBvhSetting:
    case MocapApi::Error_JointNotFound:
    case MocapApi::Error_WithoutTransformation:
    case MocapApi::Error_NoneMessage:
    case MocapApi::Error_NoneParent:
    case MocapApi::Error_NoneChild:
    case MocapApi::Error_AddressInUse:
    case MocapApi::Error_ClientNotReady:
    case MocapApi::Error_IncompleteCommand:
    case MocapApi::Error_UDP:
    case MocapApi::Error_TCP:
    case MocapApi::Error_QueuedCommandFaild:
    default:
        // Brian.Wang todo [28/03/2023] 
        ATLASSERT(false);
        break;
    }
}

void MocapPanel::appendEvent(WTL::CString msg)
{
    _eventPanel.appendEventLog(msg);
}

void MocapPanel::queuedCommand(MocapApi::MCPCommandHandle_t cmdHandle)
{
    MCP_VERIFY(MocapApiService::Get()
        .application()
        .QueuedServerCommand(cmdHandle, _appHandle));
}

void MocapPanel::updateRecordNotify(MocapApi::MCPRecordNotifyHandle_t handle) 
{
    auto interface_ = MocapApiService::Get().getRecordNotifyInterface();
    if (nullptr == interface_) {
        return;
    }
    const char* takeName = nullptr;
    if (MocapApi::Error_None == interface_->RecordNotifyGetTakeName(
        &takeName, handle)) {
        std::wostringstream ostrstrm;
        ostrstrm << L"\t Take Name : " << Utf8ToW(takeName);
        appendEvent(ostrstrm.str().data());
    }
    const char* takeFileSuffix = nullptr;
    if (MocapApi::Error_None == interface_->RecordNotifyGetTakeFileSuffix(
        &takeFileSuffix, handle)) {
        std::wostringstream ostrstrm;
        ostrstrm << L"\t Take File Suffix : " << Utf8ToW(takeFileSuffix);
        appendEvent(ostrstrm.str().data());
    }
    const char* takePath = nullptr;
    if (MocapApi::Error_None == interface_->RecordNotifyGetTakePath(
        &takePath, handle)) {
        std::wostringstream ostrstrm;
        ostrstrm << L"\t Take Path : " << Utf8ToW(takePath);
        appendEvent(ostrstrm.str().data());
    }
    const char* takeSaveDir = nullptr;
    if (MocapApi::Error_None == interface_->RecordNotifyGetTakeSaveDir(
        &takeSaveDir, handle)) {
        std::wostringstream ostrstrm;
        ostrstrm << L"\t Take Save Dir : " << Utf8ToW(takeSaveDir);
        appendEvent(ostrstrm.str().data());
    }
}

