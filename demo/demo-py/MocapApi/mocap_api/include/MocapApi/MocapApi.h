#ifndef _NOITOM_MOCAPAPI_H
#define _NOITOM_MOCAPAPI_H

#include <cstdint>
#if defined(_WIN32)
#define MCP_CALLTYPE __cdecl
#ifdef MCP_API_EXPORT
#define MCP_INTERFACE extern "C" __declspec( dllexport )
#elif (defined MCP_API_STATIC)
#define MCP_INTERFACE extern "C"
#else
#define MCP_INTERFACE extern "C" __declspec( dllimport )
#endif
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define MCP_CALLTYPE
#ifdef MCP_API_EXPORT
#define MCP_INTERFACE extern "C" __attribute__((visibility("default")))
#else
#define MCP_INTERFACE extern "C"
#endif
#else
#error "Unsupported Platform."
#endif

#define MOCAP_API_VERSION_MAJOR 0
#define MOCAP_API_VERSION_MINOR 0
#define MOCAP_API_VERSION_BUILD 73
#define MOCAP_API_VERSION_REVISION 19a54f85

namespace MocapApi
{
    enum EMCPError
    {
        Error_None=0,
        Error_MoreEvent=1,
        Error_InsufficientBuffer=2,
        Error_InvalidObject=3,
        Error_InvalidHandle=4,
        Error_InvalidParameter=5,
        Error_NotSupported=6,
        Error_IgnoreUDPSetting=7,
        Error_IgnoreTCPSetting=8,
        Error_IgnoreBvhSetting=9,
        Error_JointNotFound=10,
        Error_WithoutTransformation=11,
        Error_NoneMessage=12,
        Error_NoneParent=13,
        Error_NoneChild=14,
        Error_AddressInUse=15,
        Error_ServerNotReady=16,
        Error_ClientNotReady=17,
        Error_IncompleteCommand=18,
        Error_UDP=19,
        Error_TCP=20,
        Error_QueuedCommandFaild=21,
        Error_InterfaceIncompatible=22
    };
    enum EMCPJointTag
    {
        JointTag_Invalid=-1,
        JointTag_Hips=0,
        JointTag_RightUpLeg=1,
        JointTag_RightLeg=2,
        JointTag_RightFoot=3,
        JointTag_LeftUpLeg=4,
        JointTag_LeftLeg=5,
        JointTag_LeftFoot=6,
        JointTag_Spine=7,
        JointTag_Spine1=8,
        JointTag_Spine2=9,
        JointTag_Neck=10,
        JointTag_Neck1=11,
        JointTag_Head=12,
        JointTag_RightShoulder=13,
        JointTag_RightArm=14,
        JointTag_RightForeArm=15,
        JointTag_RightHand=16,
        JointTag_RightHandThumb1=17,
        JointTag_RightHandThumb2=18,
        JointTag_RightHandThumb3=19,
        JointTag_RightInHandIndex=20,
        JointTag_RightHandIndex1=21,
        JointTag_RightHandIndex2=22,
        JointTag_RightHandIndex3=23,
        JointTag_RightInHandMiddle=24,
        JointTag_RightHandMiddle1=25,
        JointTag_RightHandMiddle2=26,
        JointTag_RightHandMiddle3=27,
        JointTag_RightInHandRing=28,
        JointTag_RightHandRing1=29,
        JointTag_RightHandRing2=30,
        JointTag_RightHandRing3=31,
        JointTag_RightInHandPinky=32,
        JointTag_RightHandPinky1=33,
        JointTag_RightHandPinky2=34,
        JointTag_RightHandPinky3=35,
        JointTag_LeftShoulder=36,
        JointTag_LeftArm=37,
        JointTag_LeftForeArm=38,
        JointTag_LeftHand=39,
        JointTag_LeftHandThumb1=40,
        JointTag_LeftHandThumb2=41,
        JointTag_LeftHandThumb3=42,
        JointTag_LeftInHandIndex=43,
        JointTag_LeftHandIndex1=44,
        JointTag_LeftHandIndex2=45,
        JointTag_LeftHandIndex3=46,
        JointTag_LeftInHandMiddle=47,
        JointTag_LeftHandMiddle1=48,
        JointTag_LeftHandMiddle2=49,
        JointTag_LeftHandMiddle3=50,
        JointTag_LeftInHandRing=51,
        JointTag_LeftHandRing1=52,
        JointTag_LeftHandRing2=53,
        JointTag_LeftHandRing3=54,
        JointTag_LeftInHandPinky=55,
        JointTag_LeftHandPinky1=56,
        JointTag_LeftHandPinky2=57,
        JointTag_LeftHandPinky3=58,
        JointTag_Spine3=59,
        JointTag_JointsCount=60
    };
    typedef uint64_t MCPRigidBodyHandle_t;
    class IMCPRigidBody
    {
    public:
        virtual EMCPError GetRigidBodyRotation(float * x, float * y, float * z, float * w, MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
        virtual EMCPError GetRigidBodyPosition(float * x, float * y, float * z, MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
        virtual EMCPError GetRigidBodyStatus(int * status, MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
        virtual EMCPError GetRigidBodyId(int * id, MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
        virtual EMCPError GetRigidBodyJointTag(EMCPJointTag * jointTag_, MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
        virtual EMCPError GetRigidBodyAxisAngle(float * x, float * y, float * z, float * angle, MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
        virtual EMCPError GetRigidBodyType(uint32_t * type, MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
        virtual EMCPError GetRigidBodyName(const char ** name, MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
    };
    static const char * IMCPRigidBody_Version = "IMCPRigidBody_002";
    typedef uint64_t MCPTrackerHandle_t;
    class IMCPTracker
    {
    public:
        virtual EMCPError SendMessageData(const char * message, int len, MCPTrackerHandle_t ulTrackerHandle) = 0;
        virtual EMCPError GetTrackerRotation(float * x, float * y, float * z, float * w, const char * deviceName, MCPTrackerHandle_t ulTrackerHandle) = 0;
        virtual EMCPError GetTrackerPosition(float * x, float * y, float * z, const char * deviceName, MCPTrackerHandle_t ulTrackerHandle) = 0;
        virtual EMCPError GetTrackerEulerAng(float * x, float * y, float * z, const char * deviceName, MCPTrackerHandle_t ulTrackerHandle) = 0;
        virtual EMCPError GetDeviceCount(int * devCount, MCPTrackerHandle_t ulTrackerHandle) = 0;
        virtual EMCPError GetDeviceName(int serialNum, const char ** name, MCPTrackerHandle_t ulTrackerHandle) = 0;
    };
    static const char * IMCPTracker_Version = "IMCPTracker_001";
    enum EMCPMagEnv
    {
        kMagEnv_Unknown=-1,
        kMagEnv_Worst=0,
        kMagEnv_Worse=1,
        kMagEnv_Bad=2,
        kMagEnv_Good=3
    };
    typedef uint64_t MCPSensorModuleHandle_t;
    class IMCPSensorModule
    {
    public:
        virtual EMCPError GetSensorModulePosture(float * x, float * y, float * z, float * w, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleAngularVelocity(float * x, float * y, float * z, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleAcceleratedVelocity(float * x, float * y, float * z, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleId(uint32_t * id, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleCompassValue(float * x, float * y, float * z, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleTemperature(float * temperature, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModulePostureTimeCode(uint32_t * hour, uint32_t * minute, uint32_t * second, uint32_t * frame, uint32_t * rate, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModulePostureTime(uint32_t * hour, uint32_t * minute, uint32_t * second, uint32_t * millisecond, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleBoundBone(EMCPJointTag * bone, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleMagEnvValue(EMCPMagEnv * pMagEnvValue, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSesnorModulePacketRate(float * pRate, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleSerialNumber(const char ** serialNumber, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleFirmwareVersionString(const char ** firmwareVersion, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
        virtual EMCPError GetSensorModuleFirmwareVersion(uint32_t * major, uint32_t * minor, uint32_t * revision, uint32_t * build, MCPSensorModuleHandle_t sensorModuleHandle) = 0;
    };
    static const char * IMCPSensorModule_Version = "IMCPSensorModule_002";
    typedef uint64_t MCPBodyPartHandle_t;
    class IMCPBodyPart
    {
    public:
        virtual EMCPError GetJointPosition(float * x, float * y, float * z, MCPBodyPartHandle_t bodyPartHandle) = 0;
        virtual EMCPError GetJointDisplacementSpeed(float * x, float * y, float * z, MCPBodyPartHandle_t bodyPartHandle) = 0;
        virtual EMCPError GetBodyPartPosture(float * x, float * y, float * z, float * w, MCPBodyPartHandle_t bodyPartHandle) = 0;
    };
    static const char * IMCPBodyPart_Version = "IMCPBodyPart_001";
    enum EMCPGroundingState
    {
        GroundingState_Grounding=0,
        GroundingState_Flying=1
    };
    typedef uint64_t MCPJointHandle_t;
    class IMCPJoint
    {
    public:
        virtual EMCPError GetJointName(const char ** ppStr, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointLocalRotation(float * x, float * y, float * z, float * w, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointLocalRotationByEuler(float * x, float * y, float * z, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointLocalPosition(float * x, float * y, float * z, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointDefaultLocalPosition(float * x, float * y, float * z, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointChild(MCPJointHandle_t * pJointHandle, uint32_t * punSizeOfJointHandle, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointBodyPart(MCPBodyPartHandle_t * pBodyPartHandle, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointSensorModule(MCPSensorModuleHandle_t * pSensorModuleHandle, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointTag(EMCPJointTag * pJointTag, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointNameByTag(const char ** ppStr, EMCPJointTag jointTag) = 0;
        virtual EMCPError GetJointChildJointTag(EMCPJointTag * pJointTag, uint32_t * punSizeOfJointTag, EMCPJointTag jointTag) = 0;
        virtual EMCPError GetJointParentJointTag(EMCPJointTag * pJointTag, EMCPJointTag jointTag) = 0;
        virtual EMCPError GetJointGroundingState(EMCPGroundingState * pGroundingState, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointGroundablePoints(float * pointsPostion, uint32_t * numberOfPoints, uint32_t * plowest_index, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointGlobalRotation(float * x, float * y, float * z, float * w, MCPJointHandle_t ulJointHandle) = 0;
        virtual EMCPError GetJointGlobalPosition(float * x, float * y, float * z, MCPJointHandle_t ulJointHandle) = 0;
    };
    static const char * IMCPJoint_Version = "IMCPJoint_004";
    typedef uint64_t MCPAvatarHandle_t;
    class IMCPAvatar
    {
    public:
        virtual EMCPError GetAvatarIndex(uint32_t * index, MCPAvatarHandle_t ulAvatarHandle) = 0;
        virtual EMCPError GetAvatarRootJoint(MCPJointHandle_t * pJointHandle, MCPAvatarHandle_t ulAvatarHandle) = 0;
        virtual EMCPError GetAvatarJoints(MCPJointHandle_t * pJointHandle, uint32_t * punSizeOfJointHandle, MCPAvatarHandle_t ulAvatarHandle) = 0;
        virtual EMCPError GetAvatarJointByName(const char * name, MCPJointHandle_t * pJointHandle, MCPAvatarHandle_t ulAvatarHandle) = 0;
        virtual EMCPError GetAvatarName(const char ** ppStr, MCPAvatarHandle_t ulAvatarHandle) = 0;
        virtual EMCPError GetAvatarRigidBodies(MCPRigidBodyHandle_t * vRigidBodies, uint32_t * punSizeOfRigidBodies, MCPAvatarHandle_t ulAvatarHandle) = 0;
        virtual EMCPError GetAvatarJointHierarchy(const char ** ppStr) = 0;
        virtual EMCPError GetAvatarPostureIndex(uint32_t * postureIndex, MCPAvatarHandle_t ulAvatarHandle) = 0;
        virtual EMCPError GetAvatarPostureTimeCode(uint32_t * hour, uint32_t * minute, uint32_t * second, uint32_t * frame, uint32_t * rate, MCPAvatarHandle_t ulAvatarHandle) = 0;
        virtual EMCPError GetAvatarPostureTime(uint32_t * hour, uint32_t * minute, uint32_t * second, uint32_t * millisecond, MCPAvatarHandle_t ulAvatarHandle) = 0;
        virtual EMCPError GetAvatarPosturePtpTime(uint32_t * second, uint32_t * nanosecond, MCPAvatarHandle_t ulAvatarHandle) = 0;
    };
    static const char * IMCPAvatar_Version = "IMCPAvatar_005";
    typedef uint64_t MCPMarkerHandle_t;
    class IMCPMarker
    {
    public:
        virtual EMCPError GetMarkerPosition(float * x, float * y, float * z, MCPMarkerHandle_t handle) = 0;
    };
    static const char * IMCPMarker_Version = "IMCPMarker_001";
    typedef uint64_t MCPPWRHandle_t;
    class IMCPPWR
    {
    public:
        virtual EMCPError GetPWRId(uint32_t * id, MCPPWRHandle_t handle) = 0;
        virtual EMCPError GetPWRStatus(int * status, MCPPWRHandle_t handle) = 0;
        virtual EMCPError GetPWRPosition(float * x, float * y, float * z, MCPPWRHandle_t handle) = 0;
        virtual EMCPError GetPWRQuaternion(float * x, float * y, float * z, float * w, MCPPWRHandle_t handle) = 0;
    };
    static const char * IMCPPWR_Version = "IMCPPWR_001";
    typedef uint64_t MCPAliceBusHandle_t;
    class IMCPAliceHub
    {
    public:
        virtual EMCPError GetSensorModuleList(MCPSensorModuleHandle_t * pHandles, uint32_t * nHandles) = 0;
        virtual EMCPError GetSensorModuleTimestamp(uint64_t * timestamp) = 0;
        virtual EMCPError GetMarkerList(MCPMarkerHandle_t * pHandles, uint32_t * nHandles) = 0;
        virtual EMCPError GetMarkerTimestamp(uint64_t * timestamp) = 0;
        virtual EMCPError GetRigidBodyList(MCPRigidBodyHandle_t * pHandles, uint32_t * nHandles) = 0;
        virtual EMCPError GetRigidBodyTimestamp(uint64_t * timestamp) = 0;
        virtual EMCPError GetPWRList(MCPPWRHandle_t * pHandles, uint32_t * nHandles) = 0;
        virtual EMCPError GetPWRTimestamp(uint64_t * timestamp) = 0;
    };
    static const char * IMCPAliceHub_Version = "IMCPAliceHub_001";
    enum EStartCaptureFlag
    {
        StartCaptureFlag_EnablePtp=1
    };
    enum EMCPCommand
    {
        CommandStartCapture=0,
        CommandStopCapture=1,
        CommandZeroPosition=2,
        CommandCalibrateMotion=3,
        CommandStartRecored=4,
        CommandStopRecored=5,
        CommandResumeOriginalPosture=6,
        CommandClearZeroMotionDrift=7,
        CommandResumeOriginalHandsPosture=8,
        CommandPilotSimulationDriving=9
    };
    enum EMCPCalibrateMotionFlag
    {
        CalibrateMotionFlag_AutoNextStep=0,
        CalibrateMotionFlag_ManualNextStep=1
    };
    enum EMCPCalibrateMotionOperation
    {
        CalibrateMotionOperation_Next=0,
        CalibrateMotionOperation_Cancel=1
    };
    enum EMCPCommandStopCatpureExtraFlag
    {
        StopCatpureExtraFlag_SensorsModulesPowerOff=0,
        StopCatpureExtraFlag_SensorsModulesHibernate=1
    };
    enum EMCPCommandExtraLong
    {
        CommandExtraLong_DeviceRadio=0,
        CommandExtraLong_AvatarName=1,
        CommandExtraLong_Extra0=0,
        CommandExtraLong_Extra1=1,
        CommandExtraLong_Extra2=2,
        CommandExtraLong_Extra3=3
    };
    enum EMCPCommandProgress
    {
        CommandProgress_CalibrateMotion=0
    };
    enum EMCPRecoredFlag
    {
        RecoredFlag_AutoRecalculating=1,
        RecoredFlag_AutoExportingBvh=2
    };
    typedef uint64_t MCPCommandHandle_t;
    class IMCPCommand
    {
    public:
        virtual EMCPError CreateCommand(uint32_t cmd, MCPCommandHandle_t * handle_) = 0;
        virtual EMCPError SetCommandExtraFlags(uint32_t extraFlags, MCPCommandHandle_t handle_) = 0;
        virtual EMCPError SetCommandExtraLong(uint32_t extraLongIndex, intptr_t extraLong, MCPCommandHandle_t handle_) = 0;
        virtual EMCPError GetCommandResultMessage(const char ** pMsg, MCPCommandHandle_t handle_) = 0;
        virtual EMCPError GetCommandResultCode(uint32_t * pResCode, MCPCommandHandle_t handle_) = 0;
        virtual EMCPError GetCommandProgress(uint32_t progress, intptr_t extra, MCPCommandHandle_t handle_) = 0;
        virtual EMCPError DestroyCommand(MCPCommandHandle_t handle_) = 0;
        virtual EMCPError DuplicateCommand(MCPCommandHandle_t * duplicateCmd, MCPCommandHandle_t handle_) = 0;
        virtual EMCPError AddCommandExtraLong(uint32_t extraLongIndex, intptr_t extraLong, MCPCommandHandle_t handle_) = 0;
        virtual EMCPError GetCommandExtraLong(uint32_t extraLongIndex, intptr_t * pExtraLong, MCPCommandHandle_t handle_) = 0;
        virtual EMCPError RemoveCommandExtraLong(uint32_t extraLongIndex, MCPCommandHandle_t handle_) = 0;
        virtual EMCPError GetCommandTag(uint32_t * pTag, MCPCommandHandle_t handle_) = 0;
    };
    static const char * IMCPCommand_Version = "IMCPCommand_003";
    enum EMCPCalibrateMotionProgressStep
    {
        CalibrateMotionProgressStep_Prepare=0,
        CalibrateMotionProgressStep_Countdown=1,
        CalibrateMotionProgressStep_Progress=2
    };
    typedef uint64_t MCPCalibrateMotionProgressHandle_t;
    class IMCPCalibrateMotionProgress
    {
    public:
        virtual EMCPError GetCalibrateMotionProgressCountOfSupportPoses(uint32_t * pCount, MCPCalibrateMotionProgressHandle_t handle_) = 0;
        virtual EMCPError GetCalibrateMotionProgressNameOfSupportPose(char * pName, uint32_t * pLenOfName, uint32_t index, MCPCalibrateMotionProgressHandle_t handle_) = 0;
        virtual EMCPError GetCalibrateMotionProgressStepOfPose(uint32_t * pStep, const char * name, MCPCalibrateMotionProgressHandle_t handle_) = 0;
        virtual EMCPError GetCalibrateMotionProgressCountdownOfPose(uint32_t * pCountdown, const char * name, MCPCalibrateMotionProgressHandle_t handle_) = 0;
        virtual EMCPError GetCalibrateMotionProgressProgressOfPose(uint32_t * pProgress, const char * name, MCPCalibrateMotionProgressHandle_t handle_) = 0;
        virtual EMCPError GetCalibrateMotionProgressStepOfCurrentPose(uint32_t * pStep, char * pName, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_) = 0;
        virtual EMCPError GetCalibrateMotionProgressCountdownOfCurrentPose(uint32_t * pCountdown, char * pName, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_) = 0;
        virtual EMCPError GetCalibrateMotionProgressProgressOfCurrentPose(uint32_t * pProgress, char * pName, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_) = 0;
    };
    static const char * IMCPCalibrateMotionProgress_Version = "IMCPCalibrateMotionProgress_001";
    enum EMCPMasterStatus
    {
        MasterStatus_Online=0,
        MasterStatus_Offline=1
    };
    typedef uint64_t MCPSystemHandle_t;
    class IMCPSystem
    {
    public:
        virtual EMCPError GetMasterVersion(const char ** pstr, MCPSystemHandle_t handle) = 0;
        virtual EMCPError GetMasterSerialNumber(const char ** pstr, MCPSystemHandle_t handle) = 0;
        virtual EMCPError GetSensorModules(MCPSensorModuleHandle_t * pHandles, uint32_t * pLenOfHandles, MCPSystemHandle_t handle) = 0;
        virtual EMCPError GetMasterStatus(EMCPMasterStatus * masterStatus, MCPSystemHandle_t handle) = 0;
    };
    static const char * IMCPSystem_Version = "IMCPSystem_001";
    struct MCPEvent_Reserved_t
    {
        uint64_t reserved0;
        uint64_t reserved1;
        uint64_t reserved2;
        uint64_t reserved3;
        uint64_t reserved4;
        uint64_t reserved5;
    };
    struct MCPEvent_MotionData_t
    {
        MCPAvatarHandle_t avatarHandle;
    };
    struct MCPEvent_SystemError_t
    {
        EMCPError error;
        uint64_t info0;
    };
    struct MCPEvent_SensorModuleData_t
    {
        MCPSensorModuleHandle_t _sensorModuleHandle;
    };
    struct MCPEvent_TrackerData_t
    {
        MCPTrackerHandle_t _trackerHandle;
    };
    struct MCPEvent_MarkerData_t
    {
        MCPMarkerHandle_t _markerHandle;
    };
    struct MCPEvent_PWRData_t
    {
        MCPPWRHandle_t _pwrHandle;
    };
    enum EMCPNotify
    {
        Notify_RecordStarted=0,
        Notify_RecordStoped=1,
        Notify_RecordFinished=2,
        Notify_SystemUpdated=3
    };
    struct MCPEvent_NotifyData_t
    {
        EMCPNotify _notify;
        uint64_t _notifyHandle;
    };
    enum EMCPReplay
    {
        MCPReplay_Response=0,
        MCPReplay_Running=1,
        MCPReplay_Result=2
    };
    struct MCPEvent_CommandRespond_t
    {
        MCPCommandHandle_t _commandHandle;
        EMCPReplay _replay;
    };
    union MCPEventData_t
    {
        MCPEvent_Reserved_t reserved;
        MCPEvent_MotionData_t motionData;
        MCPEvent_SystemError_t systemError;
        MCPEvent_SensorModuleData_t sensorModuleData;
        MCPEvent_TrackerData_t trackerData;
        MCPEvent_CommandRespond_t commandRespond;
        MCPEvent_MarkerData_t markerData;
        MCPEvent_PWRData_t pwrData;
        MCPEvent_NotifyData_t notifyData;
    };
    enum EMCPEventType
    {
        MCPEvent_None=0,
        MCPEvent_AvatarUpdated=256,
        MCPEvent_RigidBodyUpdated=512,
        MCPEvent_Error=768,
        MCPEvent_SensorModulesUpdated=1024,
        MCPEvent_TrackerUpdated=1280,
        MCPEvent_CommandReply=1536,
        MCPEvent_Notify=1792,
        MCPEvent_AliceIMUUpdated=4096,
        MCPEvent_AliceRigidbodyUpdated=4097,
        MCPEvent_AliceTrackerUpdated=4098,
        MCPEvent_AliceMarkerUpdated=4099
    };
    struct MCPEvent_t
    {
        uint32_t size;
        EMCPEventType eventType;
        double fTimestamp;
        MCPEventData_t eventData;
    };
    enum EMCPBvhRotation
    {
        BvhRotation_XYZ=0,
        BvhRotation_XZY=1,
        BvhRotation_YXZ=2,
        BvhRotation_YZX=3,
        BvhRotation_ZXY=4,
        BvhRotation_ZYX=5
    };
    enum EMCPBvhData
    {
        BvhDataType_String=0,
        BvhDataType_BinaryWithOldFrameHeader=1,
        BvhDataType_Binary=2,
        BvhDataType_Mask_LegacyHumanHierarchy=4
    };
    enum EMCPBvhTransformation
    {
        BvhTransformation_Disable=0,
        BvhTransformation_Enable=1
    };
    typedef uint64_t MCPSettingsHandle_t;
    class IMCPSettings
    {
    public:
        virtual EMCPError CreateSettings(MCPSettingsHandle_t * pSettingsHandle) = 0;
        virtual EMCPError DestroySettings(MCPSettingsHandle_t ulSettingsHandle) = 0;
        virtual EMCPError SetSettingsUDP(uint16_t localPort, MCPSettingsHandle_t ulSettingsHandle) = 0;
        virtual EMCPError SetSettingsTCP(const char * serverIp, uint16_t serverPort, MCPSettingsHandle_t ulSettingsHandle) = 0;
        virtual EMCPError SetSettingsBvhRotation(EMCPBvhRotation bvhRotation, MCPSettingsHandle_t ulSettingsHandle) = 0;
        virtual EMCPError SetSettingsBvhTransformation(EMCPBvhTransformation bvhTransformation, MCPSettingsHandle_t ulSettingsHandle) = 0;
        virtual EMCPError SetSettingsBvhData(EMCPBvhData bvhData, MCPSettingsHandle_t ulSettingsHandle) = 0;
        virtual EMCPError SetSettingsCalcData(MCPSettingsHandle_t ulSettingsHandle) = 0;
        virtual EMCPError SetSettingsUDPServer(const char * serverIp, uint16_t serverPort, MCPSettingsHandle_t ulSettingsHandle) = 0;
        virtual EMCPError SetSettingsUDPEx(const char * localIp, uint16_t localPort, MCPSettingsHandle_t ulSettingsHandle) = 0;
    };
    static const char * IMCPSettings_Version = "IMCPSettings_001";
    enum EMCPUpVector
    {
        UpVector_XAxis=1,
        UpVector_YAxis=2,
        UpVector_ZAxis=3
    };
    enum EMCPFrontVector
    {
        FrontVector_ParityEven=1,
        FrontVector_ParityOdd=2
    };
    enum EMCPCoordSystem
    {
        CoordSystem_RightHanded=0,
        CoordSystem_LeftHanded=1
    };
    enum EMCPRotatingDirection
    {
        RotatingDirection_Clockwise=0,
        RotatingDirection_CounterClockwise=1
    };
    enum EMCPPreDefinedRenderSettings
    {
        PreDefinedRenderSettings_Default=0,
        PreDefinedRenderSettings_UnrealEngine=1,
        PreDefinedRenderSettings_Unity3D=2,
        PreDefinedRenderSettings_Count=3
    };
    enum EMCPUnit
    {
        Unit_Centimeter=0,
        Uint_Meter=1
    };
    typedef uint64_t MCPRenderSettingsHandle_t;
    class IMCPRenderSettings
    {
    public:
        virtual EMCPError CreateRenderSettings(MCPRenderSettingsHandle_t * pRenderSettings) = 0;
        virtual EMCPError GetPreDefRenderSettings(EMCPPreDefinedRenderSettings preDefinedRenderSettings, MCPRenderSettingsHandle_t * pRenderSettings) = 0;
        virtual EMCPError SetUpVector(EMCPUpVector upVector, int sign, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetUpVector(EMCPUpVector * pUpVector, int * sign, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError SetFrontVector(EMCPFrontVector frontVector, int sign, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetFrontVector(EMCPFrontVector * pFrontVector, int * sign, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError SetCoordSystem(EMCPCoordSystem coordSystem, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetCoordSystem(EMCPCoordSystem * pCoordSystem, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError SetRotatingDirection(EMCPRotatingDirection rotatingDirection, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetRotatingDirection(EMCPRotatingDirection * pRotatingDirection, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError SetUnit(EMCPUnit mcpUnit, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetUnit(EMCPUnit * mcpUnit, MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError DestroyRenderSettings(MCPRenderSettingsHandle_t renderSettings) = 0;
    };
    static const char * IMCPRenderSettings_Version = "IMCPRenderSettings_001";
    typedef void ( * MCPEventHandleProc ) ( const MCPEvent_t * ev , void * custom );
    typedef uint64_t MCPApplicationHandle_t;
    class IMCPApplication
    {
    public:
        virtual EMCPError CreateApplication(MCPApplicationHandle_t * ulApplicationHandle) = 0;
        virtual EMCPError DestroyApplication(MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError SetApplicationSettings(MCPSettingsHandle_t ulSettingsHandle, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError SetApplicationRenderSettings(MCPRenderSettingsHandle_t ulRenderSettings, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError OpenApplication(MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError EnableApplicationCacheEvents(MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError DisableApplicationCacheEvents(MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError ApplicationCacheEventsIsEnabled(bool * isEnabled, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError CloseApplication(MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError GetApplicationRigidBodies(MCPRigidBodyHandle_t * pRigidBodyHandle, uint32_t * punRigidBodyHandleSize, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError GetApplicationAvatars(MCPAvatarHandle_t * pAvatarHandle, uint32_t * punAvatarHandle, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError PollApplicationNextEvent(MCPEvent_t * pEvent, uint32_t * punSizeOfEvent, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError GetApplicationSensorModules(MCPSensorModuleHandle_t * pSensorModuleHandle, uint32_t * punSensorModuleHandle, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError GetApplicationTrackers(MCPTrackerHandle_t * pTrackerHandle, uint32_t * punTrackerHandle, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError QueuedServerCommand(MCPCommandHandle_t cmdHandle, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError RegisterEventHandler(MCPEventHandleProc eventHandleProc, intptr_t userData, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError UnregisterEventHandler(MCPEventHandleProc eventHandleProc, intptr_t * userData, MCPApplicationHandle_t ulApplicationHandle) = 0;
        virtual EMCPError GetAppplicationSystem(MCPSystemHandle_t * pSystemHandle, MCPApplicationHandle_t ulApplicationHandle) = 0;
    };
    static const char * IMCPApplication_Version = "IMCPApplication_004";
    typedef uint64_t MCPRecordNotifyHandle_t;
    class IMCPRecordNotify
    {
    public:
        virtual EMCPError DuplicateRecordNotify(MCPRecordNotifyHandle_t * duplicatedRecordNotifyHandle, MCPRecordNotifyHandle_t recordNotifyHandle) = 0;
        virtual EMCPError RecordNotifyGetTakeName(const char ** takeName, MCPRecordNotifyHandle_t recordNotifyHandle) = 0;
        virtual EMCPError RecordNotifyGetTakePath(const char ** takePath, MCPRecordNotifyHandle_t recordNotifyHandle) = 0;
        virtual EMCPError RecordNotifyGetTakeSaveDir(const char ** takeSaveDir, MCPRecordNotifyHandle_t recordNotifyHandle) = 0;
        virtual EMCPError RecordNotifyGetTakeFileSuffix(const char ** takeFileSuffix, MCPRecordNotifyHandle_t recordNotifyHandle) = 0;
        virtual EMCPError DestroyRecordNotify(MCPRecordNotifyHandle_t recordNotifyHandle) = 0;
    };
    static const char * IMCPRecordNotify_Version = "IMCPRecordNotify_001";
    enum EMCPRobotJointType
    {
        JointLeftShoulder_Pitch=0,
        JointLeftShoulder_Roll=1,
        JointLeftShoulder_Yaw=2,
        JointLeftElbow=3,
        JointLeftWrist_Yaw=4,
        JointLeftWrist_Roll=5,
        JointLeftWrist_Pitch=6,
        JointRightShoulder_Pitch=7,
        JointRightShoulder_Roll=8,
        JointRightShoulder_Yaw=9,
        JointRightElbow=10,
        JointRightWrist_Yaw=11,
        JointRightWrist_Roll=12,
        JointRightWrist_Pitch=13,
        JointLeftHip_Yaw=14,
        JointLeftHip_Pitch=15,
        JointLeftHip_Roll=16,
        JointLeftKnee=17,
        JointLeftAnkle_Pitch=18,
        JointLeftAnkle_Roll=19,
        JointRightHip_Yaw=20,
        JointRightHip_Pitch=21,
        JointRightHip_Roll=22,
        JointRightKnee=23,
        JointRightAnkle_Pitch=24,
        JointRightAnkle_Roll=25,
        JointSpine_Yaw=26,
        JointSpine_Roll=27,
        JointSpine_Pitch=28,
        JointHead_Yaw=29,
        JointHead_Roll=30,
        JointHead_Pitch=31,
        JointCount=32
    };
    typedef uint64_t MCPRobotHandle_t;
    class IMCPRobot
    {
    public:
        virtual EMCPError CreateRobot(const char * jsonConfigStr, MCPRobotHandle_t * pHandle) = 0;
        virtual EMCPError SetRobotFPS(int fps, MCPRobotHandle_t handle) = 0;
        virtual EMCPError UpdateRobot(MCPAvatarHandle_t avatarHandle, MCPRobotHandle_t handle) = 0;
        virtual EMCPError GetRobotRawJointAngle(EMCPRobotJointType jointType, float * value, MCPRobotHandle_t handle) = 0;
        virtual EMCPError GetRobotRetargetJointAngle(const char * jointName, float * value, MCPRobotHandle_t handle) = 0;
        virtual EMCPError GetRobotRosFrameJson(const char ** jsonStr, bool compress, MCPRobotHandle_t handle) = 0;
        virtual EMCPError GetRobotRootRotation(float * x, float * y, float * z, float * w, MCPRobotHandle_t handle) = 0;
        virtual EMCPError GetRobotRootPosition(float * x, float * y, float * z, MCPRobotHandle_t handle) = 0;
        virtual EMCPError DestroyRobot(MCPRobotHandle_t handle) = 0;
        virtual EMCPError RunRobotStep(MCPRobotHandle_t handle) = 0;
        virtual EMCPError RunRobotStep1(float fixedDelta, MCPRobotHandle_t handle) = 0;
        virtual EMCPError GetRobotSlideSpeed(float * value, MCPRobotHandle_t handle) = 0;
        virtual EMCPError GetRobotSlideHeight(float * value, MCPRobotHandle_t handle) = 0;
    };
    static const char * IMCPRobot_Version = "IMCPRobot_001";
    MCP_INTERFACE EMCPError MCP_CALLTYPE MCPGetGenericInterface(const char * pchInterfaceVersion, void ** ppInterface);
    MCP_INTERFACE void MCP_CALLTYPE MCPGetMocapApiVersion(uint32_t * major, uint32_t * minor, uint32_t * build, uint32_t * revision);
    MCP_INTERFACE const char * MCP_CALLTYPE MCPGetMocapApiVersionString();
}
#endif  // end _NOITOM_MOCAPAPI_H
