#pragma once

#include <MocapApi.h>

namespace MocapApi {
    struct MCPMocapApiVersion 
    {
        uint32_t _major;
        uint32_t _minor;
        uint32_t _build;
        uint32_t _revision;
    };
}

class MocapApiService 
{
public:

    static MocapApiService& Get(void);

    MocapApi::IMCPApplication & application() 
    {
        if (nullptr != _application) {
            return *_application;
        }
        else {
            _application = GetInterface<MocapApi::IMCPApplication>(
                MocapApi::IMCPApplication_Version);
            return *_application;
        }
    }

    MocapApi::IMCPSettings& settings()
    {
        if (nullptr != _settings) {
            return *_settings;
        }
        else {
            _settings = GetInterface<MocapApi::IMCPSettings>(
                MocapApi::IMCPSettings_Version);
            return *_settings;
        }
    }

    MocapApi::IMCPRenderSettings& renderSettings() 
    {
        if (nullptr != _renderSettings) {
            return *_renderSettings;
        }
        else {
            _renderSettings = GetInterface<MocapApi::IMCPRenderSettings>(
                MocapApi::IMCPRenderSettings_Version);
            return *_renderSettings;
        }
    }

    MocapApi::IMCPAvatar& avatarInterface()
    {
        if (nullptr != _avatarSettings)
        {
            return *_avatarSettings;
        }
        else
        {
            _avatarSettings = GetInterface<MocapApi::IMCPAvatar>(
                MocapApi::IMCPAvatar_Version);
            return *_avatarSettings;
        }
    }

    MocapApi::IMCPRigidBody& rigidBodyInterface() 
    {
        if (nullptr != _rigidBodyInterface) {
            return *_rigidBodyInterface;
        }
        else {
            _rigidBodyInterface = GetInterface<MocapApi::IMCPRigidBody>(
                MocapApi::IMCPRigidBody_Version);
            return *_rigidBodyInterface;
        }
    }

    MocapApi::IMCPJoint& jointInterface()
    {
        if (nullptr != _jointSettings)
        {
            return *_jointSettings;
        }
        else
        {
            _jointSettings = GetInterface<MocapApi::IMCPJoint>(
                MocapApi::IMCPJoint_Version);
            return *_jointSettings;
        }
    }

    MocapApi::IMCPCommand& commandInterface()
    {
        if (nullptr != _command)
        {
            return *_command;
        }
        else
        {
            _command = GetInterface<MocapApi::IMCPCommand>(
                MocapApi::IMCPCommand_Version);
            return *_command;
        }
    }

    MocapApi::IMCPCalibrateMotionProgress& calibrateMotionSettings()
    {
        if (nullptr != _calibrateMotionProgressSettings)
        {
            return *_calibrateMotionProgressSettings;
        }
        else
        {
            _calibrateMotionProgressSettings = GetInterface<MocapApi::IMCPCalibrateMotionProgress>(
                MocapApi::IMCPCalibrateMotionProgress_Version);
            return *_calibrateMotionProgressSettings;
        }
    }

    MocapApi::IMCPSensorModule& sensorModuleInterface()
    {
        if (nullptr != _sensorModule)
        {
            return *_sensorModule;
        }
        else
        {
            _sensorModule = GetInterface<MocapApi::IMCPSensorModule>(
                MocapApi::IMCPSensorModule_Version);
            return *_sensorModule;
        }
    }

    MocapApi::IMCPBodyPart& bodyPartInterface()
    {
        if (nullptr != _bodyPartSettings)
        {
            return *_bodyPartSettings;
        }
        else
        {
            _bodyPartSettings = GetInterface<MocapApi::IMCPBodyPart>
                (MocapApi::IMCPBodyPart_Version);
            return *_bodyPartSettings;
        }
    }

    MocapApi::IMCPRecordNotify& recordNotifyInterface() 
    {
        return *getRecordNotifyInterface();
    }

    MocapApi::IMCPCommand * getCommandInterface()
    {
        if (nullptr != _command) {
            return _command;
        }
        _command = GetInterface<MocapApi::IMCPCommand>(
            MocapApi::IMCPCommand_Version);
        return _command;
    }

    MocapApi::IMCPRecordNotify * getRecordNotifyInterface()
    {
        if (nullptr != _recordNotifyInterface) {
            return _recordNotifyInterface;
        }
        _recordNotifyInterface = GetInterface<MocapApi::IMCPRecordNotify>(
            MocapApi::IMCPRecordNotify_Version);
        return _recordNotifyInterface;
    }

    virtual const MocapApi::MCPMocapApiVersion* mocapApiVersion() = 0;

    virtual const wchar_t* mocapApiVersionString() = 0;

protected:
    MocapApiService() 
        : _application(nullptr)
        , _settings(nullptr)
        , _renderSettings(nullptr)
        , _avatarSettings(nullptr)
        , _jointSettings(nullptr)
        , _command(nullptr)
        , _calibrateMotionProgressSettings(nullptr)
        , _sensorModule(nullptr)
        , _bodyPartSettings(nullptr)
        , _recordNotifyInterface(nullptr)
        , _rigidBodyInterface(nullptr)
    {}

    ~MocapApiService() = default;

    template<typename Interface>
    Interface* GetInterface(const char* pchInterfaceVersion)
    {
        return static_cast<Interface*>(GetInterfaceInternal(pchInterfaceVersion));
    }

    virtual void* GetInterfaceInternal(const char* ) =0;

    MocapApiService(const MocapApiService & other) = delete;
            
    MocapApiService& operator= (const MocapApiService & other) = delete;
            
    MocapApiService(MocapApiService && other) = delete;
            
    MocapApiService& operator= (MocapApiService && other) = delete;

private:
    MocapApi::IMCPApplication* _application;

    MocapApi::IMCPSettings* _settings;

    MocapApi::IMCPRenderSettings* _renderSettings;

    MocapApi::IMCPAvatar* _avatarSettings;

    MocapApi::IMCPJoint* _jointSettings;

    MocapApi::IMCPCommand* _command;

    MocapApi::IMCPCalibrateMotionProgress* _calibrateMotionProgressSettings;

    MocapApi::IMCPSensorModule* _sensorModule;

    MocapApi::IMCPBodyPart* _bodyPartSettings;

    MocapApi::IMCPRecordNotify* _recordNotifyInterface;

    MocapApi::IMCPRigidBody* _rigidBodyInterface;
};