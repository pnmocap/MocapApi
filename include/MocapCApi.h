#ifndef _NOITOM_CMOCAPAPI_H
#define _NOITOM_CMOCAPAPI_H

#include <stdint.h>

#include <string.h>

#ifdef __cplusplus
    #define MCP_PROC_TABLE_EXTERN_C extern "C"
#else
    #define MCP_PROC_TABLE_EXTERN_C
#endif

#if defined(_WIN32)
#define MCP_PROC_TABLE_CALLTYPE __cdecl
#ifdef MCP_API_EXPORT
#define MCP_PROC_TABLE_API MCP_PROC_TABLE_EXTERN_C __declspec( dllexport )
#else
#define MCP_PROC_TABLE_API MCP_PROC_TABLE_EXTERN_C __declspec( dllimport )
#endif
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#ifdef MCP_API_EXPORT
#define MCP_PROC_TABLE_API MCP_PROC_TABLE_EXTERN_C __attribute__((visibility("default")))
#else
#define MCP_PROC_TABLE_API MCP_PROC_TABLE_EXTERN_C 
#endif
#else
#error "Unsupported Platform."
#endif
enum EMCPError{
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
};
enum EMCPJointTag{
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
    JointTag_JointsCount=60,
};
typedef uint64_t MCPRigidBodyHandle_t;
struct MCPRigidBody_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyRotation) ( float * x, float * y, float * z, float * w, MCPRigidBodyHandle_t ulRigidBodyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyPosition) ( float * x, float * y, float * z, MCPRigidBodyHandle_t ulRigidBodyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyStatus) ( int * status, MCPRigidBodyHandle_t ulRigidBodyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyId) ( int * id, MCPRigidBodyHandle_t ulRigidBodyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyJointTag) ( EMCPJointTag * jointTag_, MCPRigidBodyHandle_t ulRigidBodyHandle);
};
static const char * IMCPRigidBody_Version = "IMCPRigidBody_001";
typedef uint64_t MCPTrackerHandle_t;
struct MCPTracker_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SendMessageData) ( const char * message, int len, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetTrackerRotation) ( float * x, float * y, float * z, float * w, const char * deviceName, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetTrackerPosition) ( float * x, float * y, float * z, const char * deviceName, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetTrackerEulerAng) ( float * x, float * y, float * z, const char * deviceName, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetDeviceCount) ( int * devCount, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetDeviceName) ( int serialNum, const char ** name, MCPTrackerHandle_t ulTrackerHandle);
};
static const char * IMCPTracker_Version = "IMCPTracker_001";
typedef uint64_t MCPSensorModuleHandle_t;
struct MCPSensorModule_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModulePosture) ( float * x, float * y, float * z, float * w, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleAngularVelocity) ( float * x, float * y, float * z, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleAcceleratedVelocity) ( float * x, float * y, float * z, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleId) ( uint32_t * id, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleCompassValue) ( float * x, float * y, float * z, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleTemperature) ( float * temperature, MCPSensorModuleHandle_t sensorModuleHandle);
};
static const char * IMCPSensorModule_Version = "IMCPSensorModule_001";
typedef uint64_t MCPBodyPartHandle_t;
struct MCPBodyPart_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointPosition) ( float * x, float * y, float * z, MCPBodyPartHandle_t bodyPartHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointDisplacementSpeed) ( float * x, float * y, float * z, MCPBodyPartHandle_t bodyPartHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetBodyPartPosture) ( float * x, float * y, float * z, float * w, MCPBodyPartHandle_t bodyPartHandle);
};
static const char * IMCPBodyPart_Version = "IMCPBodyPart_001";
typedef uint64_t MCPJointHandle_t;
struct MCPJoint_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointName) ( const char ** ppStr, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointLocalRotation) ( float * x, float * y, float * z, float * w, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointLocalRotationByEuler) ( float * x, float * y, float * z, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointLocalPosition) ( float * x, float * y, float * z, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointDefaultLocalPosition) ( float * x, float * y, float * z, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointChild) ( MCPJointHandle_t * pJointHandle, uint32_t * punSizeOfJointHandle, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointBodyPart) ( MCPBodyPartHandle_t * pBodyPartHandle, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointSensorModule) ( MCPSensorModuleHandle_t * pSensorModuleHandle, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointTag) ( EMCPJointTag * pJointTag, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointNameByTag) ( const char ** ppStr, EMCPJointTag jointTag);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointChildJointTag) ( EMCPJointTag * pJointTag, uint32_t * punSizeOfJointTag, EMCPJointTag jointTag);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointParentJointTag) ( EMCPJointTag * pJointTag, EMCPJointTag jointTag);
};
static const char * IMCPJoint_Version = "IMCPJoint_003";
typedef uint64_t MCPAvatarHandle_t;
struct MCPAvatar_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarIndex) ( uint32_t * index, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarRootJoint) ( MCPJointHandle_t * pJointHandle, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarJoints) ( MCPJointHandle_t * pJointHandle, uint32_t * punSizeOfJointHandle, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarJointByName) ( const char * name, MCPJointHandle_t * pJointHandle, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarName) ( const char ** ppStr, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarRigidBodies) ( MCPRigidBodyHandle_t * vRigidBodies, uint32_t * punSizeOfRigidBodies, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarJointHierarchy) ( const char ** ppStr);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarPostureIndex) ( uint32_t * postureIndex, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarPostureTimeCode) ( uint32_t * hour, uint32_t * minute, uint32_t * second, uint32_t * frame, uint32_t * rate, MCPAvatarHandle_t ulAvatarHandle);
};
static const char * IMCPAvatar_Version = "IMCPAvatar_003";
enum EMCPCommand{
    CommandStartCapture=0,
    CommandStopCapture=1,
    CommandZeroPosition=2,
    CommandCalibrateMotion=3,
    CommandStartRecored=4,
    CommandStopRecored=5,
    CommandResumeOriginalPosture=6,
};
enum EMCPCommandStopCatpureExtraFlag{
    StopCatpureExtraFlag_SensorsModulesPowerOff=0,
    StopCatpureExtraFlag_SensorsModulesHibernate=1,
};
enum EMCPCommandExtraLong{
    CommandExtraLong_DeviceRadio=0,
    CommandExtraLong_AvatarName=1,
};
enum EMCPCommandProgress{
    CommandProgress_CalibrateMotion=0,
};
typedef uint64_t MCPCommandHandle_t;
struct MCPCommand_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CreateCommand) ( uint32_t cmd, MCPCommandHandle_t * handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetCommandExtraFlags) ( uint32_t extraFlags, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetCommandExtraLong) ( uint32_t extraLongIndex, intptr_t extraLong, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCommandResultMessage) ( const char ** pMsg, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCommandResultCode) ( uint32_t * pResCode, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCommandProgress) ( uint32_t progress, intptr_t extra, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DestroyCommand) ( MCPCommandHandle_t handle_);
};
static const char * IMCPCommand_Version = "IMCPCommand_001";
enum EMCPCalibrateMotionProgressStep{
    CalibrateMotionProgressStep_Prepare=0,
    CalibrateMotionProgressStep_Countdown=1,
    CalibrateMotionProgressStep_Progress=2,
};
typedef uint64_t MCPCalibrateMotionProgressHandle_t;
struct MCPCalibrateMotionProgress_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressCountOfSupportPoses) ( uint32_t * pCount, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressNameOfSupportPose) ( char * name, uint32_t * pLenOfName, uint32_t index, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressStepOfPose) ( uint32_t * pStep, const char * name, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressCountdownOfPose) ( uint32_t * pCountdown, const char * name, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressProgressOfPose) ( uint32_t * pProgress, const char * name, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressStepOfCurrentPose) ( uint32_t * pStep, char * name, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressCountdownOfCurrentPose) ( uint32_t * pCountdown, char * name, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressProgressOfCurrentPose) ( uint32_t * pProgress, char * name, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_);
};
static const char * IMCPCalibrateMotionProgress_Version = "IMCPCalibrateMotionProgress_001";
struct MCPEvent_Reserved_t{
    uint64_t reserved0;
    uint64_t reserved1;
    uint64_t reserved2;
    uint64_t reserved3;
    uint64_t reserved4;
    uint64_t reserved5;
};
struct MCPEvent_MotionData_t{
    MCPAvatarHandle_t avatarHandle;
};
struct MCPEvent_SystemError_t{
    EMCPError error;
    uint64_t info0;
};
struct MCPEvent_SensorModuleData_t{
    MCPSensorModuleHandle_t _sensorModuleHandle;
};
struct MCPEvent_TrackerData_t{
    MCPTrackerHandle_t _trackerHandle;
};
enum EMCPReplay{
    MCPReplay_Response=0,
    MCPReplay_Running=1,
    MCPReplay_Result=2,
};
struct MCPEvent_CommandRespond_t{
    MCPCommandHandle_t _commandHandle;
    EMCPReplay _replay;
};
union MCPEventData_t{
    MCPEvent_Reserved_t reserved;
    MCPEvent_MotionData_t motionData;
    MCPEvent_SystemError_t systemError;
    MCPEvent_SensorModuleData_t sensorModuleData;
    MCPEvent_TrackerData_t trackerData;
    MCPEvent_CommandRespond_t commandRespond;
};
enum EMCPEventType{
    MCPEvent_None=0,
    MCPEvent_AvatarUpdated=256,
    MCPEvent_RigidBodyUpdated=512,
    MCPEvent_Error=768,
    MCPEvent_SensorModulesUpdated=1024,
    MCPEvent_TrackerUpdated=1280,
    MCPEvent_CommandReply=1536,
};
struct MCPEvent_t{
    uint32_t size;
    EMCPEventType eventType;
    double fTimestamp;
    MCPEventData_t eventData;
};
enum EMCPBvhRotation{
    BvhRotation_XYZ=0,
    BvhRotation_XZY=1,
    BvhRotation_YXZ=2,
    BvhRotation_YZX=3,
    BvhRotation_ZXY=4,
    BvhRotation_ZYX=5,
};
enum EMCPBvhData{
    BvhDataType_String=0,
    BvhDataType_BinaryWithOldFrameHeader=1,
    BvhDataType_Binary=2,
    BvhDataType_Mask_LegacyHumanHierarchy=4,
};
enum EMCPBvhTransformation{
    BvhTransformation_Disable=0,
    BvhTransformation_Enable=1,
};
typedef uint64_t MCPSettingsHandle_t;
struct MCPSettings_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CreateSettings) ( MCPSettingsHandle_t * pSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DestroySettings) ( MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsUDP) ( uint16_t localPort, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsTCP) ( const char * serverIp, uint16_t serverPort, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsBvhRotation) ( EMCPBvhRotation bvhRotation, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsBvhTransformation) ( EMCPBvhTransformation bvhTransformation, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsBvhData) ( EMCPBvhData bvhData, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsCalcData) ( MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsUDPServer) ( const char * serverIp, uint16_t serverPort, MCPSettingsHandle_t ulSettingsHandle);
};
static const char * IMCPSettings_Version = "IMCPSettings_001";
enum EMCPUpVector{
    UpVector_XAxis=1,
    UpVector_YAxis=2,
    UpVector_ZAxis=3,
};
enum EMCPFrontVector{
    FrontVector_ParityEven=1,
    FrontVector_ParityOdd=2,
};
enum EMCPCoordSystem{
    CoordSystem_RightHanded=0,
    CoordSystem_LeftHanded=1,
};
enum EMCPRotatingDirection{
    RotatingDirection_Clockwise=0,
    RotatingDirection_CounterClockwise=1,
};
enum EMCPPreDefinedRenderSettings{
    PreDefinedRenderSettings_Default=0,
    PreDefinedRenderSettings_UnrealEngine=1,
    PreDefinedRenderSettings_Unity3D=2,
    PreDefinedRenderSettings_Count=3,
};
enum EMCPUnit{
    Unit_Centimeter=0,
    Uint_Meter=1,
};
typedef uint64_t MCPRenderSettingsHandle_t;
struct MCPRenderSettings_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CreateRenderSettings) ( MCPRenderSettingsHandle_t * pRenderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetPreDefRenderSettings) ( EMCPPreDefinedRenderSettings preDefinedRenderSettings, MCPRenderSettingsHandle_t * pRenderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetUpVector) ( EMCPUpVector upVector, int sign, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetUpVector) ( EMCPUpVector * pUpVector, int * sign, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetFrontVector) ( EMCPFrontVector frontVector, int sign, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetFrontVector) ( EMCPFrontVector * pFrontVector, int * sign, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetCoordSystem) ( EMCPCoordSystem coordSystem, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCoordSystem) ( EMCPCoordSystem * pCoordSystem, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetRotatingDirection) ( EMCPRotatingDirection rotatingDirection, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRotatingDirection) ( EMCPRotatingDirection * pRotatingDirection, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetUnit) ( EMCPUnit mcpUnit, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetUnit) ( EMCPUnit * mcpUnit, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DestroyRenderSettings) ( MCPRenderSettingsHandle_t renderSettings);
};
static const char * IMCPRenderSettings_Version = "IMCPRenderSettings_001";
typedef uint64_t MCPApplicationHandle_t;
struct MCPApplication_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CreateApplication) ( MCPApplicationHandle_t * ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DestroyApplication) ( MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetApplicationSettings) ( MCPSettingsHandle_t ulSettingsHandle, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetApplicationRenderSettings) ( MCPRenderSettingsHandle_t ulRenderSettings, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * OpenApplication) ( MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * EnableApplicationCacheEvents) ( MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DisableApplicationCacheEvents) ( MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * ApplicationCacheEventsIsEnabled) ( bool * isEnabled, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CloseApplication) ( MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetApplicationRigidBodies) ( MCPRigidBodyHandle_t * pRigidBodyHandle, uint32_t * punRigidBodyHandleSize, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetApplicationAvatars) ( MCPAvatarHandle_t * pAvatarHandle, uint32_t * punAvatarHandle, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * PollApplicationNextEvent) ( MCPEvent_t * pEvent, uint32_t * punSizeOfEvent, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetApplicationSensorModules) ( MCPSensorModuleHandle_t * pSensorModuleHandle, uint32_t * punSensorModuleHandle, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetApplicationTrackers) ( MCPTrackerHandle_t * pTrackerHandle, uint32_t * punTrackerHandle, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * QueuedServerCommand) ( MCPCommandHandle_t cmdHandle, MCPApplicationHandle_t ulApplicationHandle);
};
static const char * IMCPApplication_Version = "IMCPApplication_002";
MCP_PROC_TABLE_API EMCPError MCPGetGenericInterface ( const char * pchInterfaceVersion, void ** ppInterface);

static EMCPError MCPGetProcessTable(const char * pchInterfaceVersion, void ** ppInterface)
{
    char table[128] = "PROC_TABLE:";
    return MCPGetGenericInterface(strcat(table, pchInterfaceVersion), ppInterface);
}
#endif /* END OF _NOITOM_CMOCAPAPI_H */
