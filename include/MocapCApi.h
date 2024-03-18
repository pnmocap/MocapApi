#ifndef _NOITOM_CMOCAPAPI_H
#define _NOITOM_CMOCAPAPI_H
#include <stdint.h>
#if defined(_WIN32)
#define MCP_PROC_TABLE_CALLTYPE __cdecl
#ifdef MCP_API_EXPORT
#define MCP_PROC_TABLE_API __declspec( dllexport )
#else
#define MCP_PROC_TABLE_API __declspec( dllimport )
#endif
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define MCP_PROC_TABLE_CALLTYPE 
#ifdef MCP_API_EXPORT
#define MCP_PROC_TABLE_API __attribute__((visibility("default")))
#else
#define MCP_PROC_TABLE_API 
#endif
#else
#error "Unsupported Platform."
#endif

#define MOCAP_API_VERSION_MAJOR 0
#define MOCAP_API_VERSION_MINOR 0
#define MOCAP_API_VERSION_BUILD 16
#define MOCAP_API_VERSION_REVISION c304fc96

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
struct MCPRigidBody_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyRotation) (float * x, float * y, float * z, float * w, MCPRigidBodyHandle_t ulRigidBodyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyPosition) (float * x, float * y, float * z, MCPRigidBodyHandle_t ulRigidBodyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyStatus) (int * status, MCPRigidBodyHandle_t ulRigidBodyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyId) (int * id, MCPRigidBodyHandle_t ulRigidBodyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyJointTag) (EMCPJointTag * jointTag_, MCPRigidBodyHandle_t ulRigidBodyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyAxisAngle) (float * x, float * y, float * z, float * angle, MCPRigidBodyHandle_t ulRigidBodyHandle);
};
static const char * IMCPRigidBody_Version = "PROC_TABLE:IMCPRigidBody_001";
typedef uint64_t MCPTrackerHandle_t;
struct MCPTracker_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SendMessageData) (const char * message, int len, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetTrackerRotation) (float * x, float * y, float * z, float * w, const char * deviceName, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetTrackerPosition) (float * x, float * y, float * z, const char * deviceName, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetTrackerEulerAng) (float * x, float * y, float * z, const char * deviceName, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetDeviceCount) (int * devCount, MCPTrackerHandle_t ulTrackerHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetDeviceName) (int serialNum, const char ** name, MCPTrackerHandle_t ulTrackerHandle);
};
static const char * IMCPTracker_Version = "PROC_TABLE:IMCPTracker_001";
typedef uint64_t MCPSensorModuleHandle_t;
struct MCPSensorModule_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModulePosture) (float * x, float * y, float * z, float * w, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleAngularVelocity) (float * x, float * y, float * z, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleAcceleratedVelocity) (float * x, float * y, float * z, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleId) (uint32_t * id, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleCompassValue) (float * x, float * y, float * z, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleTemperature) (float * temperature, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModulePostureTimeCode) (uint32_t * hour, uint32_t * minute, uint32_t * second, uint32_t * frame, uint32_t * rate, MCPSensorModuleHandle_t sensorModuleHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModulePostureTime) (uint32_t * hour, uint32_t * minute, uint32_t * second, uint32_t * millisecond, MCPSensorModuleHandle_t sensorModuleHandle);
};
static const char * IMCPSensorModule_Version = "PROC_TABLE:IMCPSensorModule_002";
typedef uint64_t MCPBodyPartHandle_t;
struct MCPBodyPart_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointPosition) (float * x, float * y, float * z, MCPBodyPartHandle_t bodyPartHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointDisplacementSpeed) (float * x, float * y, float * z, MCPBodyPartHandle_t bodyPartHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetBodyPartPosture) (float * x, float * y, float * z, float * w, MCPBodyPartHandle_t bodyPartHandle);
};
static const char * IMCPBodyPart_Version = "PROC_TABLE:IMCPBodyPart_001";
typedef uint64_t MCPJointHandle_t;
struct MCPJoint_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointName) (const char ** ppStr, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointLocalRotation) (float * x, float * y, float * z, float * w, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointLocalRotationByEuler) (float * x, float * y, float * z, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointLocalPosition) (float * x, float * y, float * z, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointDefaultLocalPosition) (float * x, float * y, float * z, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointChild) (MCPJointHandle_t * pJointHandle, uint32_t * punSizeOfJointHandle, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointBodyPart) (MCPBodyPartHandle_t * pBodyPartHandle, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointSensorModule) (MCPSensorModuleHandle_t * pSensorModuleHandle, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointTag) (EMCPJointTag * pJointTag, MCPJointHandle_t ulJointHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointNameByTag) (const char ** ppStr, EMCPJointTag jointTag);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointChildJointTag) (EMCPJointTag * pJointTag, uint32_t * punSizeOfJointTag, EMCPJointTag jointTag);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetJointParentJointTag) (EMCPJointTag * pJointTag, EMCPJointTag jointTag);
};
static const char * IMCPJoint_Version = "PROC_TABLE:IMCPJoint_003";
typedef uint64_t MCPAvatarHandle_t;
struct MCPAvatar_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarIndex) (uint32_t * index, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarRootJoint) (MCPJointHandle_t * pJointHandle, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarJoints) (MCPJointHandle_t * pJointHandle, uint32_t * punSizeOfJointHandle, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarJointByName) (const char * name, MCPJointHandle_t * pJointHandle, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarName) (const char ** ppStr, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarRigidBodies) (MCPRigidBodyHandle_t * vRigidBodies, uint32_t * punSizeOfRigidBodies, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarJointHierarchy) (const char ** ppStr);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarPostureIndex) (uint32_t * postureIndex, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarPostureTimeCode) (uint32_t * hour, uint32_t * minute, uint32_t * second, uint32_t * frame, uint32_t * rate, MCPAvatarHandle_t ulAvatarHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetAvatarPostureTime) (uint32_t * hour, uint32_t * minute, uint32_t * second, uint32_t * millisecond, MCPAvatarHandle_t ulAvatarHandle);
};
static const char * IMCPAvatar_Version = "PROC_TABLE:IMCPAvatar_004";
typedef uint64_t MCPMarkerHandle_t;
struct MCPMarker_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetMarkerPosition) (float * x, float * y, float * z, MCPMarkerHandle_t handle);
};
static const char * IMCPMarker_Version = "PROC_TABLE:IMCPMarker_001";
typedef uint64_t MCPPWRHandle_t;
struct MCPPWR_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetPWRId) (uint32_t * id, MCPPWRHandle_t handle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetPWRStatus) (int * status, MCPPWRHandle_t handle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetPWRPosition) (float * x, float * y, float * z, MCPPWRHandle_t handle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetPWRQuaternion) (float * x, float * y, float * z, float * w, MCPPWRHandle_t handle);
};
static const char * IMCPPWR_Version = "PROC_TABLE:IMCPPWR_001";
typedef uint64_t MCPAliceBusHandle_t;
struct MCPAliceHub_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleList) (MCPSensorModuleHandle_t * pHandles, uint32_t * nHandles);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetSensorModuleTimestamp) (uint64_t * timestamp);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetMarkerList) (MCPMarkerHandle_t * pHandles, uint32_t * nHandles);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetMarkerTimestamp) (uint64_t * timestamp);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyList) (MCPRigidBodyHandle_t * pHandles, uint32_t * nHandles);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRigidBodyTimestamp) (uint64_t * timestamp);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetPWRList) (MCPPWRHandle_t * pHandles, uint32_t * nHandles);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetPWRTimestamp) (uint64_t * timestamp);
};
static const char * IMCPAliceHub_Version = "PROC_TABLE:IMCPAliceHub_001";
enum EMCPCommand
{
    CommandStartCapture=0,
    CommandStopCapture=1,
    CommandZeroPosition=2,
    CommandCalibrateMotion=3,
    CommandStartRecored=4,
    CommandStopRecored=5,
    CommandResumeOriginalPosture=6
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
typedef uint64_t MCPCommandHandle_t;
struct MCPCommand_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CreateCommand) (uint32_t cmd, MCPCommandHandle_t * handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetCommandExtraFlags) (uint32_t extraFlags, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetCommandExtraLong) (uint32_t extraLongIndex, intptr_t extraLong, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCommandResultMessage) (const char ** pMsg, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCommandResultCode) (uint32_t * pResCode, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCommandProgress) (uint32_t progress, intptr_t extra, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DestroyCommand) (MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DuplicateCommand) (MCPCommandHandle_t * duplicateCmd, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * AddCommandExtraLong) (uint32_t extraLongIndex, intptr_t extraLong, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCommandExtraLong) (uint32_t extraLongIndex, intptr_t * pExtraLong, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * RemoveCommandExtraLong) (uint32_t extraLongIndex, MCPCommandHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCommandTag) (uint32_t * pTag, MCPCommandHandle_t handle_);
};
static const char * IMCPCommand_Version = "PROC_TABLE:IMCPCommand_002";
enum EMCPCalibrateMotionProgressStep
{
    CalibrateMotionProgressStep_Prepare=0,
    CalibrateMotionProgressStep_Countdown=1,
    CalibrateMotionProgressStep_Progress=2
};
typedef uint64_t MCPCalibrateMotionProgressHandle_t;
struct MCPCalibrateMotionProgress_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressCountOfSupportPoses) (uint32_t * pCount, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressNameOfSupportPose) (char * name, uint32_t * pLenOfName, uint32_t index, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressStepOfPose) (uint32_t * pStep, const char * name, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressCountdownOfPose) (uint32_t * pCountdown, const char * name, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressProgressOfPose) (uint32_t * pProgress, const char * name, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressStepOfCurrentPose) (uint32_t * pStep, char * name, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressCountdownOfCurrentPose) (uint32_t * pCountdown, char * name, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCalibrateMotionProgressProgressOfCurrentPose) (uint32_t * pProgress, char * name, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_);
};
static const char * IMCPCalibrateMotionProgress_Version = "PROC_TABLE:IMCPCalibrateMotionProgress_001";
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
    Notify_RecordFinished=2
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
struct MCPSettings_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CreateSettings) (MCPSettingsHandle_t * pSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DestroySettings) (MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsUDP) (uint16_t localPort, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsTCP) (const char * serverIp, uint16_t serverPort, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsBvhRotation) (EMCPBvhRotation bvhRotation, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsBvhTransformation) (EMCPBvhTransformation bvhTransformation, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsBvhData) (EMCPBvhData bvhData, MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsCalcData) (MCPSettingsHandle_t ulSettingsHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetSettingsUDPServer) (const char * serverIp, uint16_t serverPort, MCPSettingsHandle_t ulSettingsHandle);
};
static const char * IMCPSettings_Version = "PROC_TABLE:IMCPSettings_001";
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
struct MCPRenderSettings_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CreateRenderSettings) (MCPRenderSettingsHandle_t * pRenderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetPreDefRenderSettings) (EMCPPreDefinedRenderSettings preDefinedRenderSettings, MCPRenderSettingsHandle_t * pRenderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetUpVector) (EMCPUpVector upVector, int sign, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetUpVector) (EMCPUpVector * pUpVector, int * sign, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetFrontVector) (EMCPFrontVector frontVector, int sign, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetFrontVector) (EMCPFrontVector * pFrontVector, int * sign, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetCoordSystem) (EMCPCoordSystem coordSystem, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetCoordSystem) (EMCPCoordSystem * pCoordSystem, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetRotatingDirection) (EMCPRotatingDirection rotatingDirection, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetRotatingDirection) (EMCPRotatingDirection * pRotatingDirection, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetUnit) (EMCPUnit mcpUnit, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetUnit) (EMCPUnit * mcpUnit, MCPRenderSettingsHandle_t renderSettings);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DestroyRenderSettings) (MCPRenderSettingsHandle_t renderSettings);
};
static const char * IMCPRenderSettings_Version = "PROC_TABLE:IMCPRenderSettings_001";
typedef void ( * MCPEventHandleProc ) ( const MCPEvent_t * ev , void * custom );
typedef uint64_t MCPApplicationHandle_t;
struct MCPApplication_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CreateApplication) (MCPApplicationHandle_t * ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DestroyApplication) (MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetApplicationSettings) (MCPSettingsHandle_t ulSettingsHandle, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * SetApplicationRenderSettings) (MCPRenderSettingsHandle_t ulRenderSettings, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * OpenApplication) (MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * EnableApplicationCacheEvents) (MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DisableApplicationCacheEvents) (MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * ApplicationCacheEventsIsEnabled) (bool * isEnabled, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * CloseApplication) (MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetApplicationRigidBodies) (MCPRigidBodyHandle_t * pRigidBodyHandle, uint32_t * punRigidBodyHandleSize, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetApplicationAvatars) (MCPAvatarHandle_t * pAvatarHandle, uint32_t * punAvatarHandle, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * PollApplicationNextEvent) (MCPEvent_t * pEvent, uint32_t * punSizeOfEvent, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetApplicationSensorModules) (MCPSensorModuleHandle_t * pSensorModuleHandle, uint32_t * punSensorModuleHandle, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * GetApplicationTrackers) (MCPTrackerHandle_t * pTrackerHandle, uint32_t * punTrackerHandle, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * QueuedServerCommand) (MCPCommandHandle_t cmdHandle, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * RegisterEventHandler) (MCPEventHandleProc eventHandleProc, intptr_t userData, MCPApplicationHandle_t ulApplicationHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * UnregisterEventHandler) (MCPEventHandleProc eventHandleProc, intptr_t * userData, MCPApplicationHandle_t ulApplicationHandle);
};
static const char * IMCPApplication_Version = "PROC_TABLE:IMCPApplication_004";
typedef uint64_t MCPRecordNotifyHandle_t;
struct MCPRecordNotify_ProcTable {
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DuplicateRecordNotify) (MCPRecordNotifyHandle_t * duplicatedRecordNotifyHandle, MCPRecordNotifyHandle_t recordNotifyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * RecordNotifyGetTakeName) (const char ** takeName, MCPRecordNotifyHandle_t recordNotifyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * RecordNotifyGetTakePath) (const char ** takePath, MCPRecordNotifyHandle_t recordNotifyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * RecordNotifyGetTakeSaveDir) (const char ** takeSaveDir, MCPRecordNotifyHandle_t recordNotifyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * RecordNotifyGetTakeFileSuffix) (const char ** takeFileSuffix, MCPRecordNotifyHandle_t recordNotifyHandle);
    EMCPError (MCP_PROC_TABLE_CALLTYPE * DestroyRecordNotify) (MCPRecordNotifyHandle_t recordNotifyHandle);
};
static const char * IMCPRecordNotify_Version = "PROC_TABLE:IMCPRecordNotify_001";
MCP_PROC_TABLE_API EMCPError MCP_PROC_TABLE_CALLTYPE MCPGetGenericInterface(const char * pchInterfaceVersion, void ** ppInterface);
MCP_PROC_TABLE_API void MCP_PROC_TABLE_CALLTYPE MCPGetMocapApiVersion(uint32_t * major, uint32_t * minor, uint32_t * build, uint32_t * revision);
MCP_PROC_TABLE_API const char * MCP_PROC_TABLE_CALLTYPE MCPGetMocapApiVersionString();

#endif /* END OF _NOITOM_CMOCAPAPI_H */
