#pragma once
#include "column_tree_ctrl.hpp"

template<typename MocapPanel>
class AvatarSensorModuleDataViewT 
    : public MocapPanel::AbstractView
{
    template<class Owner>
    class UpdaterT
    {
    public:
        template<typename Avatar>
        UpdaterT(Avatar& avatar, Owner& owner)
            : _avatar(avatar)
            , _owner(owner)
        {}

        ~UpdaterT()
        {}

        void build(MocapApi::MCPAvatarHandle_t avatarHandle)
        {
            auto& avatarInterface = MocapApiService::Get()
                .avatarInterface();
            const char* avatarName = nullptr;
            MCP_VERIFY(avatarInterface.GetAvatarName(
                &avatarName, avatarHandle));
            _avatar._root = insertItem(avatarName);
            MocapApi::MCPJointHandle_t rootJointHandle = 0;
            MCP_VERIFY(avatarInterface.GetAvatarRootJoint(
                &rootJointHandle, avatarHandle));
            buildJointRecursively(rootJointHandle, _avatar._root);
        }

        void update(MocapApi::MCPAvatarHandle_t avatarHandle)
        {
            uint32_t numberOfJoints = 0;
            auto& avatarInterface = MocapApiService::Get()
                .avatarInterface();
            const char* avatarName = nullptr;
            MCP_VERIFY(avatarInterface.GetAvatarName(
                &avatarName, avatarHandle));
            updateItem(avatarName, _avatar._root);
            MCP_VERIFY(avatarInterface.GetAvatarJoints(nullptr,
                &numberOfJoints, avatarHandle));
            std::vector<MocapApi::MCPJointHandle_t> vJoints;
            vJoints.resize(numberOfJoints);
            MCP_VERIFY(avatarInterface.GetAvatarJoints(vJoints.data(),
                &numberOfJoints, avatarHandle));
            vJoints.resize(numberOfJoints);
            std::for_each(vJoints.begin(), vJoints.end(),
                [this](auto jointHandle) {
                    updatePostureData(jointHandle,
                    _avatar._treeItems[jointHandle]);
                });
        }

        typename Owner::Avatar& _avatar;
        Owner& _owner;

        HTREEITEM insertItem(const char* text, HTREEITEM parent = TVI_ROOT)
        {
            return _owner._dataView.GetTreeCtrl()
                .InsertItem(AToW(text).data(),
                    parent, TVI_LAST);
        }

        void updateItem(const char* text, HTREEITEM treeItem)
        {
            auto nstr = AToW(text);
            WTL::CString ostr;
            _owner._dataView.GetTreeCtrl()
                .GetItemText(treeItem, ostr);
            if (::_tccmp(ostr, nstr.data()) != 0) {
                _owner._dataView.GetTreeCtrl()
                    .SetItemText(treeItem, nstr.data());
            }
        }

        void buildJointRecursively(MocapApi::MCPJointHandle_t jointHandle,
            HTREEITEM treeParentItem)
        {
            auto& jointInterface = MocapApiService::Get()
                .jointInterface();
            const char* jointName = nullptr;
            MCP_VERIFY(jointInterface.GetJointName(
                &jointName, jointHandle));
            auto treeItem = insertItem(jointName, treeParentItem);
            setPostureData(jointHandle, treeItem);
            _avatar._treeItems[jointHandle] = treeItem;
            uint32_t numberOfChildJoints = 0;
            MCP_VERIFY(jointInterface.GetJointChild(nullptr,
                &numberOfChildJoints, jointHandle));
            std::vector<MocapApi::MCPJointHandle_t> vJoints;
            vJoints.resize(numberOfChildJoints);
            MCP_VERIFY(jointInterface.GetJointChild(vJoints.data(),
                &numberOfChildJoints, jointHandle));
            vJoints.resize(numberOfChildJoints);
            std::for_each(vJoints.begin(), vJoints.end(),
                [treeItem, this](auto childJointHandle) {
                    buildJointRecursively(childJointHandle, treeItem);
                });
        }

        void setBodyPartData(MocapApi::MCPBodyPartHandle_t bodyPartHandle,
            HTREEITEM treeItem) 
        {
            auto & bodyPartInterface = MocapApiService::Get()
                .bodyPartInterface();
            float px, py, pz;
            if (MocapApi::Error_None == bodyPartInterface.GetJointPosition(
                &px, &py, &pz, bodyPartHandle)) {
                _owner._dataView.SetItemText(treeItem, 1,
                    std::to_wstring(px).data());
                _owner._dataView.SetItemText(treeItem, 2,
                    std::to_wstring(py).data());
                _owner._dataView.SetItemText(treeItem, 3,
                    std::to_wstring(pz).data());
            }
            float qx, qy, qz, qw;
            if (MocapApi::Error_None == bodyPartInterface.GetBodyPartPosture(
                &qx, &qy, &qz, &qw, bodyPartHandle)) {
                _owner._dataView.SetItemText(treeItem, 4,
                    std::to_wstring(qx).data());
                _owner._dataView.SetItemText(treeItem, 5,
                    std::to_wstring(qy).data());
                _owner._dataView.SetItemText(treeItem, 6,
                    std::to_wstring(qz).data());
                _owner._dataView.SetItemText(treeItem, 7,
                    std::to_wstring(qw).data());
            }

            float sx, sy, sz;
            if (MocapApi::Error_None == bodyPartInterface.GetJointDisplacementSpeed(
                &sx, &sy, &sz, bodyPartHandle)) {
                _owner._dataView.SetItemText(treeItem, 8,
                    std::to_wstring(sx).data());
                _owner._dataView.SetItemText(treeItem, 9,
                    std::to_wstring(sy).data());
                _owner._dataView.SetItemText(treeItem, 10,
                    std::to_wstring(sz).data());
            }
        }

        void setSensorModuleData(MocapApi::MCPSensorModuleHandle_t sensorModuleHandle,
            HTREEITEM treeItem) 
        {
            auto & sensorModuleInterface = MocapApiService::Get()
                .sensorModuleInterface();
            uint32_t id;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleId(
                &id, sensorModuleHandle)) {
                _owner._dataView.SetItemText(treeItem, 11,
                    std::to_wstring(id).data());
            }
            float qx, qy, qz, qw;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModulePosture(
                &qx, &qy, &qz, &qw, sensorModuleHandle)) {
                _owner._dataView.SetItemText(treeItem, 12,
                    std::to_wstring(qx).data());
                _owner._dataView.SetItemText(treeItem, 13,
                    std::to_wstring(qy).data());
                _owner._dataView.SetItemText(treeItem, 14,
                    std::to_wstring(qz).data());
                _owner._dataView.SetItemText(treeItem, 15,
                    std::to_wstring(qw).data());
            }
            float accx, accy, accz;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleAcceleratedVelocity(
                &accx, &accy, &accz, sensorModuleHandle)) {
                _owner._dataView.SetItemText(treeItem, 16,
                    std::to_wstring(accx).data());
                _owner._dataView.SetItemText(treeItem, 17,
                    std::to_wstring(accy).data());
                _owner._dataView.SetItemText(treeItem, 18,
                    std::to_wstring(accz).data());
            }
            float gryx, gryy, gryz;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleAngularVelocity(
                &gryx, &gryy, &gryz, sensorModuleHandle)) {
                _owner._dataView.SetItemText(treeItem, 19,
                    std::to_wstring(gryx).data());
                _owner._dataView.SetItemText(treeItem, 20,
                    std::to_wstring(gryy).data());
                _owner._dataView.SetItemText(treeItem, 21,
                    std::to_wstring(gryz).data());
            }
            float magx, magy, magz;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleCompassValue(
                &magx, &magy, &magz, sensorModuleHandle)) {
                _owner._dataView.SetItemText(treeItem, 22,
                    std::to_wstring(magx).data());
                _owner._dataView.SetItemText(treeItem, 23,
                    std::to_wstring(magy).data());
                _owner._dataView.SetItemText(treeItem, 24,
                    std::to_wstring(magz).data());
            }
            float temp;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleTemperature(
                &temp, sensorModuleHandle)) {
                _owner._dataView.SetItemText(treeItem, 25,
                    std::to_wstring(temp).data());
            }
        }

        void setPostureData(MocapApi::MCPJointHandle_t jointHandle,
            HTREEITEM treeItem)
        {
            auto& jointInterface = MocapApiService::Get()
                .jointInterface();
            MocapApi::MCPBodyPartHandle_t bodyPartHandle = 0;
            jointInterface.GetJointBodyPart(&bodyPartHandle, jointHandle);
            setBodyPartData(bodyPartHandle, treeItem);
            MocapApi::MCPSensorModuleHandle_t sensorModuleHandle = 0;
            jointInterface.GetJointSensorModule(&sensorModuleHandle, jointHandle);
            setSensorModuleData(sensorModuleHandle, treeItem);
        }

        template<typename Value>
        void updateValue(HTREEITEM treeItem, int nColumn, Value v)
        {
            auto nstr = std::to_wstring(v);
            WTL::CString ostr;
            _owner._dataView.GetItemText(treeItem, nColumn, ostr);
            if (::_tccmp(ostr, nstr.data()) != 0) {
                _owner._dataView.SetItemText(
                    treeItem, nColumn, nstr.data());
            }
        }

        void clearValue(HTREEITEM treeItem, int nColumn) 
        {
            WTL::CString ostr;
            _owner._dataView.GetItemText(treeItem, nColumn, ostr);
            if (ostr.IsEmpty()) {
                return;
            }
            _owner._dataView.SetItemText(treeItem, nColumn, L"");
        }

        void updateBodyPartData(MocapApi::MCPBodyPartHandle_t bodyPartHandle,
            HTREEITEM treeItem) 
        {
            auto& bodyPartInterface = MocapApiService::Get()
                .bodyPartInterface();

            float px, py, pz;
            if (MocapApi::Error_None == bodyPartInterface.GetJointPosition(
                &px, &py, &pz, bodyPartHandle)) {
                updateValue(treeItem, 1, px);
                updateValue(treeItem, 2, py);
                updateValue(treeItem, 3, pz);
            }
            else {
                clearValue(treeItem, 1);
                clearValue(treeItem, 2);
                clearValue(treeItem, 3);
            }
            float qx, qy, qz, qw;
            if (MocapApi::Error_None == bodyPartInterface.GetBodyPartPosture(
                &qx, &qy, &qz, &qw, bodyPartHandle)) {
                updateValue(treeItem, 4, qx);
                updateValue(treeItem, 5, qy);
                updateValue(treeItem, 6, qz);
                updateValue(treeItem, 7, qw);
            }
            else {
                clearValue(treeItem, 4);
                clearValue(treeItem, 5);
                clearValue(treeItem, 6);
                clearValue(treeItem, 7);
            }

            float sx, sy, sz;
            if (MocapApi::Error_None == bodyPartInterface.GetJointDisplacementSpeed(
                &sx, &sy, &sz, bodyPartHandle)) {
                updateValue(treeItem, 8, sx);
                updateValue(treeItem, 9, sy);
                updateValue(treeItem, 10, sz);
            }
            else {
                clearValue(treeItem, 8);
                clearValue(treeItem, 9);
                clearValue(treeItem, 10);
            }
        }

        void updateSensorModuleData(MocapApi::MCPSensorModuleHandle_t sensorModuleHandle,
            HTREEITEM treeItem) 
        {
            auto& sensorModuleInterface = MocapApiService::Get()
                .sensorModuleInterface();
            uint32_t id;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleId(
                &id, sensorModuleHandle)) {
                updateValue(treeItem, 11, id);
            }
            else {
                clearValue(treeItem, 11);
            }
            float qx, qy, qz, qw;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModulePosture(
                &qx, &qy, &qz, &qw, sensorModuleHandle)) {
                updateValue(treeItem, 12, qx);
                updateValue(treeItem, 13, qy);
                updateValue(treeItem, 14, qz);
                updateValue(treeItem, 15, qw);
            }
            else {
                clearValue(treeItem, 12);
                clearValue(treeItem, 13);
                clearValue(treeItem, 14);
                clearValue(treeItem, 15);
            }
            float accx, accy, accz;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleAcceleratedVelocity(
                &accx, &accy, &accz, sensorModuleHandle)) {
                updateValue(treeItem, 16, accx);
                updateValue(treeItem, 17, accy);
                updateValue(treeItem, 18, accz);
            }
            else {
                clearValue(treeItem, 16);
                clearValue(treeItem, 17);
                clearValue(treeItem, 18);
            }
            float gryx, gryy, gryz;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleAngularVelocity(
                &gryx, &gryy, &gryz, sensorModuleHandle)) {
                updateValue(treeItem, 19, gryx);
                updateValue(treeItem, 20, gryy);
                updateValue(treeItem, 21, gryz);
            }
            else {
                clearValue(treeItem, 19);
                clearValue(treeItem, 20);
                clearValue(treeItem, 21);
            }
            float magx, magy, magz;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleCompassValue(
                &magx, &magy, &magz, sensorModuleHandle)) {
                updateValue(treeItem, 22, magx);
                updateValue(treeItem, 23, magy);
                updateValue(treeItem, 24, magz);
            }
            else {
                clearValue(treeItem, 22);
                clearValue(treeItem, 23);
                clearValue(treeItem, 24);
            }
            float temp;
            if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleTemperature(
                &temp, sensorModuleHandle)) {
                updateValue(treeItem, 25, temp);
            }
            else {
                clearValue(treeItem, 25);
            }
        }

        void updatePostureData(MocapApi::MCPJointHandle_t jointHandle,
            HTREEITEM treeItem)
        {
            auto& jointInterface = MocapApiService::Get()
                .jointInterface();
            MocapApi::MCPBodyPartHandle_t bodyPartHandle = 0;
            jointInterface.GetJointBodyPart(&bodyPartHandle, jointHandle);
            updateBodyPartData(bodyPartHandle, treeItem);
            MocapApi::MCPSensorModuleHandle_t sensorModuleHandle = 0;
            jointInterface.GetJointSensorModule(&sensorModuleHandle, jointHandle);
            updateSensorModuleData(sensorModuleHandle, treeItem);
        }
    };

    using Updater = UpdaterT<AvatarSensorModuleDataViewT<MocapPanel>>;
    friend class Updater;

