#pragma once

#include <boost/asio.hpp>
#include "debug.h"

struct PreDefineItem {
    int nCoordSystem;
    int nUpVector;
    int nFrontVector;
    int nRotatingDirection;
    int nUnit;
};

template<typename MocapPanel>
class SettingsPanelT
    : public CAxDialogImpl<SettingsPanelT<MocapPanel>>
{
public:
    enum {
        IDD = IDD_DIALOG_SETTINGS
    };
    explicit SettingsPanelT(MocapPanel& mocapPanel)
        : CAxDialogImpl<SettingsPanelT<MocapPanel>>()
        , _mocapPanel(mocapPanel)
        , _settingsLayoutItems{
            {IDC_DATA_TYPE_STATIC, IDC_DATA_TYPE_COMBO},
            {IDC_BVH_ROTATION_STATIC, IDC_BVH_ROTATION_COMBO},
            {IDC_BVH_TRANSFORMATION_STATIC, IDC_BVH_TRANSFORMATION_COMBO},
            {IDC_PROTOCOL_STATIC, IDC_PROTOCOL_COMBO},
            {IDC_TCP_SERVER_IP_STATIC, IDC_TCP_SERVER_IPADDRESS},
            {IDC_TCP_SERVER_PORT_STATIC, IDC_TCP_SERVER_PORT_EDIT},
            {IDC_UDP_LOCAL_IP_STATIC, IDC_UDP_LOCAL_IPADDRESS},
            {IDC_UDP_LOCAL_PORT_STATIC, IDC_UDP_LOCAL_PORT_EDIT},
            {IDC_UDP_SERVER_IP_STATIC, IDC_UDP_SERVER_IPADDRESS},
            {IDC_UDP_SERVER_PORT_STATIC, IDC_UDP_SERVER_PORT_EDIT},
        }
        , _renderSettingsLayoutItems{
            {IDC_PRE_DEFINED_STATIC, IDC_PRE_DEFINED_COMBO},
            {IDC_COORD_SYSTEM_STATIC, IDC_COORD_SYSTEM_COMBO},
            {IDC_UP_VECTOR_STATIC, IDC_UP_VECTOR_COMBO},
            {IDC_FRONT_VECTOR_STATIC, IDC_FRONT_VECTOR_COMBO},
            {IDC_ROTATING_DIRECTION_STATIC, IDC_ROTATING_DIRECTION_COMBO},
            {IDC_UNIT_STATIC, IDC_UNIT_COMBO},
        }
        , _disableHandleResize(true)
    {}

    BEGIN_MSG_MAP(SettingsPanelT)
        CHAIN_MSG_MAP_ALT(CAxDialogImpl<SettingsPanelT<MocapPanel>>, dwMsgMapID)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
//        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
        COMMAND_HANDLER(IDC_DATA_TYPE_COMBO, CBN_SELCHANGE, OnDataTypeComboSelChange)
        COMMAND_HANDLER(IDC_PRE_DEFINED_COMBO, CBN_SELCHANGE, OnPreDefinedComboSelChange)
        COMMAND_HANDLER(IDC_COORD_SYSTEM_COMBO, CBN_SELCHANGE, OnCoordSystemComboSelChange)
        COMMAND_HANDLER(IDC_UP_VECTOR_COMBO, CBN_SELCHANGE, OnUpVectorComboSelChange)
        COMMAND_HANDLER(IDC_FRONT_VECTOR_COMBO, CBN_SELCHANGE, OnFrontVectorComboSelChange)
        COMMAND_HANDLER(IDC_ROTATING_DIRECTION_COMBO, CBN_SELCHANGE, OnRotatingDirectionComboSelChange)
        COMMAND_HANDLER(IDC_UNIT_COMBO, CBN_SELCHANGE, OnUnitComboSelChange)
        COMMAND_HANDLER(IDC_PROTOCOL_COMBO, CBN_SELCHANGE, OnProtocolComboSelChange)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_MOVE, OnMove)
    END_MSG_MAP()

    BEGIN_DDX_MAP(SettingsPanelT)
    END_DDX_MAP()

    void initRenderSettings(MocapApi::MCPApplicationHandle_t applicationHandle) 
    {
        MocapApi::MCPRenderSettingsHandle_t mcpRenderSettingHandle = 0;
        MCP_VERIFY(MocapApiService::Get()
            .renderSettings()
            .CreateRenderSettings(&mcpRenderSettingHandle));    // Brian.Wang todo [06/03/2023] 
        switch (_coordSystem) //coordSystem config
        {
        case ECoordSystem::RightHanded:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetCoordSystem(MocapApi::CoordSystem_RightHanded, mcpRenderSettingHandle));
            break;
        case ECoordSystem::LeftHanded:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetCoordSystem(MocapApi::CoordSystem_LeftHanded, mcpRenderSettingHandle));
            break;
        default:
            ATLASSERT(false);
            break;
        }

        switch (_upVector)
        {
        case EUpVector::XAxis:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetUpVector(MocapApi::UpVector_XAxis, 1, mcpRenderSettingHandle));
            break;
        case EUpVector::YAxis:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetUpVector(MocapApi::UpVector_YAxis, 1, mcpRenderSettingHandle));
            break;
        case EUpVector::ZAxis:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetUpVector(MocapApi::UpVector_ZAxis, 1, mcpRenderSettingHandle));
            break;
        case EUpVector::minusXAxis:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetUpVector(MocapApi::UpVector_XAxis, -1, mcpRenderSettingHandle));
            break;
        case EUpVector::minusYAxis:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetUpVector(MocapApi::UpVector_YAxis, -1, mcpRenderSettingHandle));
            break;
        case EUpVector::minusZAxis:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetUpVector(MocapApi::UpVector_ZAxis, -1, mcpRenderSettingHandle));
            break;
        default:
            ATLASSERT(false);
            break;
        }

        switch (_frontVector)
        {
        case EFrontVector::ParityEven:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetFrontVector(MocapApi::FrontVector_ParityEven, 1, mcpRenderSettingHandle));
            break;
        case EFrontVector::ParityOdd:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetFrontVector(MocapApi::FrontVector_ParityOdd, 1, mcpRenderSettingHandle));
            break;
        case EFrontVector::minusParityOdd:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetFrontVector(MocapApi::FrontVector_ParityOdd, -1, mcpRenderSettingHandle));
            break;
        case EFrontVector::minusParityEven:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetFrontVector(MocapApi::FrontVector_ParityEven, -1, mcpRenderSettingHandle));
            break;
        default:
            ATLASSERT(false);
            break;
        }

        switch (_rotatingDirection)
        {
        case ERotatingDirection::Clockwise:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetRotatingDirection(MocapApi::RotatingDirection_Clockwise, mcpRenderSettingHandle));
            break;
        case ERotatingDirection::CounterClockwise:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetRotatingDirection(MocapApi::RotatingDirection_CounterClockwise, mcpRenderSettingHandle));
            break;
        default:
            ATLASSERT(false);
            break;
        }

        switch (_unit)
        {
        case EUnit::Centimeter:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetUnit(MocapApi::Unit_Centimeter, mcpRenderSettingHandle));
            break;
        case EUnit::Meter:
            MCP_VERIFY(MocapApiService::Get()
                .renderSettings()
                .SetUnit(MocapApi::Uint_Meter, mcpRenderSettingHandle));
            break;
        default:
            ATLASSERT(false);
            break;
        }
        MCP_VERIFY(MocapApiService::Get()
            .application()
            .SetApplicationRenderSettings(mcpRenderSettingHandle, applicationHandle));

        MCP_VERIFY(MocapApiService::Get()
            .renderSettings()
            .DestroyRenderSettings(mcpRenderSettingHandle));
    }

    void initSettings(MocapApi::MCPApplicationHandle_t applicationHandle)
    {
        MocapApi::MCPSettingsHandle_t settingsHandle = 0;
        MCP_VERIFY(MocapApiService::Get()
            .settings()
            .CreateSettings(&settingsHandle));
        switch (_dataType)
        {
        case EDataTye::kBvh:
            setBvhSettings(settingsHandle);
            break;
        case EDataTye::kCalc:
            MocapApiService::Get()
                .settings()
                .SetSettingsCalcData(settingsHandle);
            break;
        default:
            ATLASSERT(false);
            break;
        }
        switch (_protocol)
        {
        case EProtocol::kUdp:
            setUdpSettings(settingsHandle);
            break;
        case EProtocol::kTcp:
            setTcpSettings(settingsHandle);
            break;
        default:
            ATLASSERT(false);
            break;
        }
        MCP_VERIFY(MocapApiService::Get()
            .application()
            .SetApplicationSettings(settingsHandle, applicationHandle));
        MCP_VERIFY(MocapApiService::Get()
            .settings()
            .DestroySettings(settingsHandle));
    }

