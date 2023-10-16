// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atltheme.h>
#include "aero.h"
#include "resource.h"
#include <mocapapi.h>
#include <stdexcept>
#include <boost/asio.hpp>
#include <unordered_map>
#include <vector>
#include <sstream>
#include "w_a_conv.h"
#include <iomanip>
#define VERIFY_MOCAP_API(err)\
if(err != MocapApi::Error_None) {\
    throw std::runtime_error("");\
}

class CMainFrame : public CAeroFrameImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
    public CMessageFilter, public CIdleHandler
{
public:
    DECLARE_FRAME_WND_CLASS_EX(NULL, IDR_MAINFRAME, 0, -1)

    CMainFrame() 
        : CAeroFrameImpl<CMainFrame>()
        , CUpdateUI<CMainFrame>()
        , CMessageFilter()
        , CIdleHandler()
        , _applicationInterface(nullptr)
        , _applicationHandle(0)
        , _avatarInterface(nullptr)
        , _jointInterface(nullptr)
        , _sensorModuleInterface(nullptr)
    {
        VERIFY_MOCAP_API(MocapApi::MCPGetGenericInterface(
            MocapApi::IMCPApplication_Version, 
            reinterpret_cast<void**>(&_applicationInterface)));
        VERIFY_MOCAP_API(MocapApi::MCPGetGenericInterface(
            MocapApi::IMCPAvatar_Version,
            reinterpret_cast<void**>(&_avatarInterface)));
        VERIFY_MOCAP_API(MocapApi::MCPGetGenericInterface(
            MocapApi::IMCPJoint_Version,
            reinterpret_cast<void**>(&_jointInterface)));
        VERIFY_MOCAP_API(MocapApi::MCPGetGenericInterface(
            MocapApi::IMCPSensorModule_Version,
            reinterpret_cast<void**>(&_sensorModuleInterface)));
    }

    
    CListViewCtrl m_ListCtrl;
    CCommandBarCtrl m_CmdBar;

    CFont m_Font;
    MocapApi::IMCPApplication* _applicationInterface;
    MocapApi::MCPApplicationHandle_t _applicationHandle;
    MocapApi::IMCPAvatar* _avatarInterface;
    MocapApi::IMCPJoint* _jointInterface;
    MocapApi::IMCPSensorModule* _sensorModuleInterface;


    void InitFont()
    {
        LOGFONT lf = { 0 };
        if (IsTheming())
        {
            GetThemeSysFont(TMT_MSGBOXFONT, &lf);
        }
        else
        {
            NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
            SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, false);
            lf = ncm.lfMessageFont;
        }
        lf.lfHeight *= 3;

        ATLVERIFY(m_Font.CreateFontIndirect(&lf));
    }

