#pragma once

template<typename MocapPanel>
class SensorModuleDataViewT
    : public MocapPanel::AbstractView 
{
public:
    explicit SensorModuleDataViewT(MocapPanel& mocapPanel) 
        : _mocapPanel(mocapPanel)
    {}

    ~SensorModuleDataViewT() 
    {}

    void init() 
    {
        _dataView.Create(_mocapPanel.GetDataViewParent(), CRect(), NULL, 
            WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_ALIGNLEFT);
        _dataView.InsertColumn(0, L"ID", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(1, L"Rotation-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(2, L"Rotation-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(3, L"Rotation-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(4, L"Rotation-W", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(5, L"AcceleratedVelocity-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(6, L"AcceleratedVelocity-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(7, L"AcceleratedVelocity-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(8, L"AngularVelocity-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(9, L"AngularVelocity-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(10, L"AngularVelocity-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(11, L"CompassValue-X", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(12, L"CompassValue-Y", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(13, L"CompassValue-Z", LVCFMT_CENTER, 126);
        _dataView.InsertColumn(14, L"Temperature", LVCFMT_CENTER, 126);
    }

    void update(MocapApi::MCPSensorModuleHandle_t sensorModuleHandle) 
    {
        auto iter = _sensorModuleIndex.find(sensorModuleHandle);
        int rowIndex = 0;
        if (iter == _sensorModuleIndex.end()) {
            rowIndex = _sensorModuleIndex.size();
            _sensorModuleIndex[sensorModuleHandle] = rowIndex;
            _dataView.SetItemCount(_sensorModuleIndex.size());
        }
        else{
            rowIndex = iter->second;
        }
        update(rowIndex);
    }

    template<typename Value>
    void updateValue(int nRow, int nCol, Value v)
    {
        auto nstr = std::to_wstring(v);
        CString ostr;
        _dataView.GetItemText(nRow, nCol, ostr);
        if (::_tccmp(ostr, nstr.data()) != 0) {
            _dataView.SetItemText(
                nRow, nCol, nstr.data());
        }
    }
    void clearValue(int nRow, int nCol) 
    {
        CString ostr;
        _dataView.GetItemText(nRow, nCol, ostr);
        if (ostr.IsEmpty()) {
            return;
        }
        _dataView.SetItemText(nRow, nCol, L"");
    }

    void update(int rowIndex, MocapApi::MCPSensorModuleHandle_t sensorModuleHandle) 
    {
        auto& sensorModuleInterface = MocapApiService::Get()
            .sensorModuleInterface();
        uint32_t id;
        if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleId(
            &id, sensorModuleHandle)) {
            updateValue(rowIndex, 0, id);
        }
        else {
            clearValue(rowIndex, 0);
        }
        float qx, qy, qz, qw;
        if (MocapApi::Error_None == sensorModuleInterface.GetSensorModulePosture(
            &qx, &qy, &qz, &qw, sensorModuleHandle)) {
            updateValue(rowIndex, 1, qx);
            updateValue(rowIndex, 2, qy);
            updateValue(rowIndex, 3, qz);
            updateValue(rowIndex, 4, qw);
        }
        else {
            clearValue(rowIndex, 1);
            clearValue(rowIndex, 2);
            clearValue(rowIndex, 3);
            clearValue(rowIndex, 4);
        }
        float accx, accy, accz;
        if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleAcceleratedVelocity(
            &accx, &accy, &accz, sensorModuleHandle)) {
            updateValue(rowIndex, 5, accx);
            updateValue(rowIndex, 6, accy);
            updateValue(rowIndex, 7, accz);
        }
        else {
            clearValue(rowIndex, 5);
            clearValue(rowIndex, 6);
            clearValue(rowIndex, 7);
        }
        float gryx, gryy, gryz;
        if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleAngularVelocity(
            &gryx, &gryy, &gryz, sensorModuleHandle)) {
            updateValue(rowIndex, 8, gryx);
            updateValue(rowIndex, 9, gryy);
            updateValue(rowIndex, 10, gryz);
        }
        else {
            clearValue(rowIndex, 8);
            clearValue(rowIndex, 9);
            clearValue(rowIndex, 10);
        }
        float magx, magy, magz;
        if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleCompassValue(
            &magx, &magy, &magz, sensorModuleHandle)) {
            updateValue(rowIndex, 11, magx);
            updateValue(rowIndex, 12, magy);
            updateValue(rowIndex, 13, magz);
        }
        else {
            clearValue(rowIndex, 11);
            clearValue(rowIndex, 12);
            clearValue(rowIndex, 13);
        }
        float temp;
        if (MocapApi::Error_None == sensorModuleInterface.GetSensorModuleTemperature(
            &temp, sensorModuleHandle)) {
            updateValue(rowIndex, 14, temp);
        }
        else {
            clearValue(rowIndex, 14);
        }
    }

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
    CListViewCtrl _dataView;
    std::unordered_map<MocapApi::MCPSensorModuleHandle_t, int> _sensorModuleIndex;
};