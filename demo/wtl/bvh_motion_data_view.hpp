#pragma once
#include "column_tree_ctrl.hpp"
#include <atlsplit.h>

template<typename MocapPanel>
class BvhMotionDataViewT 
    : public MocapPanel::AbstractView
{
    template<class Owner>
    class UpdaterT 
    {
    public:
        template<typename Avatar>
        UpdaterT(Avatar & avatar, Owner & owner)
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
            updateRigidBodies(avatarHandle);
        }

        void update(MocapApi::MCPAvatarHandle_t avatarHandle) 
        {
            uint32_t numberOfJoints = 0;
            auto & avatarInterface = MocapApiService::Get()
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
            updateRigidBodies(avatarHandle);
        }

        void updateRigidBodies(MocapApi::MCPAvatarHandle_t avatarHandle) 
        {
            auto& avatarInterface = MocapApiService::Get()
                .avatarInterface();
            uint32_t numberOfRigidBodies = 0;
            avatarInterface.GetAvatarRigidBodies(nullptr, 
                &numberOfRigidBodies, avatarHandle);
            if (numberOfRigidBodies == 0) {
                return;
            }
            std::vector<MocapApi::MCPRigidBodyHandle_t> vRigidBody;
            vRigidBody.resize(numberOfRigidBodies);
            avatarInterface.GetAvatarRigidBodies(vRigidBody.data(),
                &numberOfRigidBodies, avatarHandle);
            vRigidBody.resize(numberOfRigidBodies);
            std::for_each(vRigidBody.begin(), vRigidBody.end(), 
                [this](auto rigidBody) {
                    _owner.updateRigidBody(rigidBody);
                });
        }

        typename Owner::Avatar & _avatar;
        Owner& _owner;

        HTREEITEM insertItem(const char* text, HTREEITEM parent= TVI_ROOT)
        {
            return _owner._bvhMotionDataView.GetTreeCtrl()
                .InsertItem(AToW(text).data(), 
                    parent, TVI_LAST);
        }

        void updateItem(const char* text, HTREEITEM treeItem) 
        {
            auto nstr = AToW(text);
            WTL::CString ostr;
            _owner._bvhMotionDataView.GetTreeCtrl()
                .GetItemText(treeItem, ostr);
            if (::_tccmp(ostr, nstr.data()) != 0) {
                _owner._bvhMotionDataView.GetTreeCtrl()
                    .SetItemText(treeItem, nstr.data());
            }
        }

        void buildJointRecursively(MocapApi::MCPJointHandle_t jointHandle,
            HTREEITEM treeParentItem) 
        {
            auto & jointInterface = MocapApiService::Get()
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

        void setPostureData(MocapApi::MCPJointHandle_t jointHandle,
            HTREEITEM treeItem) 
        {
            auto& jointInterface = MocapApiService::Get()
                .jointInterface();
            float x = 0.0f, y = 0.0f, z = 0.0f;
            jointInterface.GetJointLocalPosition(
                &x, &y, &z, jointHandle);
            float r0, r1, r2;
            jointInterface.GetJointLocalRotationByEuler(
                &r0, &r1, &r2, jointHandle);
            _owner._bvhMotionDataView.SetItemText(treeItem, 1, 
                std::to_wstring(x).data());
            _owner._bvhMotionDataView.SetItemText(treeItem, 2,
                std::to_wstring(y).data());
            _owner._bvhMotionDataView.SetItemText(treeItem, 3,
                std::to_wstring(z).data());
            _owner._bvhMotionDataView.SetItemText(treeItem, 4,
                std::to_wstring(r0).data());
            _owner._bvhMotionDataView.SetItemText(treeItem, 5,
                std::to_wstring(r1).data());
            _owner._bvhMotionDataView.SetItemText(treeItem, 6,
                std::to_wstring(r2).data());
        }

        void updateValue(HTREEITEM treeItem, int nColumn, float v)
        {
            auto nstr = std::to_wstring(v);
            WTL::CString ostr;
            _owner._bvhMotionDataView.GetItemText(treeItem, nColumn, ostr);
            if (::_tccmp(ostr, nstr.data()) != 0) {
                _owner._bvhMotionDataView.SetItemText(
                    treeItem, nColumn, nstr.data());
            }
        }

        void updatePostureData(MocapApi::MCPJointHandle_t jointHandle,
            HTREEITEM treeItem) 
        {
            auto& jointInterface = MocapApiService::Get()
                .jointInterface();
            float x = 0.0f, y = 0.0f, z = 0.0f;
            jointInterface.GetJointLocalPosition(
                &x, &y, &z, jointHandle);
            float r0, r1, r2;
            jointInterface.GetJointLocalRotationByEuler(
                &r0, &r1, &r2, jointHandle);
            updateValue(treeItem, 1, x);
            updateValue(treeItem, 2, y);
            updateValue(treeItem, 3, z);
            updateValue(treeItem, 4, r0);
            updateValue(treeItem, 5, r1);
            updateValue(treeItem, 6, r2);
        }
    };

    using Updater = UpdaterT<BvhMotionDataViewT<MocapPanel>>;
    friend class Updater;
public:
    explicit BvhMotionDataViewT(MocapPanel& mocapPanel)
        : _mocapPanel(mocapPanel)
        , _bvhMotionDataView()
    {}

    ~BvhMotionDataViewT() 
    {}

    void init() 
    {
        _dataPanel.Create(_mocapPanel.GetDataViewParent());
        _bvhMotionDataView.Create(WS_VISIBLE | WS_CHILD, CRect(), _dataPanel, 0);
        _rigidBodyDataView.Create(_dataPanel, CRect(), NULL, 
            WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_ALIGNLEFT);

        _dataPanel.SetSplitterPanes(
            _bvhMotionDataView, _rigidBodyDataView);
        _dataPanel.m_cxyMin = 100;

        _dataPanel.SetSplitterPos(200);
        _dataPanel.SetSplitterExtendedStyle(SPLIT_BOTTOMALIGNED);
        _dataPanel.SetSplitterPos();

        UINT uTreeStyle = TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
        _bvhMotionDataView.GetTreeCtrl().ModifyStyle(0, uTreeStyle);
        _bvhMotionDataView.InsertColumn(0, L"JointName", LVCFMT_CENTER, 126);
        _bvhMotionDataView.InsertColumn(1, L"BvhPositionX", LVCFMT_CENTER, 126);
        _bvhMotionDataView.InsertColumn(2, L"BvhPositionY", LVCFMT_CENTER, 126);
        _bvhMotionDataView.InsertColumn(3, L"BvhPositionZ", LVCFMT_CENTER, 126);
        _bvhMotionDataView.InsertColumn(4, L"BvhRotation-0", LVCFMT_CENTER, 126);
        _bvhMotionDataView.InsertColumn(5, L"BvhRotation-1", LVCFMT_CENTER, 126);
        _bvhMotionDataView.InsertColumn(6, L"BvhRotation-2", LVCFMT_CENTER, 126);

        _rigidBodyDataView.InsertColumn(0, L"ID", LVCFMT_CENTER, 126);
        _rigidBodyDataView.InsertColumn(1, L"Joint", LVCFMT_CENTER, 126);
        _rigidBodyDataView.InsertColumn(2, L"Position-X", LVCFMT_CENTER, 126);
        _rigidBodyDataView.InsertColumn(3, L"Position-Y", LVCFMT_CENTER, 126);
        _rigidBodyDataView.InsertColumn(4, L"Position-Z", LVCFMT_CENTER, 126);
        _rigidBodyDataView.InsertColumn(5, L"Rotation-X", LVCFMT_CENTER, 126);
        _rigidBodyDataView.InsertColumn(6, L"Rotation-Y", LVCFMT_CENTER, 126);
        _rigidBodyDataView.InsertColumn(7, L"Rotation-Z", LVCFMT_CENTER, 126);
        _rigidBodyDataView.InsertColumn(8, L"Rotation-W", LVCFMT_CENTER, 126);
        _rigidBodyDataView.InsertColumn(9, L"Status", LVCFMT_CENTER, 126);
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
        _dataPanel.ShowWindow(SW_SHOW);
    }
    void hide() override final 
    {
        _dataPanel.ShowWindow(SW_HIDE);
    }
    void move(CRect rc) override final 
    {
        _dataPanel.MoveWindow(rc);
    }

protected:

    void updateValue(int nRow, int nCol, MocapApi::EMCPJointTag jointTag)
    {
        const char* name = nullptr;
        MocapApiService::Get()
            .jointInterface()
            .GetJointNameByTag(&name, jointTag);
        WTL::CString ostr;
        _rigidBodyDataView.GetItemText(nRow, nCol, ostr);
        std::wstring nstr = AToW(name);
        if (::_tccmp(ostr, nstr.data()) != 0) {
            _rigidBodyDataView.SetItemText(nRow, nCol, nstr.data());
        }
    }

    template<typename Value>
    void updateValue(int nRow, int nCol, Value v)
    {
        auto nstr = std::to_wstring(v);
        WTL::CString ostr;
        _rigidBodyDataView.GetItemText(nRow, nCol, ostr);
        if (::_tccmp(ostr, nstr.data()) != 0) {
            _rigidBodyDataView.SetItemText(
                nRow, nCol, nstr.data());
        }
    }
    void clearValue(int nRow, int nCol)
    {
        WTL::CString ostr;
        _rigidBodyDataView.GetItemText(nRow, nCol, ostr);
        if (ostr.IsEmpty()) {
            return;
        }
        _rigidBodyDataView.SetItemText(nRow, nCol, L"");
    }

    void updateRigidBody(
        MocapApi::MCPRigidBodyHandle_t rigidBodyHandle, int rowIndex)
    {
        auto& rigidBodyInterface = MocapApiService::Get()
            .rigidBodyInterface();
        int id;
        if (MocapApi::Error_None == rigidBodyInterface.GetRigidBodyId(
            &id, rigidBodyHandle)) {
            updateValue(rowIndex, 0, id);
        }
        else {
            clearValue(rowIndex, 0);
        }

        MocapApi::EMCPJointTag jointTag;
        if (MocapApi::Error_None == rigidBodyInterface.GetRigidBodyJointTag(
            &jointTag, rigidBodyHandle)) {
            updateValue(rowIndex, 1, jointTag);
        }
        else {
            clearValue(rowIndex, 1);
        }

        float x, y, z;
        if (MocapApi::Error_None == rigidBodyInterface.GetRigidBodyPosition(
            &x, &y, &z, rigidBodyHandle)) {
            updateValue(rowIndex, 2, x);
            updateValue(rowIndex, 3, y);
            updateValue(rowIndex, 4, z);
        }
        else {
            clearValue(rowIndex, 2);
            clearValue(rowIndex, 3);
            clearValue(rowIndex, 4);
        }

        float qx, qy, qz, qw;
        if (MocapApi::Error_None == rigidBodyInterface.GetRigidBodyRotation(
            &qx, &qy, &qz, &qw, rigidBodyHandle)) {
            updateValue(rowIndex, 5, qx);
            updateValue(rowIndex, 6, qy);
            updateValue(rowIndex, 7, qz);
            updateValue(rowIndex, 8, qw);
        }
        else {
            clearValue(rowIndex, 5);
            clearValue(rowIndex, 6);
            clearValue(rowIndex, 7);
            clearValue(rowIndex, 8);
        }

        int status = 0;
        if (MocapApi::Error_None == rigidBodyInterface.GetRigidBodyStatus(
            &status, rigidBodyHandle)) {
            updateValue(rowIndex, 9, status);
        }
        else {
            clearValue(rowIndex, 9);
        }
    }

    void updateRigidBody(MocapApi::MCPRigidBodyHandle_t rigidBodyHandle)
    {
        int rowIndex = 0;
        auto iter = _rigidBodyIndexMap.find(rigidBodyHandle);
        if (iter != _rigidBodyIndexMap.end()) {
            rowIndex = iter->second;
        }
        else {
            rowIndex = _rigidBodyIndexMap.size();

            _rigidBodyIndexMap[rigidBodyHandle] = rowIndex;
        }
        updateRigidBody(rigidBodyHandle, rowIndex);
    }

private:
    MocapPanel& _mocapPanel;
    CHorSplitterWindow _dataPanel;
    CColumnTreeCtrl _bvhMotionDataView;
    CListViewCtrl _rigidBodyDataView;
    using Joint2TreeItemMap = std::unordered_map<MocapApi::MCPJointHandle_t, HTREEITEM>;
    struct Avatar{
        Avatar() 
            : _root(NULL)
            , _treeItems()
        {}

        ~Avatar() 
        {}

        Avatar(const Avatar & other) = delete;
                
        Avatar& operator= (const Avatar & other) = delete;
                
        Avatar(Avatar && other) = delete;
                
        Avatar& operator= (Avatar && other) = delete;

        HTREEITEM _root;
        Joint2TreeItemMap _treeItems;
    };
    std::unordered_map<MocapApi::MCPAvatarHandle_t, Avatar> _avatarMap;
    std::unordered_map<MocapApi::MCPRigidBodyHandle_t, int> _rigidBodyIndexMap;
};