public:
    AvatarSensorModuleDataViewT(MocapPanel& mocapPanel) 
        : _mocapPanel(mocapPanel)
    {}

    ~AvatarSensorModuleDataViewT() 
    {}

    void init() 
    {
        _dataView.Create(WS_VISIBLE|WS_CHILD, 
            CRect(), _mocapPanel.GetDlgItem(IDC_TAB_DATA_CONTENT), 0);
        _dataView.GetTreeCtrl().ModifyStyle(0, 
            TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);
        
        _dataView.InsertColumn(0, L"JointName", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(1, L"BodyPart-Position-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(2, L"BodyPart-Position-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(3, L"BodyPart-Position-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(4, L"BodyPart-Rotation-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(5, L"BodyPart-Rotation-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(6, L"BodyPart-Rotation-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(7, L"BodyPart-Rotation-W", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(8, L"BodyPart-DisplacementSpeed-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(9, L"BodyPart-DisplacementSpeed-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(10, L"BodyPart-DisplacementSpeed-Z", LVCFMT_CENTER, 126);

        _dataView.InsertColumn(11, L"SensorModule-ID", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(12, L"SensorModule-Rotation-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(13, L"SensorModule-Rotation-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(14, L"SensorModule-Rotation-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(15, L"SensorModule-Rotation-W", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(16, L"SensorModule-AcceleratedVelocity-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(17, L"SensorModule-AcceleratedVelocity-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(18, L"SensorModule-AcceleratedVelocity-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(19, L"SensorModule-AngularVelocity-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(20, L"SensorModule-AngularVelocity-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(21, L"SensorModule-AngularVelocity-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(22, L"SensorModule-CompassValue-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(23, L"SensorModule-CompassValue-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(24, L"SensorModule-CompassValue-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(25, L"SensorModule-Temperature", LVCFMT_CENTER, 126);
    }

    void update(MocapApi::MCPAvatarHandle_t avatarHandle) 
    {
        auto iter = _avatarMap.find(avatarHandle);
        if (iter != _avatarMap.end()) {
            Updater updater(iter->second, *this);
            updater.update(avatarHandle);
        }
        else {
            Updater updater(_avatarMap[avatarHandle], *this);
            updater.build(avatarHandle);
        }
    }

    static const UINT kTreeItemMask = TVIF_TEXT;
protected:
    // overide MocapPanel::AbstractView [4/7/2023 Brian.Wang]
    void show() override final 
    {
        _dataView.ShowWindow(SW_SHOW);
    }

    void hide() override final 
    {
        _dataView.ShowWindow(SW_HIDE);
    }

    void move(CRect rc) override final 
    {
        _dataView.MoveWindow(rc);
    }

private:
    MocapPanel& _mocapPanel;
    CColumnTreeCtrl _dataView;
    using Joint2TreeItemMap = std::unordered_map<MocapApi::MCPJointHandle_t, HTREEITEM>;
    struct Avatar {
        Avatar()
            : _root(NULL)
            , _treeItems()
        {}

        ~Avatar()
        {}

        Avatar(const Avatar& other) = delete;

        Avatar& operator= (const Avatar& other) = delete;

        Avatar(Avatar&& other) = delete;

        Avatar& operator= (Avatar&& other) = delete;

        HTREEITEM _root;
        Joint2TreeItemMap _treeItems;
    };
    std::unordered_map<MocapApi::MCPAvatarHandle_t, Avatar> _avatarMap;
};