//     void AeroDoPaint(CDCHandle dc, RECT& rClient, RECT& rView, RECT& rectClip)
//     {
//         rClient.top = rView.bottom;
// 
//         HFONT hOld = dc.SelectFont(m_Font);
// 
//         AeroDrawText(dc, L"WTL is great with Aero", &rClient,
//             DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX,
//             DTT_COMPOSITED | DTT_GLOWSIZE, 20);
// 
//         dc.SelectFont(hOld);
//     }

    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
            return TRUE;
        //return m_view.PreTranslateMessage(pMsg);
        return FALSE;
    }

    template<typename Stream>
    void sensorModuleSerialize(Stream& strm, MocapApi::MCPSensorModuleHandle_t sensorModuleHandle) 
    {
        uint32_t id = 0;
        auto err = _sensorModuleInterface->GetSensorModuleId(&id, sensorModuleHandle);
        if (err == MocapApi::Error_None) {
            strm << L"-" << std::setw(2)<< std::setfill(L'0') << id;
        }
        else {
            strm << L"-" << L"ID";
        }
        {
            float x, y, z, w;
            err = _sensorModuleInterface->GetSensorModulePosture(
                &x, &y, &z, &w, sensorModuleHandle);
            if (err == MocapApi::Error_None) {
                strm << L"(" << x << L"," << y << L"," << z << L"," << w << L")";
            }
            else {
                strm << L"(NoneQue)";
            }
        }
        {
            float x, y, z;
            err = _sensorModuleInterface->GetSensorModuleAngularVelocity(
                &x, &y, &z, sensorModuleHandle);
            if (err == MocapApi::Error_None) {
                strm << L"(" << x << L"," << y << L"," << z << L")";
            }
            else {
                strm << L"(NoneAngVelo)";
            }
        }
        {
            float x, y, z;
            err = _sensorModuleInterface->GetSensorModuleAcceleratedVelocity(
                &x, &y, &z, sensorModuleHandle);
            if (err == MocapApi::Error_None) {
                strm << L"(" << x << L"," << y << L"," << z << L")";
            }
            else {
                strm << L"(NoneAccVelo)";
            }
        }
        {
            float x, y, z;
            err = _sensorModuleInterface->GetSensorModuleCompassValue(
                &x, &y, &z, sensorModuleHandle);
            if (err == MocapApi::Error_None) {
                strm << L"(" << x << L"," << y << L"," << z << L")";
            }
            else {
                strm << L"(NoneCompass)";
            }
        }
        {
            float temperature;
            err = _sensorModuleInterface->GetSensorModuleTemperature(
                &temperature, sensorModuleHandle);
            if (err == MocapApi::Error_None) {
                strm << L"," << temperature;
            }
            else {
                strm << L"(NoTemp)";
            }
        }
    }

    void handleMotionDataEvent(const MocapApi::MCPEvent_MotionData_t & motionData) 
    {
        uint32_t unSizeOfJointHandle = 0;
        std::vector<MocapApi::MCPJointHandle_t> jointHandles;
        VERIFY_MOCAP_API(_avatarInterface->GetAvatarJoints(
            nullptr, &unSizeOfJointHandle, motionData.avatarHandle));
        jointHandles.resize(unSizeOfJointHandle);
        VERIFY_MOCAP_API(_avatarInterface->GetAvatarJoints(
            jointHandles.data(), &unSizeOfJointHandle, motionData.avatarHandle));
        jointHandles.resize(unSizeOfJointHandle);
        std::wostringstream wostrstrm;
        std::for_each(jointHandles.begin(), jointHandles.end(),
            [&wostrstrm, this](auto handle) {
                const char* jointName = nullptr;
                _jointInterface->GetJointName(&jointName, handle);
                wostrstrm << AToW(jointName);
                MocapApi::MCPSensorModuleHandle_t sensorModuleHandle = 0;
                VERIFY_MOCAP_API(_jointInterface->GetJointSensorModule(
                    &sensorModuleHandle, handle));
                sensorModuleSerialize(wostrstrm, sensorModuleHandle);
                wostrstrm << L";";
            });
        wostrstrm << L"\r\n";
//         m_view.SetSel(-1, -1);
//         m_view.AppendText(wostrstrm.str().data());
//         m_view.LineScroll(-1);
    }

    void handleEvent(const MocapApi::MCPEvent_t& event) 
    {
        switch (event.eventType) {
        case MocapApi::MCPEvent_AvatarUpdated:
            handleMotionDataEvent(event.eventData.motionData);
            break;
        default:
            //ATLASSERT(false);
            break;
        }
    }

    virtual BOOL OnIdle()
    {
        std::vector<MocapApi::MCPEvent_t> vEvent;
        uint32_t punSizeOfEvent = 0;
        auto err = _applicationInterface->PollApplicationNextEvent(
            nullptr, &punSizeOfEvent, _applicationHandle);
        vEvent.resize(punSizeOfEvent);
        err = _applicationInterface->PollApplicationNextEvent(
            vEvent.data(), &punSizeOfEvent, _applicationHandle);
        vEvent.resize(punSizeOfEvent);

        std::for_each(vEvent.begin(), vEvent.end(), 
            [this](const auto& event) {
            handleEvent(event);
            });

        UIUpdateToolBar();
        return FALSE;
    }

    BEGIN_UPDATE_UI_MAP(CMainFrame)
        UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
    END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP(CMainFrame)
        CHAIN_MSG_MAP(CAeroFrameImpl<CMainFrame>)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
        COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
        COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
        COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
        COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
        CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        // create command bar window
        HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
        // attach menu
        m_CmdBar.AttachMenu(GetMenu());
        // load command bar images
        m_CmdBar.LoadImages(IDR_MAINFRAME);
        // remove old menu
        SetMenu(NULL);

        HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

        CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
        AddSimpleReBarBand(hWndCmdBar);
        AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

        CreateSimpleStatusBar();

        const DWORD editStyle = LVS_REPORT | LVS_SINGLESEL | LVS_AUTOARRANGE;
        const DWORD editExtStyle = 0;
        m_hWndClient = m_ListCtrl.Create(m_hWnd, rcDefault, NULL,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | editStyle, 
            WS_EX_CLIENTEDGE | editExtStyle);
        m_ListCtrl.InsertColumn(0, L"Joint&Bone", LVCFMT_CENTER, 100);
        m_ListCtrl.InsertColumn(1, L"Message", LVCFMT_CENTER| LVCFMT_FILL);

        UIAddToolBar(hWndToolBar);
        UISetCheck(ID_VIEW_TOOLBAR, 1);
        UISetCheck(ID_VIEW_STATUS_BAR, 1);

        // create margins and font
        MARGINS m = { 2, 2, 1, 1 };
        SetMargins(m);
        InitFont();

        // register object for message filtering and idle updates
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
        pLoop->AddIdleHandler(this);

        initMocapApi();
        return 0;
    }

    void initMocapApi() 
    {
        VERIFY_MOCAP_API(_applicationInterface->CreateApplication(
            &_applicationHandle));
        MocapApi::IMCPRenderSettings* renderSettingsInterface = nullptr;
        VERIFY_MOCAP_API(MocapApi::MCPGetGenericInterface(
            MocapApi::IMCPRenderSettings_Version,
            reinterpret_cast<void**>(&renderSettingsInterface)));
        MocapApi::MCPRenderSettingsHandle_t renderSettingsHandle = 0;
        VERIFY_MOCAP_API(renderSettingsInterface->GetPreDefRenderSettings(
            MocapApi::PreDefinedRenderSettings_Default,
            &renderSettingsHandle));
        VERIFY_MOCAP_API(_applicationInterface->SetApplicationRenderSettings(
            renderSettingsHandle, _applicationHandle));
        MocapApi::IMCPSettings* settingsInterface = nullptr;
        VERIFY_MOCAP_API(MocapApi::MCPGetGenericInterface(
            MocapApi::IMCPSettings_Version,
            reinterpret_cast<void**>(&settingsInterface)));
        MocapApi::MCPSettingsHandle_t settingsHandle = 0;
        VERIFY_MOCAP_API(settingsInterface->CreateSettings(&settingsHandle));
        settingsInterface->SetSettingsTCP("127.0.0.1", 7009, settingsHandle);
        settingsInterface->SetSettingsCalcData(settingsHandle);
        VERIFY_MOCAP_API(_applicationInterface->SetApplicationSettings(
            settingsHandle, _applicationHandle));
        VERIFY_MOCAP_API(settingsInterface->DestroySettings(settingsHandle));
        VERIFY_MOCAP_API(_applicationInterface->OpenApplication(_applicationHandle));
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        // unregister message filtering and idle updates
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->RemoveMessageFilter(this);
        pLoop->RemoveIdleHandler(this);

        bHandled = FALSE;
        return 1;
    }

    LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        PostMessage(WM_CLOSE);
        return 0;
    }

    LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        // TODO: add code to initialize document

        return 0;
    }

    LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        static BOOL bVisible = TRUE;	// initially visible
        bVisible = !bVisible;
        CReBarCtrl rebar = m_hWndToolBar;
        int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
        rebar.ShowBand(nBandIndex, bVisible);
        UISetCheck(ID_VIEW_TOOLBAR, bVisible);
        UpdateLayout();
        return 0;
    }

    LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
        ::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
        UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
        UpdateLayout();
        return 0;
    }

    LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
//         CAboutDlg dlg;
//         dlg.DoModal();
        return 0;
    }
};