private:
    MocapPanel& _mocapPanel;
    enum class EDataTye : UINT {
        kCalc,
        kBvh,
    }_dataType = EDataTye::kBvh;
    enum class EProtocol : UINT {
        kUdp,
        kTcp,
    } _protocol = EProtocol::kUdp;
    enum class EPreDefined : UINT {
        kCustom,
        kDefault,
        kUnrealEngine,
        kUnity3D,
    } _preDefined;
    enum class ECoordSystem : UINT {
        RightHanded,
        LeftHanded,
    } _coordSystem;
    enum class EUpVector : UINT {
        XAxis,
        YAxis,
        ZAxis,
        minusXAxis,
        minusYAxis,
        minusZAxis,
    } _upVector;
    enum class EFrontVector :UINT {
        ParityEven,
        ParityOdd,
        minusParityEven,
        minusParityOdd,
    } _frontVector;
    enum class ERotatingDirection :UINT {
        Clockwise,
        CounterClockwise,
    } _rotatingDirection;
    enum class EUnit :UINT {
        Centimeter,
        Meter,
    } _unit;

    std::unordered_map<PreDefineItem, EPreDefined> _preDefineItems;
    bool _disableHandleResize;

    struct LAYOUT_ITEM
    {
        UINT _id0;
        UINT _id1;
        UINT _h;
        UINT _w;
    };
    std::vector<LAYOUT_ITEM> _settingsLayoutItems;
    std::vector<LAYOUT_ITEM> _renderSettingsLayoutItems;

    void setUdpProtocol()
    {
        showTcpCtrls(false);
        showUdpCtrls(true);
    }

    void setTcpProtocol()
    {
        showTcpCtrls(true);
        showUdpCtrls(false);
    }

    void setCalcPanel() 
    {
        _mocapPanel.switchCalcPanel();
        showBvhCtrls(false);
    }

    void setBvhPanel()
    {
        _mocapPanel.switchBvhPanel();
        showBvhCtrls(true);
    }

    template<typename IDS>
    void showCtrls(const IDS& ids, bool show) 
    {
        std::for_each(ids.begin(), ids.end(),
            [this, show](UINT id) {
                GetDlgItem(id).ShowWindow(show ? SW_SHOW : SW_HIDE);
            });
    }

    void showBvhCtrls(bool show) 
    {
        static const std::array<UINT, 4> kBvhCtrls = {
            IDC_BVH_ROTATION_STATIC,
            IDC_BVH_ROTATION_COMBO,
            IDC_BVH_TRANSFORMATION_STATIC,
            IDC_BVH_TRANSFORMATION_COMBO
        };
        showCtrls(kBvhCtrls, show);
    }

    void showTcpCtrls(bool show) 
    {
        static const std::array<UINT, 4> kTcpCtrls = {
            IDC_TCP_SERVER_IP_STATIC,
            IDC_TCP_SERVER_IPADDRESS,
            IDC_TCP_SERVER_PORT_STATIC,
            IDC_TCP_SERVER_PORT_EDIT,
        };
        showCtrls(kTcpCtrls, show);
    }

    void showUdpCtrls(bool show) 
    {
        static const std::array<UINT, 8> kUdpCtrls = {
            IDC_UDP_LOCAL_IP_STATIC,
            IDC_UDP_LOCAL_IPADDRESS,
            IDC_UDP_LOCAL_PORT_STATIC,
            IDC_UDP_LOCAL_PORT_EDIT,
            IDC_UDP_SERVER_IP_STATIC,
            IDC_UDP_SERVER_IPADDRESS,
            IDC_UDP_SERVER_PORT_STATIC,
            IDC_UDP_SERVER_PORT_EDIT
        };
        showCtrls(kUdpCtrls, show);
    }

    void updateSelectedRenderSettings()
    {
        PreDefineItem preDefineItem = {
            CComboBox(GetDlgItem(IDC_COORD_SYSTEM_COMBO)).GetCurSel(),
            CComboBox(GetDlgItem(IDC_UP_VECTOR_COMBO)).GetCurSel(),
            CComboBox(GetDlgItem(IDC_FRONT_VECTOR_COMBO)).GetCurSel(),
            CComboBox(GetDlgItem(IDC_ROTATING_DIRECTION_COMBO)).GetCurSel(),
            CComboBox(GetDlgItem(IDC_UNIT_COMBO)).GetCurSel(),
        };
        auto iter = _preDefineItems.find(preDefineItem);
        if (iter != _preDefineItems.end()) {
            CComboBox(GetDlgItem(IDC_PRE_DEFINED_COMBO)).SetCurSel(static_cast<UINT>(iter->second));
        }
        else {
            CComboBox(GetDlgItem(IDC_PRE_DEFINED_COMBO)).SetCurSel(static_cast<UINT>(0));
        }
    }

    void setDefault()
    {
        CComboBox(GetDlgItem(IDC_COORD_SYSTEM_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_UP_VECTOR_COMBO)).SetCurSel(3);
        CComboBox(GetDlgItem(IDC_FRONT_VECTOR_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_ROTATING_DIRECTION_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_UNIT_COMBO)).SetCurSel(1);
    }

    void setUnrealEngine()
    {
        CComboBox(GetDlgItem(IDC_COORD_SYSTEM_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_UP_VECTOR_COMBO)).SetCurSel(2);
        CComboBox(GetDlgItem(IDC_FRONT_VECTOR_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_ROTATING_DIRECTION_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_UNIT_COMBO)).SetCurSel(1);
    }

    void setUnity3D()
    {
        CComboBox(GetDlgItem(IDC_COORD_SYSTEM_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_UP_VECTOR_COMBO)).SetCurSel(4);
        CComboBox(GetDlgItem(IDC_FRONT_VECTOR_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_ROTATING_DIRECTION_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_UNIT_COMBO)).SetCurSel(1);
    }

    void setBvhSettings(MocapApi::MCPSettingsHandle_t settingsHandle)
    {
        MCP_VERIFY(MocapApiService::Get()
            .settings()
            .SetSettingsBvhData(MocapApi::BvhDataType_Binary, settingsHandle));
        switch (CComboBox(GetDlgItem(IDC_BVH_ROTATION_COMBO)).GetCurSel())
        {
        case 0:
            MCP_VERIFY(MocapApiService::Get()
                .settings()
                .SetSettingsBvhRotation(
                    MocapApi::BvhRotation_XYZ, settingsHandle));
            break;// XYZ;
        case 1:
            MCP_VERIFY(MocapApiService::Get()
                .settings()
                .SetSettingsBvhRotation(
                    MocapApi::BvhRotation_XZY, settingsHandle));
            break;// XZY;
        case 2:
            MCP_VERIFY(MocapApiService::Get()
                .settings()
                .SetSettingsBvhRotation(
                    MocapApi::BvhRotation_YXZ, settingsHandle));
            break;// YXZ; 
        case 3:
            MCP_VERIFY(MocapApiService::Get()
                .settings()
                .SetSettingsBvhRotation(
                    MocapApi::BvhRotation_YZX, settingsHandle));
            break;// YZX; 
        case 4:
            MCP_VERIFY(MocapApiService::Get()
                .settings()
                .SetSettingsBvhRotation(
                    MocapApi::BvhRotation_ZXY, settingsHandle));
            break;// ZXY; 
        case 5:
            MCP_VERIFY(MocapApiService::Get()
                .settings()
                .SetSettingsBvhRotation(
                    MocapApi::BvhRotation_ZYX, settingsHandle));
            break;// ZYX;
        default:
            ATLASSERT(false);
            break;
        }
        switch (CComboBox(GetDlgItem(IDC_BVH_TRANSFORMATION_COMBO)).GetCurSel())
        {
        case 0:
            MCP_VERIFY(MocapApiService::Get()
                .settings()
                .SetSettingsBvhTransformation(
                    MocapApi::BvhTransformation_Enable, settingsHandle));
            break;//With Transformation;
        case 1:
            MCP_VERIFY(MocapApiService::Get()
                .settings()
                .SetSettingsBvhTransformation(
                    MocapApi::BvhTransformation_Disable, settingsHandle));
            break;//Without Transformation;"
        default:
            ATLASSERT(false);
            break;
        }
    }

    void setUdpSettings(MocapApi::MCPSettingsHandle_t settingsHandle)
    {
        {
            std::string addr;
            UINT port;
            if (GetAddressAndPort(IDC_UDP_LOCAL_IPADDRESS,
                IDC_UDP_LOCAL_PORT_EDIT, addr, port)) {
                MCP_VERIFY(MocapApiService::Get()
                    .settings()
                    .SetSettingsUDP(port, settingsHandle));
                int a = 0xcc;
            }
            else {
                AtlMessageBox(m_hWnd, L"UDP Local IP or Port error!");
            }
        }
        {
            std::string addr;
            UINT port;
            if (GetAddressAndPort(IDC_UDP_SERVER_IPADDRESS,
                IDC_UDP_SERVER_PORT_EDIT, addr, port)) {
                MCP_VERIFY(MocapApiService::Get()
                    .settings()
                    .SetSettingsUDPServer(addr.data(), port, settingsHandle));
                int a = 0xcc;
            }
            else {
                AtlMessageBox(m_hWnd, L"UDP Server IP or Port error!");
            }
        }
    }

    void setTcpSettings(MocapApi::MCPSettingsHandle_t settingsHandle)
    {
        std::string addr;
        UINT port;
        if (GetAddressAndPort(IDC_TCP_SERVER_IPADDRESS,
            IDC_TCP_SERVER_PORT_EDIT, addr, port)) {
            MCP_VERIFY(MocapApiService::Get()
                .settings()
                .SetSettingsTCP(addr.data(), port, settingsHandle));
        }
        else {
            AtlMessageBox(m_hWnd, L"TCP Server IP address or port error!");
        }
    }

    BOOL GetAddressAndPort(int nAddrId, int nPortId,
        std::string& addr, UINT& port)
    {
        DWORD dwIpv4;
        CIPAddressCtrl(GetDlgItem(nAddrId))
            .GetAddress(&dwIpv4);
        CStringW str;
        CEdit(GetDlgItem(nPortId)).GetWindowText(str);
        port = std::stoul(str.GetBuffer());
        if (dwIpv4 != 0 && port != 0) {
            boost::asio::ip::address_v4 ipv4(dwIpv4);
            addr = ipv4.to_string();
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    void relayout() 
    {
        static const int kGroupVSpace = 5;

        CRect rcSettingsGroup;
        GetDlgItem(IDC_SETTINGS_GROUP).GetWindowRect(rcSettingsGroup);
        ScreenToClient(rcSettingsGroup);
        const int kSettingsWidth = rcSettingsGroup.Width();
        CRect rcClient;
        GetClientRect(rcClient);
        rcSettingsGroup.left = (rcClient.Width() - kSettingsWidth) / 2;
        rcSettingsGroup.right = rcSettingsGroup.left + kSettingsWidth;
        rcSettingsGroup.top = MocapPanel::kControllerPanelConfig.SettingsGroupOffset.y;

        // settings items [3/28/2023 Brian.Wang]
        int offset = MocapPanel::kControllerPanelConfig.TopSpace 
            + rcSettingsGroup.top;
        std::for_each(_settingsLayoutItems.begin(), 
            _settingsLayoutItems.end(),
            [this, &offset, &rcSettingsGroup](const LAYOUT_ITEM & item) {
                if (isItemVisible(item)) {
                    CRect rc;
                    rc.top = offset;
                    rc.bottom = rc.top + item._h;
                    rc.left = rcSettingsGroup.left
                        + MocapPanel::kControllerPanelConfig.LeftSpace;
                    rc.right = rcSettingsGroup.right 
                        - MocapPanel::kControllerPanelConfig.RightSpace;
                    moveItem(item, rc);
                    offset += item._h;
                    offset += MocapPanel::kControllerPanelConfig.VSpace;
                }
            });
        offset += MocapPanel::kControllerPanelConfig.BottomSpace;
        // settings group [3/28/2023 Brian.Wang]
        rcSettingsGroup.bottom = offset;
        GetDlgItem(IDC_SETTINGS_GROUP).MoveWindow(rcSettingsGroup);

        // render settings [3/28/2023 Brian.Wang]
        CRect rcRenderSettingsGroup;
        rcRenderSettingsGroup.left = (rcClient.Width() - kSettingsWidth) / 2;
        rcRenderSettingsGroup.right = rcRenderSettingsGroup.left + kSettingsWidth;
        rcRenderSettingsGroup.top = rcSettingsGroup.bottom + kGroupVSpace;
        
        // render settings items [3/28/2023 Brian.Wang]
        int renderItemOffset = rcRenderSettingsGroup.top 
            + MocapPanel::kControllerPanelConfig.TopSpace;
        std::for_each(_renderSettingsLayoutItems.begin(), 
            _renderSettingsLayoutItems.end(), 
            [this, &renderItemOffset, &rcRenderSettingsGroup](const LAYOUT_ITEM& item) {
                CRect rc;
                rc.top = renderItemOffset;
                rc.bottom = rc.top + item._h;
                rc.left = rcRenderSettingsGroup.left
                    + MocapPanel::kControllerPanelConfig.LeftSpace;
                rc.right = rcRenderSettingsGroup.right -
                    MocapPanel::kControllerPanelConfig.RightSpace;
                moveItem(item, rc);
                renderItemOffset += item._h;
                renderItemOffset += MocapPanel::kControllerPanelConfig.VSpace;
            });
        renderItemOffset += MocapPanel::kControllerPanelConfig.BottomSpace;
        // render setting group [3/28/2023 Brian.Wang]
        rcRenderSettingsGroup.bottom = renderItemOffset;
        GetDlgItem(IDC_RENDER_SETTINGS_GROUP).MoveWindow(rcRenderSettingsGroup);

        _disableHandleResize = false;
        CRect rcWin;
        GetWindowRect(rcWin);
        rcWin.bottom = rcWin.top + rcRenderSettingsGroup.bottom;
        GetParent().ScreenToClient(rcWin);
        MoveWindow(rcWin, TRUE);
        _mocapPanel.resizeEventPanel();
        _disableHandleResize = true;
    }

    static const int kHSpace = 2;

    void moveItem(const LAYOUT_ITEM& item, const CRect& rc) 
    {
        static const float kLabelScale = 0.41f;
        static const UINT kLabelHeight = 15;
        CRect rc0, rc1;
        rc0.left = rc.left;
        rc0.top = rc.top + (rc.Height() - kLabelHeight) / 2;
        rc0.right = rc0.left + (rc.Width()- kHSpace) *0.41f;
        rc0.bottom = rc0.top + kLabelHeight;

        rc1.left = rc0.right + kHSpace;
        rc1.top = rc.top;
        rc1.right = rc1.left + rc.Width() - kHSpace - rc0.Width();
        rc1.bottom = rc.bottom;
        GetDlgItem(item._id0).MoveWindow(rc0, FALSE);
        GetDlgItem(item._id1).MoveWindow(rc1, FALSE);
    }

    void initLayout(std::vector<LAYOUT_ITEM>& items) 
    {
        std::for_each(items.begin(), items.end(), 
            [this](LAYOUT_ITEM & item) {
                CRect rc0, rc1;
                GetDlgItem(item._id0).GetWindowRect(rc0);
                GetDlgItem(item._id1).GetWindowRect(rc1);
                item._h = (std::max)(rc0.Height(), rc1.Height());
                item._w = rc0.Width() + rc1.Width() + kHSpace;
                ATLASSERT(GetDlgItem(item._id0).GetParent() == m_hWnd);
                ATLASSERT(GetDlgItem(item._id1).GetParent() == m_hWnd);
            });
    }

    bool isItemVisible(const LAYOUT_ITEM& item)
    {
        return isVisible(item._id0) && isVisible(item._id1);
    }

    bool isVisible(UINT id) 
    {
        return (GetDlgItem(id).GetWindowLong(GWL_STYLE) & WS_VISIBLE) == WS_VISIBLE;
    }

private:
    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
    {
        bHandled = true;
        initLayout(_settingsLayoutItems);
        initLayout(_renderSettingsLayoutItems);

        switch (_dataType)
        {
        case EDataTye::kBvh:
            CComboBox(GetDlgItem(IDC_DATA_TYPE_COMBO)).SetCurSel(1);
            setBvhPanel();
            break;
        case EDataTye::kCalc:
            CComboBox(GetDlgItem(IDC_DATA_TYPE_COMBO)).SetCurSel(0);
            setCalcPanel();
            break;
        default:
            // Brian.Wang todo [28/03/2023] 
            ATLASSERT(false);
            break;
        }
        switch (_protocol)
        {
        case EProtocol::kUdp:
            CComboBox(GetDlgItem(IDC_PROTOCOL_COMBO)).SetCurSel(0);
            setUdpProtocol();
            break;
        case EProtocol::kTcp:
            CComboBox(GetDlgItem(IDC_PROTOCOL_COMBO)).SetCurSel(1);
            setTcpProtocol();
            break;
        default:
            // Brian.Wang todo [28/03/2023] 
            ATLASSERT(false);
            break;
        }
        relayout();

        CComboBox(GetDlgItem(IDC_BVH_ROTATION_COMBO)).SetCurSel(2);
        CComboBox(GetDlgItem(IDC_BVH_TRANSFORMATION_COMBO)).SetCurSel(0);

        CComboBox(GetDlgItem(IDC_PRE_DEFINED_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_COORD_SYSTEM_COMBO)).SetCurSel(0);
        CComboBox(GetDlgItem(IDC_UP_VECTOR_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_FRONT_VECTOR_COMBO)).SetCurSel(0);
        CComboBox(GetDlgItem(IDC_ROTATING_DIRECTION_COMBO)).SetCurSel(1);
        CComboBox(GetDlgItem(IDC_UNIT_COMBO)).SetCurSel(0);
        _preDefined = EPreDefined::kDefault;
        _coordSystem = ECoordSystem::RightHanded;
        _upVector = EUpVector::YAxis;
        _frontVector = EFrontVector::ParityEven;
        _rotatingDirection = ERotatingDirection::CounterClockwise;
        _unit = EUnit::Centimeter;

#if (defined DEBUG || defined _DEBUG)
        CIPAddressCtrl(GetDlgItem(IDC_TCP_SERVER_IPADDRESS))
            .SetAddress(boost::asio::ip::address_v4::loopback().to_uint());
        CEdit(GetDlgItem(IDC_TCP_SERVER_PORT_EDIT)).SetWindowText(L"7001");

        CIPAddressCtrl(GetDlgItem(IDC_UDP_SERVER_IPADDRESS))
            .SetAddress(boost::asio::ip::address_v4::loopback().to_uint());
        CEdit(GetDlgItem(IDC_UDP_SERVER_PORT_EDIT)).SetWindowText(L"8001");

        CIPAddressCtrl(GetDlgItem(IDC_UDP_LOCAL_IPADDRESS))
            .SetAddress(boost::asio::ip::address_v4::loopback().to_uint());
        CEdit(GetDlgItem(IDC_UDP_LOCAL_PORT_EDIT)).SetWindowText(L"8002");
#endif
        return 0;
    }
    LRESULT OnDataTypeComboSelChange(WORD code, WORD id, HWND hWnd, BOOL& bHandled)
    {
        CComboBox cmb(hWnd);
        switch (cmb.GetCurSel()) {
        case 0:
            if (_dataType != EDataTye::kCalc) {
                _dataType = EDataTye::kCalc;
                setCalcPanel();
                relayout();
            }   break;
        case 1:
            if (_dataType != EDataTye::kBvh) {
                _dataType = EDataTye::kBvh;
                setBvhPanel();
                relayout();
            }   break;
            break;
        default:
            ATLASSERT(FALSE);
            break;
        }
        bHandled = TRUE;
        return 1;
    }
    LRESULT OnProtocolComboSelChange(WORD code, WORD id, HWND hWnd, BOOL& bHandled)
    {
        CComboBox cmb(hWnd);
        switch (cmb.GetCurSel()) {
        case 0:
            if (_protocol != EProtocol::kUdp) {
                _protocol = EProtocol::kUdp;
                setUdpProtocol();
                relayout();
            }   break;
        case 1:
            if (_protocol != EProtocol::kTcp) {
                _protocol = EProtocol::kTcp;
                setTcpProtocol();
                relayout();
            }   break;
        default:
            ATLASSERT(FALSE);
            break;
        }
        bHandled = TRUE;
        return 1;
    }
    LRESULT OnUpVectorComboSelChange(WORD code, WORD id, HWND hWnd, BOOL& bHandled)
    {
        CComboBox cmb(hWnd);
        switch (cmb.GetCurSel())
        {
        case 0:
            if (_upVector != EUpVector::XAxis)
                _upVector = EUpVector::XAxis;
            break;
        case 1:
            if (_upVector != EUpVector::YAxis)
                _upVector = EUpVector::YAxis;
            break;
        case 2:
            if (_upVector != EUpVector::YAxis)
                _upVector = EUpVector::ZAxis;
            break;
        case 3:
            if (_upVector != EUpVector::minusXAxis)
                _upVector = EUpVector::minusXAxis;
            break;
        case 4:
            if (_upVector != EUpVector::minusYAxis)
                _upVector = EUpVector::minusYAxis;
            break;
        case 5:
            if (_upVector != EUpVector::minusZAxis)
                _upVector = EUpVector::minusZAxis;
            break;

        default:
            ATLASSERT(false);
            break;
        }
        updateSelectedRenderSettings();
        bHandled = TRUE;
        return 1;
    }
    LRESULT OnPreDefinedComboSelChange(WORD code, WORD id, HWND hWnd, BOOL& bHandled)
    {
        CComboBox cmb(hWnd);
        ATLTRACE("%d\n", cmb.GetCurSel());
        switch (cmb.GetCurSel()) {
        case 0:
            if (_preDefined != EPreDefined::kCustom) {
                _preDefined = EPreDefined::kCustom;
                cmb.SetCurSel(static_cast<UINT>(_preDefined));
            }   break;
        case 1:
            if (_preDefined != EPreDefined::kDefault) {
                _preDefined = EPreDefined::kDefault;
                setDefault();
            }   break;
        case 2:
            if (_preDefined != EPreDefined::kUnrealEngine) {
                _preDefined = EPreDefined::kUnrealEngine;
                setUnrealEngine();
            }   break;
        case 3:
            if (_preDefined != EPreDefined::kUnity3D) {
                _preDefined = EPreDefined::kUnity3D;
                setUnity3D();

            }   break;
        default:
            ATLASSERT(FALSE);
            break;
        }
        bHandled = TRUE;
        return 1;
    }
    LRESULT OnCoordSystemComboSelChange(WORD code, WORD id, HWND hWnd, BOOL& bHandled)
    {
        CComboBox cmb(hWnd);
        switch (cmb.GetCurSel())
        {
        case 0:
            if (_coordSystem != ECoordSystem::RightHanded)
                _coordSystem = ECoordSystem::RightHanded;
            break;
        case 1:
            if (_coordSystem != ECoordSystem::LeftHanded)
                _coordSystem = ECoordSystem::LeftHanded;
            break;
        default:
            ATLASSERT(false);
            break;
        }
        updateSelectedRenderSettings();
        bHandled = TRUE;
        return 1;
    }
    LRESULT OnFrontVectorComboSelChange(WORD code, WORD id, HWND hWnd, BOOL& bHandled)
    {
        CComboBox cmb(hWnd);
        switch (cmb.GetCurSel())
        {
        case 0:
            if (_frontVector != EFrontVector::ParityEven)
                _frontVector = EFrontVector::ParityEven;
            break;
        case 1:
            if (_frontVector != EFrontVector::ParityOdd)
                _frontVector = EFrontVector::ParityOdd;
            break;
        case 2:
            if (_frontVector != EFrontVector::minusParityEven)
                _frontVector = EFrontVector::minusParityEven;
            break;
        case 3:
            if (_frontVector != EFrontVector::minusParityOdd)
                _frontVector = EFrontVector::minusParityOdd;
            break;
        default:
            ATLASSERT(false);
            break;
        }
        updateSelectedRenderSettings();
        bHandled = TRUE;
        return 1;
    }
    LRESULT OnRotatingDirectionComboSelChange(WORD code, WORD id, HWND hWnd, BOOL& bHandled)
    {
        CComboBox cmb(hWnd);
        switch (cmb.GetCurSel())
        {
        case 0:
            if (_rotatingDirection != ERotatingDirection::Clockwise)
                _rotatingDirection = ERotatingDirection::Clockwise;
            break;
        case 1:
            if (_rotatingDirection != ERotatingDirection::CounterClockwise)
                _rotatingDirection = ERotatingDirection::CounterClockwise;
            break;
        default:
            ATLASSERT(false);
            break;
        }
        updateSelectedRenderSettings();
        bHandled = TRUE;
        return 1;
    }
    LRESULT OnUnitComboSelChange(WORD code, WORD id, HWND hWnd, BOOL& bHandled)
    {
        CComboBox cmb(hWnd);
        switch (cmb.GetCurSel())
        {
        case 0:
            if (_unit != EUnit::Centimeter)
                _unit = EUnit::Centimeter;
            break;
        case 1:
            if (_unit != EUnit::Meter)
                _unit = EUnit::Meter;
            break;
        default:
            ATLASSERT(false);
            break;
        }
        updateSelectedRenderSettings();
        bHandled = TRUE;
        return 1;
    }
    LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
    {
        CDC dc(GetDC());
        CBrush brush;
        brush.CreateSolidBrush(RGB(255, 0, 0));
        CRect rc;
        GetClientRect(rc);
        dc.FillRect(rc, brush);
        brush.DeleteObject();
        ;
        return 1;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
    {
        if (_disableHandleResize) {
            relayout();
        }
        bHandled = TRUE;
        return 0;
    }

    LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
    {
        bHandled = TRUE;
        return 0;
    }
};