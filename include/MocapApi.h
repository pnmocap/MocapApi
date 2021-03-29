#ifndef _NOITOM_MOCAPAPI_H
#define _NOITOM_MOCAPAPI_H

#include <cstdint>
#if defined(_WIN32)

#ifdef MCP_API_EXPORT
#define MCP_INTERFACE extern "C" __declspec( dllexport )
#elif (defined MCP_API_STATIC)
#define MCP_INTERFACE extern "C"
#else
#define MCP_INTERFACE extern "C" __declspec( dllimport )
#endif

#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)

#ifdef MCP_API_EXPORT
#define MCP_INTERFACE extern "C" __attribute__((visibility("default")))
#else
#define MCP_INTERFACE extern "C" 
#endif

#else
#error "Unsupported Platform."
#endif


#if defined( _WIN32 )
#define MCP_CALLTYPE __cdecl
#else
#define MCP_CALLTYPE 
#endif

namespace MocapApi {
    enum EMCPError
    {
        Error_None = 0,
        Error_MoreEvent,
        Error_InsufficientBuffer,
        Error_InvalidObject,
        Error_InvalidHandle,
        Error_InvalidParameter,
        Error_NotSupported,
        Error_IgnoreUDPSetting,
        Error_IgnoreTCPSetting,
        Error_IgnoreBvhSetting,
        Error_JointNotFound,
        Error_WithoutTransformation,
        Error_NoneMessage,
        Error_NoneParent,
        Error_NoneChild,
        Error_AddressInUse,
        Error_ServerNotReady,
    };

    enum EMCPJointTag {
        JointTag_Invalid = -1,
        JointTag_Hips = 0,
        JointTag_RightUpLeg,
        JointTag_RightLeg,
        JointTag_RightFoot,
        JointTag_LeftUpLeg,
        JointTag_LeftLeg,
        JointTag_LeftFoot,
        JointTag_Spine,
        JointTag_Spine1,
        JointTag_Spine2,
        JointTag_Neck,
        JointTag_Neck1,
        JointTag_Head,
        JointTag_RightShoulder,
        JointTag_RightArm,
        JointTag_RightForeArm,
        JointTag_RightHand,
        JointTag_RightHandThumb1,
        JointTag_RightHandThumb2,
        JointTag_RightHandThumb3,
        JointTag_RightInHandIndex,
        JointTag_RightHandIndex1,
        JointTag_RightHandIndex2,
        JointTag_RightHandIndex3,
        JointTag_RightInHandMiddle,
        JointTag_RightHandMiddle1,
        JointTag_RightHandMiddle2,
        JointTag_RightHandMiddle3,
        JointTag_RightInHandRing,
        JointTag_RightHandRing1,
        JointTag_RightHandRing2,
        JointTag_RightHandRing3,
        JointTag_RightInHandPinky,
        JointTag_RightHandPinky1,
        JointTag_RightHandPinky2,
        JointTag_RightHandPinky3,
        JointTag_LeftShoulder,
        JointTag_LeftArm,
        JointTag_LeftForeArm,
        JointTag_LeftHand,
        JointTag_LeftHandThumb1,
        JointTag_LeftHandThumb2,
        JointTag_LeftHandThumb3,
        JointTag_LeftInHandIndex,
        JointTag_LeftHandIndex1,
        JointTag_LeftHandIndex2,
        JointTag_LeftHandIndex3,
        JointTag_LeftInHandMiddle,
        JointTag_LeftHandMiddle1,
        JointTag_LeftHandMiddle2,
        JointTag_LeftHandMiddle3,
        JointTag_LeftInHandRing,
        JointTag_LeftHandRing1,
        JointTag_LeftHandRing2,
        JointTag_LeftHandRing3,
        JointTag_LeftInHandPinky,
        JointTag_LeftHandPinky1,
        JointTag_LeftHandPinky2,
        JointTag_LeftHandPinky3,
        JointTag_Spine3,
        JointTag_JointsCount
    };

