#pragma once

#include <atlwin.h>
#include <atlddx.h>
#include <atomic>
#include <atlmisc.h>
#include "resource.h"
#include <unordered_map>
#include <string_view>
#include <MocapApi.h>
#include <atlctrls.h>
#include <windowsnumerics.h>
#include <MocapApi.h>
#include <algorithm>
#include "mocap_api_service.h"
#include "settings_panel.hpp"
#include "command_panel.hpp"
#include "event_panel.hpp"
#include "bvh_motion_data_view.hpp"
#include "avatar_sensor_module_data_view.hpp"
#include "sensor_module_data_view.hpp"

namespace std {
    template<>
    struct hash<PreDefineItem>
    {
        size_t operator()(const PreDefineItem& preDefineItem) const noexcept 
        {
            return std::hash<std::string_view>()(std::string_view(reinterpret_cast<const char*>(&preDefineItem), sizeof(preDefineItem)));
        }
    };

    template<>
    struct equal_to<PreDefineItem> 
    {
        constexpr bool operator()(const PreDefineItem& l, const PreDefineItem& r) const 
        {
            return l.nCoordSystem == r.nCoordSystem 
                && l.nUpVector == r.nUpVector
                && l.nFrontVector == r.nFrontVector
                && l.nRotatingDirection == r.nRotatingDirection
                && l.nUnit == r.nUnit;
        }
    };
}

class MocapPanel
    : public CAxDialogImpl<MocapPanel>
{
    using SettingsPanel = SettingsPanelT<MocapPanel>;
    using CommandPanel = CommandPanelT<MocapPanel>;
    using EventPanel = EventPanelT<MocapPanel>;
    using BvhMotionDataView = BvhMotionDataViewT<MocapPanel>;
    using AvatarSensorModuleDataView = AvatarSensorModuleDataViewT<MocapPanel>;
    using SensorModuleDataView = SensorModuleDataViewT<MocapPanel>;
    friend class SettingsPanel;
    friend class CommandPanel;
    friend class EventPanel;
    friend class BvhMotionDataView;
    friend class AvatarSensorModuleDataView;
    friend class SensorModuleDataView;

    class AbstractView 
    {
    public:
        virtual void show() =0;
        virtual void hide() = 0;
        virtual void move(CRect ) = 0;

    protected:
        AbstractView() = default;

        ~AbstractView() = default;

        AbstractView(const AbstractView & other) = delete;
                
        AbstractView& operator= (const AbstractView & other) = delete;
                
        AbstractView(AbstractView && other) = delete;
                
        AbstractView& operator= (AbstractView && other) = delete;
    };
    
public:
    enum {
        IDD = IDD_MOCAP_CONSOLE
    };
    MocapPanel();

    BEGIN_MSG_MAP(MocapPanel)
        CHAIN_MSG_MAP_ALT(CAxDialogImpl<MocapPanel>, dwMsgMapID)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        NOTIFY_HANDLER(IDC_TAB_CONTROL, TCN_SELCHANGE, OnControlPanelChanged)
        NOTIFY_HANDLER(IDC_TAB_DATA_CONTENT, TCN_SELCHANGE, OnContentPanelChanged)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        COMMAND_ID_HANDLER(IDC_OPEN_AND_CLOSE_BUTTON, OnOpenOrClose)
    END_MSG_MAP()

    BEGIN_DDX_MAP(MocapPanel)
    END_DDX_MAP()

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    BOOL GetIsOpenFlag() { return _opened; }
    decltype(auto) GetAppHandleInstance() { return _appHandle; }

    HWND GetDataViewParent();
    
private:
    LRESULT OnOpenOrClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnControlPanelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
    LRESULT OnContentPanelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
    void resizeEventPanel();

    void pollEvents(std::vector<MocapApi::MCPEvent_t>* pvEvents);
    void pollMocapApiEvent();
    void updateMotionData(const MocapApi::MCPEvent_MotionData_t & motionData);
    void updateRigidBody(const MocapApi::MCPEvent_MotionData_t& motionData);
    void updateSensorModules(const MocapApi::MCPEvent_SensorModuleData_t & sensorModuleData);
    void updateTrackerData(const MocapApi::MCPEvent_TrackerData_t & trackerData);
    void updateCommandRespond(const MocapApi::MCPEvent_CommandRespond_t & commandRespond);
    void updateNotify(const MocapApi::MCPEvent_NotifyData_t & notifyData);
    void updateSystemError(const MocapApi::MCPEvent_SystemError_t & systemError);
    void switchCalcPanel();
    void switchBvhPanel();
    void queuedCommand(MocapApi::MCPCommandHandle_t command);
    void relayoutContentPanel();
    void relayoutControllerPanel();

    void updateRecordNotify(MocapApi::MCPRecordNotifyHandle_t handle);

    SettingsPanel _settingsPanel;
    CommandPanel _commandPanel;
    EventPanel _eventPanel;

    bool _opened = false;

    MocapApi::MCPApplicationHandle_t _appHandle;

    BvhMotionDataView _bvhMotionDataView;
    AvatarSensorModuleDataView _avatarSensorModuleDataView;
    SensorModuleDataView _sensorModuleDataView;

    std::vector<AbstractView*> _views;
    std::function<void(MocapApi::MCPAvatarHandle_t)> _handleAvatarData;

    //  [3/27/2023 Brian.Wang]
    UINT_PTR _timerIdPolling;

    void appendEvent(WTL::CString msg);

    static const struct CONTROLLER_PANEL_CONFIG{
        POINT SettingsGroupOffset;
        UINT VSpace;
        UINT LeftSpace;
        UINT RightSpace;
        UINT TopSpace;
        UINT BottomSpace;
    } kControllerPanelConfig;
};