    typedef uint64_t MCPRigidBodyHandle_t;
    class IMCPRigidBody
    {
    public:
        virtual EMCPError GetRigidBodyRotaion(float * x, float * y, float * z, float * w, 
            MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;

        virtual EMCPError GetRigidBodyPosition(float * x, float * y, float * z,
            MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;

        virtual EMCPError GetRigidBodyStatus(int * status, 
            MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;

        virtual EMCPError GetRigidBodyId(int * id,
            MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;

        virtual EMCPError GetRigidBodyJointTag(EMCPJointTag * jointTag_, 
            MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
    };
    static const char * IMCPRigidBody_Version = "IMCPRigidBody_001";

    typedef uint64_t MCPSensorModuleHandle_t;
    class IMCPSensorModule 
    {
    public:
        virtual EMCPError GetSensorModulePosture(float * x, float * y, float * z, float * w, 
            MCPSensorModuleHandle_t sensorModuleHandle) = 0;

        virtual EMCPError GetSensorModuleAngularVelocity(float * x, float * y, float * z, 
            MCPSensorModuleHandle_t sensorModuleHandle) = 0;

        virtual EMCPError GetSensorModuleAcceleratedVelocity(float * x, float * y, float * z, 
            MCPSensorModuleHandle_t sensorModuleHandle) = 0;
    };
    static const char * IMCPSensorModule_Version = "IMCPSensorModule_001";

    typedef uint64_t MCPBodyPartHandle_t;
    class IMCPBodyPart 
    {
    public:
        virtual EMCPError GetJointPosition(float * x, float * y, float * z,
            MCPBodyPartHandle_t bodyPartHandle) = 0;

        virtual EMCPError GetJointDisplacementSpeed(float * x, float * y, float * z, 
            MCPBodyPartHandle_t bodyPartHandle) = 0;

        virtual EMCPError GetBodyPartPosture(float * x, float * y, float * z, float * w,
            MCPBodyPartHandle_t bodyPartHandle) = 0;
    };
    static const char * IMCPBodyPart_Version = "IMCPBodyPart_001";

    typedef uint64_t MCPJointHandle_t;
    class IMCPJoint
    {
    public:
        virtual EMCPError GetJointName(const char ** ppStr, 
            MCPJointHandle_t ulJointHandle) = 0;

        virtual EMCPError GetJointLocalRotaion(float * x, float * y, float * z, float * w,
            MCPJointHandle_t ulJointHandle) = 0;

        virtual EMCPError GetJointLocalRotaionByEuler(float * x, float * y, float * z,
            MCPJointHandle_t ulJointHandle) = 0;

        virtual EMCPError GetJointLocalPosition(float * x, float * y, float * z,
            MCPJointHandle_t ulJointHandle) = 0;

        virtual EMCPError GetJointDefaultLocalPosition(float * x, float * y, float * z,
            MCPJointHandle_t ulJointHandle) = 0;

        virtual EMCPError GetJointChild(MCPJointHandle_t * pJointHandle,
            uint32_t * punSizeOfJointHandle,
            MCPJointHandle_t ulJointHandle) = 0;

        virtual EMCPError GetJointBodyPart(MCPBodyPartHandle_t * pBodyPartHandle, 
            MCPJointHandle_t ulJointHandle) = 0;

        virtual EMCPError GetJointSensorModule(MCPSensorModuleHandle_t* pSensorModuleHandle,
            MCPJointHandle_t ulJointHandle) = 0;

        virtual EMCPError GetJointTag(EMCPJointTag * pJointTag, 
            MCPJointHandle_t ulJointHandle) = 0;

        virtual EMCPError GetJointNameByTag(const char ** ppStr,
            EMCPJointTag jointTag) = 0;

        virtual EMCPError GetJointChildJointTag(EMCPJointTag * pJointTag, 
            uint32_t * punSizeOfJointTag,
            EMCPJointTag jointTag) = 0;

        virtual EMCPError GetJointParentJointTag(EMCPJointTag * pJointTag, 
            EMCPJointTag jointTag) = 0;


    };
    static const char * IMCPJoint_Version = "IMCPJoint_003";

    typedef uint64_t MCPAvatarHandle_t;
    class IMCPAvatar
    {
    public:
        virtual EMCPError GetAvatarIndex(uint32_t * index,
            MCPAvatarHandle_t ulAvatarHandle) = 0;

        virtual EMCPError GetAvatarRootJoint(MCPJointHandle_t * pJointHandle,
            MCPAvatarHandle_t ulAvatarHandle) = 0;

        virtual EMCPError GetAvatarJoints(MCPJointHandle_t * pJointHandle, uint32_t * punSizeOfJointHandle,
            MCPAvatarHandle_t ulAvatarHandle) = 0;

        virtual EMCPError GetAvatarJointByName(const char * name, MCPJointHandle_t * pJointHandle,
            MCPAvatarHandle_t ulAvatarHandle) = 0;

        virtual EMCPError GetAvatarName(const char ** ppStr,
            MCPAvatarHandle_t ulAvatarHandle) = 0;

        virtual EMCPError GetAvatarRigidBodies(MCPRigidBodyHandle_t * vRigidBodies, uint32_t * punSizeOfRigidBodies, 
            MCPAvatarHandle_t ulAvatarHandle) = 0;

        virtual EMCPError GetAvatarJointHierarchy(const char ** ppStr) = 0;

        virtual EMCPError GetAvatarPostureIndex(uint32_t * postureIndex, MCPAvatarHandle_t ulAvatarHandle) = 0;

        virtual EMCPError GetAvatarPostureTimeCode(
            uint32_t * hour,  uint32_t * minute,uint32_t * second, uint32_t * frame, uint32_t * rate,
            MCPAvatarHandle_t ulAvatarHandle) = 0;
    };
    static const char * IMCPAvatar_Version = "IMCPAvatar_003";

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
        MCPAvatarHandle_t   avatarHandle;
    };

    struct MCPEvent_SystemError_t
    {
        EMCPError error;
    };

    union MCPEventData_t
    {
        MCPEvent_Reserved_t reserved;

        MCPEvent_MotionData_t motionData;

        MCPEvent_SystemError_t systemError;
    };

    enum EMCPEventType
    {
        MCPEvent_None = 0x00000000,
        MCPEvent_AvatarUpdated = 0x00000100,
        MCPEvent_RigidBodyUpdated = 0x00000200,
        MCPEvent_Error = 0x00000300,
    };

    struct MCPEvent_t
    {
        uint32_t        size;
        EMCPEventType   eventType;
        double          fTimestamp;   // timestamp since software start ( software timestamp ) [1/12/2021 brian.wang]
        MCPEventData_t  eventData;
    };

    enum EMCPBvhRotation
    {
        BvhRotation_XYZ = 0,
        BvhRotation_XZY = 1,
        BvhRotation_YXZ = 2,
        BvhRotation_YZX = 3,
        BvhRotation_ZXY = 4,
        BvhRotation_ZYX = 5,
    };

    enum EMCPBvhData
    {
        BvhDataType_String = 0,
        BvhDataType_BinaryWithOldFrameHeader = 1,
        BvhDataType_Binary = 2,
        BvhDataType_Mask_LegacyHumanHierarchy = 4,
    };

    enum EMCPBvhTransformation
    {
        BvhTransformation_Disable = 0,
        BvhTransformation_Enable = 1,
    };

    typedef uint64_t MCPSettingsHandle_t;
    class IMCPSettings
    {
    public:
        virtual EMCPError CreateSettings(MCPSettingsHandle_t * pSettingsHandle) = 0;

        virtual EMCPError DestroySettings(MCPSettingsHandle_t ulSettingsHandle) = 0;

        virtual EMCPError SetSettingsUDP(uint16_t localPort,
            MCPSettingsHandle_t ulSettingsHandle) = 0;

        virtual EMCPError SetSettingsTCP(const char * serverIp, uint16_t serverPort,
            MCPSettingsHandle_t ulSettingsHandle) = 0;

        virtual EMCPError SetSettingsBvhRotation(EMCPBvhRotation bvhRotation,
            MCPSettingsHandle_t ulSettingsHandle) = 0;

        virtual EMCPError SetSettingsBvhTransformation(EMCPBvhTransformation bvhTransformation,
            MCPSettingsHandle_t ulSettingsHandle) = 0;

        virtual EMCPError SetSettingsBvhData(EMCPBvhData bvhData,
            MCPSettingsHandle_t ulSettingsHandle) = 0;

        virtual EMCPError SetSettingsCalcData(MCPSettingsHandle_t ulSettingsHandle) = 0;
    };
    static const char * IMCPSettings_Version = "IMCPSettings_001";

    enum EMCPUpVector {
        UpVector_XAxis = 1,
        UpVector_YAxis = 2,
        UpVector_ZAxis = 3
    };
    enum EMCPFrontVector {
        FrontVector_ParityEven = 1, 
        FrontVector_ParityOdd = 2
    };
    enum EMCPCoordSystem {
        CoordSystem_RightHanded,
        CoordSystem_LeftHanded
    };
    enum EMCPRotatingDirection {
        RotatingDirection_Clockwise,
        RotatingDirection_CounterClockwise,
    };
    enum EMCPPreDefinedRenderSettings {
        PreDefinedRenderSettings_Default,
        PreDefinedRenderSettings_UnrealEngine,
        PreDefinedRenderSettings_Unity3D,
        PreDefinedRenderSettings_Count,
    };
    enum EMCPUnit 
    {
        Unit_Centimeter,
        Uint_Meter,
    };

    typedef uint64_t MCPRenderSettingsHandle_t;
    class IMCPRenderSettings
    {
    public:
        virtual EMCPError CreateRenderSettings(MCPRenderSettingsHandle_t * pRenderSettings) = 0;

        virtual EMCPError GetPreDefRenderSettings(EMCPPreDefinedRenderSettings preDefinedRenderSettings,
            MCPRenderSettingsHandle_t * pRenderSettings) = 0;

        virtual EMCPError SetUpVector(EMCPUpVector upVector, int sign,
            MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetUpVector(EMCPUpVector * pUpVector, int * sign,
            MCPRenderSettingsHandle_t renderSettings) = 0;

        virtual EMCPError SetFrontVector(EMCPFrontVector frontVector, int sign,
            MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetFrontVector(EMCPFrontVector * pFrontVector, int * sign,
            MCPRenderSettingsHandle_t renderSettings) = 0;

        virtual EMCPError SetCoordSystem(EMCPCoordSystem coordSystem,
            MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetCoordSystem(EMCPCoordSystem * pCoordSystem,
            MCPRenderSettingsHandle_t renderSettings) = 0;

        virtual EMCPError SetRotatingDirection(EMCPRotatingDirection rotatingDirection,
            MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetRotatingDirection(EMCPRotatingDirection * pRotatingDirection,
            MCPRenderSettingsHandle_t renderSettings) = 0;

        virtual EMCPError SetUnit(EMCPUnit mcpUnit, 
            MCPRenderSettingsHandle_t renderSettings) = 0;
        virtual EMCPError GetUnit(EMCPUnit * mcpUnit, 
            MCPRenderSettingsHandle_t renderSettings) = 0;

        virtual EMCPError DestroyRenderSettings(MCPRenderSettingsHandle_t renderSettings) = 0;
    };
    static const char * IMCPRenderSettings_Version = "IMCPRenderSettings_001";

    typedef uint64_t MCPApplicationHandle_t;
    class IMCPApplication
    {
    public:

        virtual EMCPError CreateApplication(MCPApplicationHandle_t * ulApplicationHandle) = 0;

        virtual EMCPError DestroyApplication(MCPApplicationHandle_t ulApplicationHandle) = 0;

        virtual EMCPError SetApplicationSettings(MCPSettingsHandle_t ulSettingsHandle,
            MCPApplicationHandle_t ulApplicationHandle) = 0;

        virtual EMCPError SetApplicationRenderSettings(MCPRenderSettingsHandle_t ulRenderSettings,
            MCPApplicationHandle_t ulApplicationHandle) = 0;

        virtual EMCPError OpenApplication(MCPApplicationHandle_t ulApplicationHandle) = 0;

        virtual EMCPError EnableApplicationCacheEvents(MCPApplicationHandle_t ulApplicationHandle) = 0;

        virtual EMCPError DisableApplicationCacheEvents(MCPApplicationHandle_t ulApplicationHandle) = 0;

        virtual EMCPError ApplicationCacheEventsIsEnabled(bool * isEnabled, 
            MCPApplicationHandle_t ulApplicationHandle) = 0;

        virtual EMCPError CloseApplication(MCPApplicationHandle_t ulApplicationHandle) = 0;

        virtual EMCPError GetApplicationRigidBodies(
            MCPRigidBodyHandle_t * pRigidBodyHandle,    /*[in, out, optional] */
            uint32_t * punRigidBodyHandleSize,        /*[in, out]*/
            MCPApplicationHandle_t ulApplicationHandle
        ) = 0;

        virtual EMCPError GetApplicationAvatars(
            MCPAvatarHandle_t * pAvatarHandle,  /*[in, out, optional] */
            uint32_t * punAvatarHandle,         /*[in, out]*/
            MCPApplicationHandle_t ulApplicationHandle
        ) = 0;

        virtual EMCPError PollApplicationNextEvent(
            MCPEvent_t * pEvent /* [in, out,  optional]*/,
            uint32_t * punSizeOfEvent, /* [in, out] */
            MCPApplicationHandle_t ulApplicationHandle
        ) = 0;
    };
    static const char * const IMCPApplication_Version = "IMCPApplication_002";

    MCP_INTERFACE EMCPError MCP_CALLTYPE MCPGetGenericInterface(const char * pchInterfaceVersion,
        void ** ppInterface);
}
#endif  // end _NOITOM_MOCAPAPI_H [10/30/2020 brian.wang]