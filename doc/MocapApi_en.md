# Overview

The MocapApi API is the next generation programming interface of the NeuronDataReader (hereafter referred to as NDR). From the get-go, the goal has been to have an API with cross-platform compatibility (Win/Mac/Android/Ios/Linux, etc.), cross-engine access (Unity/unreal), and user-free updates. Today, MocapAPI supports C, C++, and C#, Windows, u3d and Unreal Engine.

The MocapAPI API receives outgoing socket data from AxisStudio, Pnlab and other software.

# **Naming rules and calling conventions**

## **Naming rules**

MocapApi has two particularly important data types: the interface data type, generally referred to in the naming convention of `IMCpxxx` and the handle data type, generally named as `MCPXxxHandle_t`. Other data types follow the Hungarian nomenclature.

The `MCPXxxHandle_t` handle type is the entity index of the object, responsible for organizing and managing data, but not granting data access. The `IMCPXxx` interface type is responsible for accessing data according to the `MCPXxxHandle_t` handle. Typically, the `IMCPXxx` type and the `MCPXxxHandle_t` type are used in conjunction.

# Differences in Languages

## Some differences in different languages+

- C ++ uses MocapApi.h, C uses MocapCApi.h, and C# uses MocapApi.cs
- `IMCPXxx` named interfaces are in MocapApi.h, C++ will use this header file. The corresponding `MCPXxx_ProcTable` structure is used by C, including the MocapCApi.h header file. The representation is a structure combining n function pointers. C# uses MocapApi.cs and directly uses `IMCpxxx`.
- The handle type `MCPXxxHandle_t` exists in C and C++, but not in C#. There is no current replacement for it in C#.

From now on, the "Detailed Description of Types" section uses C++ as the main description language. Please refer to this section to convert structures to the corresponding language.

**The following "class specification" uses C++ as the description language.** Refer to this section to describe the conversion to the appropriate programming language.

## **The interface type**

The interface type in the form of `IMCpxxx` cannot be created directly, but it can be obtained at any time as needed. The acquisition methods of C and C++ are similar, and the acquisition method of C# is acquired through static functions.

## C and C++ acquisition methods

Since the interface type in the form of `ICMPXxx` cannot be created directly, it must be accessed through `MCPGetGenericInterface`. To achieve this, you must declare `MCPGetGenericInterface`.
```
MCP_INTERFACE EMCPError MCP_CALLTYPE MCPGetGenericInterface(const char * pchInterfaceVersion, void ** ppInterface);
```
- `pchInterfaceVersion`: is a null-terminated character string, using `IMCpxxx`_Version in C++ is fine as well.
- `ppInterface`: In C++, a pointer needs to be passed to the `IMCpxxx` pointer.

Correspondingly, use the `MCPGetProcessTable` interface in C to obtain the corresponding type, and its implementation is as follows:
```c++
static EMCPError MCPGetProcessTable(const char * pchInterfaceVersion, void ** ppInterface)
{
    char table[128] = "PROC_TABLE:";
    return MCPGetGenericInterface(strcat(table, pchInterfaceVersion), ppInterface);
}
```
- `pchInterfaceVersion`: similar to use as in C++.
- `ppInterface`: different from C++. Here you need to pass a pointer to the `MCPXXX_ProcTable` pointer.

## C# methods

Implementation in C# is easier. Simply call `IMCpxxx.Xxx()` directly.

# **Memory management**

MocapApi follows the principle of whomever allocates memory needs to release it, that is, once the user calls the function `CreateXxxxIMCpxxx`, it needs to call the corresponding `DestroyXxx` function at the right time. The memory allocated by `IMCPXxx` and `GetXxx` functions is released by MocapApi itself.

# **Error handling**

There are two kinds of errors in MocapApi. One is a function call error, such as an empty buffer or a parameter error. The other is an error during program execution, such as a communication error with AxisStudio or Pnlab. Each function in MocapApi will return a `EMCPError` error code, users can judge according to their actual situation and error. Errors during program execution will be introduced in the Event mechanism.

#  Logic Flow Chart


![][1]

# Detailed Description of types

## IMCPApplication & MCPApplicationHandle_t

An Application corresponds to a broadcast output port in Axis Studio / Axis Lab. The Application encapsulates a collection of functions used to process data from Axis Studio / Axis Lab by calling `PollApplicationNextEvent`, polling the latest Axis Studio messages and the status information of the Application itself.

You may obtain it using the following `IMCPApplication` pointer:

```c++
MocapApi::IMCPApplication * mcpApplication = nullptr;
MocapApi::MCPGetGenericInterface(MocapApi::IMCPApplication_Version, 
        reinterpret_cast<void **>(&mcpApplication)
```
&You need to use `CreateApplication` to create an Application entity

The reference code is as follows:

```c++
MocapApi::MCPApplicationHandle_t mcpApplicationHandle = 0;
mcpApplication->CreateApplication(&mcpApplicationHandle);
```
The following code can be used when an Application is no longer needed:
```c++
mcpApplication->DestroyApplication(mcpApplicationHandle);
```

 ### CreateApplication
 ```c++
 virtual EMCPError CreateApplication(MCPApplicationHandle_t * ulApplicationHandle) = 0;
 ```
Create an Application entity. When the entity is no longer needed, you need to manually call `DestroyApplication`.

`ulApplicationHandle` returns the handle of the Application entity.

### DestroyApplication
```c++
virtual EMCPError DestroyApplication(MCPApplicationHandle_t ulApplicationHandle) = 0;
```
After destroying an Application entity, the entity represented by `ulApplicationHandle` can no longer be used.

### SetApplicationSettings
```c++
virtual EMCPError SetApplicationSettings(MCPSettingsHandle_t ulSettingsHandle, MCPApplicationHandle_t ulApplicationHandle) = 0;
```

### SetApplicationRenderSettings
```c++
virtual EMCPError SetApplicationRenderSettings(MCPRenderSettingsHandle_t ulRenderSettings, MCPApplicationHandle_t ulApplicationHandle) = 0;
```

### OpenApplication
```c++
virtual EMCPError OpenApplication(MCPApplicationHandle_t ulApplicationHandle) = 0;
```

### CloseApplication
```c++
virtual EMCPError CloseApplication(MCPApplicationHandle_t ulApplicationHandle) = 0;
```
### GetApplicationRigidBodies
```c++
virtual EMCPError GetApplicationRigidBodies(
            MCPRigidBodyHandle_t * pRigidBodyHandle,    /*[in, out, optional] */
            uint32_t * punRigidBodyHandleSize,        /*[in, out]*/
            MCPApplicationHandle_t ulApplicationHandle
        ) = 0;
```

### GetApplicationAvatars
```c++
virtual EMCPError GetApplicationAvatars(
            MCPAvatarHandle_t * pAvatarHandle,  /*[in, out, optional] */
            uint32_t * punAvatarHandle,         /*[in, out]*/
            MCPApplicationHandle_t ulApplicationHandle
        ) = 0;
```
The reference call code is as follows: (other similar function call mechanisms work in a similar fashion)
```c++
numberOfAvatars = 0;
error = application->GetApplicationAvatars(nullptr, &numberOfAvatars, applicationHandle);
if(error == Error_None){
    avatars = /*allocate memory*/
    error = application->GetApplicationAvatars(avatars, &numberOfAvatars, applicationHandle);
}
```

### PollApplicationNextEvent

```c++
virtual EMCPError PollApplicationNextEvent(
            MCPEvent_t * pEvent /* [in, out,  optional]*/,
            uint32_t * unEvent, /* [in, out] */
            MCPApplicationHandle_t ulApplicationHandle
        ) = 0;
```
The size of each ` MCPEvent_t` in `pEvent` needs to be assigned:
```
MocapApi::MCPEvent_t pEvent[1];
pEvent[0].size = sizeof(MocapApi::MCPEvent_t);
```

If you do not use cache mode parameters, then call the code in the following way:
```c++
do {
    sizeEvent = 0;
    error = application->PollApplicationNextEvent(nullptr, &sizeEvent, applicationHandle);
    if(error != Error_None){
        break;  // handle error
    }
    events = /*allocate memory for sizeEvent*event */
    error = application->PollApplicationNextEvent(events, &sizeEvent, applicationHandle);
} while(error != Error_InsufficientBuffer)
if(error != Error_None){
    // handle error
}
```

### GetApplicationSensorModules

```c++
virtual EMCPError GetApplicationSensorModules(
            MCPSensorModuleHandle_t * pSensorModuleHandle,  /*[in, out, optional] */
            uint32_t * punSensorModuleHandle,         /*[in, out]*/
            MCPApplicationHandle_t ulApplicationHandle
        ) = 0;
```

### GetApplicationTrackers
```c++
virtual EMCPError GetApplicationTrackers(
			MCPTrackerHandle_t* pTrackerHandle,  /*[in, out, optional] */
			uint32_t* punTrackerHandle,         /*[in, out]*/
			MCPApplicationHandle_t ulApplicationHandle
		) = 0;
```

### QueuedServerCommand

Send command to Server, it will not delete the command. If after calling this function, call ```DestroyCommand``` to delete the ```cmdHandle``` object, the Server will return the command execution result with no Event  

```
virtual EMCPError QueuedServerCommand(MCPCommandHandle_t cmdHandle,   /*[in]*/
            MCPApplicationHandle_t ulApplicationHandle) = 0;
```

## IMCPAvatar & MCPAvatarHandle_t

The Avatar entity corresponds to the Avatar in AxisStudio, so it will only be available when connected to Axis Studio. The hierarchy of the Avatar can be obtained through `GetAvatarRootJoint` and the related functions of the `Joint` object.

You may use the following code to get the pointer of `IMCPAvatar`:

```c++
MocapApi::IMCPAvatar * mcpAvatar = nullptr;
MocapApi::MCPGetGenericInterface(MocapApi::IMCPAvatar_Version, 
        reinterpret_cast<void **>(&mcpAvatar)
```

```c++
typedef uint64_t MCPAvatarHandle_t;
```
### GetAvatarIndex
```c++
virtual EMCPError GetAvatarIndex(uint32_t * index,
            MCPAvatarHandle_t ulAvatarHandle) = 0;
```
### GetAvatarRootJoint
&emsp;Joint is the skeleton node of Avatar, only when receiving BVH data.
```c++
virtual EMCPError GetAvatarRootJoint(MCPJointHandle_t * pJointHandle,
            MCPAvatarHandle_t ulAvatarHandle) = 0;
```
### GetAvatarJoints
```c++
virtual EMCPError GetAvatarJoints(MCPJointHandle_t * pJointHandle, uint32_t * punJointHandle,
            MCPAvatarHandle_t ulAvatarHandle) = 0;
```

### GetAvatarJointByName
```c++
virtual EMCPError GetAvatarJointByName(const char * name, MCPJointHandle_t * pJointHandle,
            MCPAvatarHandle_t ulAvatarHandle) = 0;
```
### GetAvatarName
```c++
virtual EMCPError GetAvatarName(const char **,
            MCPAvatarHandle_t ulAvatarHandle) = 0;
```
### GetAvatarRigidBodies
RigidBody is a rigid body bound to the Avatar. This data is only available when receiving BVH data and when there is light mixing.
```c++
virtual EMCPError GetAvatarRigidBodies(MCPRigidBodyHandle_t * pRigidBodies, uint32_t * punRigidBodies, 
            MCPAvatarHandle_t ulAvatarHandle) = 0;
```

## IMCPJoint && MCPJointHandle_t
Use the following code to get the pointer of `IMCPJoint`:
```c++
MocapApi::IMCPJoint * mcpJoint = nullptr;
MocapApi::MCPGetGenericInterface(MocapApi::IMCPJoint_Version, 
        reinterpret_cast<void **>(&mcpJoint)
```
```c++
typedef uint64_t MCPJointHandle_t;
```
### GetJointName
```c++
virtual EMCPError GetJointName(const char **, 
        MCPJointHandle_t ulJointHandle) = 0;
```
### GetJointLocalRotaion
```c++
virtual EMCPError GetJointLocalRotaion(float * x, float * y, float * z, float * w,
        MCPJointHandle_t ulJointHandle) = 0;
```
### GetJointLocalRotaionByEuler
```c++
virtual EMCPError GetJointLocalRotaionByEuler(float * x, float * y, float * z,
        MCPJointHandle_t ulJointHandle) = 0;
```
### GetJointLocalTransformation
```c++
virtual EMCPError GetJointLocalTransformation(float * x, float * y, float * z,
        MCPJointHandle_t ulJointHandle) = 0;
```
### GetJointDefaultLocalTransformation
```c++
virtual EMCPError GetJointDefaultLocalTransformation(float * x, float * y, float * z,
        MCPJointHandle_t ulJointHandle) = 0;
```
### GetJointChild
```c++
virtual EMCPError GetJointChild(MCPJointHandle_t * pJointHandle,
        uint32_t * punJointHandle,
        MCPJointHandle_t ulJointHandle) = 0;
```
### GetJointBodyPart
`JointBodyPart` is a description of the Calc data and is only available when the Calc data is received.

```c++
virtual EMCPError GetJointBodyPart(MCPBodyPartHandle_t * pBodyPartHandle, 
        MCPJointHandle_t ulJointHandle) = 0;
```
### GetJointSensorModule
`SensorModule` is the description of the Calc data and is only available when the Calc data is received.

```c++
virtual EMCPError GetJointSensorModule(MCPSensorModuleHandle_t* pSensorModuleHandle,
        MCPJointHandle_t ulJointHandle) = 0;
```

## IMCPRigidBody & MCPRigidBodyHandle_t
A pointer to the `IMCPRigidBody` can be obtained using the following reference code:
```c++
MocapApi::IMCPRigidBody * mcpRigidBody = nullptr;
MocapApi::MCPGetGenericInterface(MocapApi::IMCPRigidBody_Version, 
        reinterpret_cast<void **>(&mcpRigidBody)
```

```c++
typedef uint64_t MCPRigidBodyHandle_t;
```
### GetRigidBodyRotaion
```c++
virtual EMCPError GetRigidBodyRotaion(float * x, float * y, float * z, float * w, 
        MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
```
### GetRigidBodyTransformation
```c++
virtual EMCPError GetRigidBodyTransformation(float * x, float * y, float * z,
        MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
```
### GetRigidBodieStatus
```c++
virtual EMCPError GetRigidBodieStatus(int * status, 
        MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
```
### GetRigidBodyId
```c++
virtual EMCPError GetRigidBodyId(int * id,
        MCPRigidBodyHandle_t ulRigidBodyHandle) = 0;
```
## IMCPSensorModule & MCPSensorModuleHandle_t
A pointer to the `IMCPSensorModule` can be obtained using the following code:
```c++
MocapApi::IMCPSensorModule * mcpSensorModule = nullptr;
MocapApi::MCPGetGenericInterface(MocapApi::IMCPSensorModule_Version, 
        reinterpret_cast<void **>(&mcpSensorModule)
```
```c++
typedef uint64_t MCPSensorModuleHandle_t;
```
### GetSensorModulePosture
```c++
virtual EMCPError GetSensorModulePosture(float * x, float * y, float * z, float * w, 
        MCPSensorModuleHandle_t sensorModuleHandle) = 0;
```
&emsp;To get the Quaternion of Pose.
### GetSensorModuleAngularVelocity
```c++
virtual EMCPError GetSensorModuleAngularVelocity(float * x, float * y, float * z, 
        MCPSensorModuleHandle_t sensorModuleHandle) = 0;
```
### GetSensorModuleAcceleratedVelocity
```c++
virtual EMCPError GetSensorModuleAcceleratedVelocity(float * x, float * y, float * z, 
        MCPSensorModuleHandle_t sensorModuleHandle) = 0;
```

### GetSensorModuleId
```c++
virtual EMCPError GetSensorModuleId(uint32_t * id,
        MCPSensorModuleHandle_t sensorModuleHandle) = 0;
```

### GetSensorModuleCompassValue
```c++
virtual EMCPError GetSensorModuleCompassValue(float * x, float * y, float * z,
    MCPSensorModuleHandle_t sensorModuleHandle) = 0;
```
### GetSensorModuleTemperature
```c++
virtual EMCPError GetSensorModuleTemperature(float * temperature, 
        MCPSensorModuleHandle_t sensorModuleHandle) = 0;
```

## IMCPBodyPart & MCPBodyPartHandle_t
&emsp;A pointer to the `IMCPBodyPart` can be obtained using the following code.
```c++
MocapApi::IMCPBodyPart * mcpBodyPart = nullptr;
MocapApi::MCPGetGenericInterface(MocapApi::IMCPBodyPart_Version, 
        reinterpret_cast<void **>(&mcpBodyPart)
```
```c++
typedef uint64_t MCPBodyPartHandle_t;
```
### GetJointPosition
```c++
virtual EMCPError GetJointPosition(float * x, float * y, float * z,
        MCPBodyPartHandle_t bodyPartHandle) = 0;
```
### GetJointDisplacementSpeed
```c++
virtual EMCPError GetJointDisplacementSpeed(float * x, float * y, float * z, 
        MCPBodyPartHandle_t bodyPartHandle) = 0;
```
### GetBodyPartPosture
```c++
virtual EMCPError GetBodyPartPosture(float * x, float * y, float * z, float * w,
        MCPBodyPartHandle_t bodyPartHandle) = 0;
```
## IMCPTracker& MCPTrackerHandle_t

***Tracker*** entity is corresponding to the **Device** in **Alice/AHM**, it is only available after connected to the **Alice/AHM**.

A pointer to `IMCPTracker` can be obtained using the following code. 

```c++
MocapApi::IMCPTracker * mcpTracker = nullptr;
MocapApi::MCPGetGenericInterface(MocapApi::IMCPTracker_Version, 
        reinterpret_cast<void **>(&mcpTracker)
```

```c++
typedef uint64_t MCPTrackerHandle_t;
```

### GetDeviceCount

```c++
virtual EMCPError GetDeviceCount(int* devCount,
            MCPTrackerHandle_t ulTrackerHandle) = 0;
```

### GetDeviceName

&emsp;On the basis of obtaining `DeviceCount`, iterating from 0 to `DeviceCount` as `serialNum` to obtain `DeviceName`, and obtaining subsequent related data using `DeviceName` as key.

```c++
virtual EMCPError GetDeviceName(int serialNum, const char** name,
            MCPTrackerHandle_t ulTrackerHandle) = 0;
```

### GetTrackerRotation

```c++
virtual EMCPError GetTrackerRotation(float* x, float* y, float* z, float* w, const char* deviceName,
            MCPTrackerHandle_t ulTrackerHandle) = 0;
```

### GetTrackerPosition

```c++
virtual EMCPError GetTrackerPosition(float* x, float* y, float* z, const char* deviceName,
            MCPTrackerHandle_t ulTrackerHandle) = 0;
```

### GetTrackerEulerAng

```c++
virtual EMCPError GetTrackerEulerAng(float* x, float* y, float* z, const char* deviceName,
            MCPTrackerHandle_t ulTrackerHandle) = 0;
```

## IMCPRenderSettings & MCPRenderSettingsHandle_t

A pointer to `IMCPRenderSettings` can be obtained using the following code.
```c++
MocapApi::IMCPRenderSettings * mcpRenderSettings = nullptr;
MocapApi::MCPGetGenericInterface(MocapApi::IMCPRenderSettings_Version, 
        reinterpret_cast<void **>(&mcpRenderSettings)
```
You need to create a `RenderSettings` entity using `CreateRenderSettings`, the reference code is as follows:
```c++
MocapApi::MCPRenderSettingsHandle_t mcpRenderSettingsHandle = 0;
mcpRenderSettings->CreateRenderSettings(&mcpRenderSettingsHandle);
```
When a `RenderSettings` is no longer needed the following code can be used:
```c++
mcpRenderSettings->DestroyRenderSettings(mcpRenderSettingsHandle);
```
You can also use `GetPreDefRenderSettings` to get some predefined RenderSettings entities, but not to destroy them using `DestroyRenderSettings`.

### CreateRenderSettings
```c++
 virtual EMCPError CreateRenderSettings(MCPRenderSettingsHandle_t * pRenderSettings) = 0;
```
Create a `RenderSettings` entity and manually call `DestroyRenderSettings` when the entity is no longer needed. `pRenderSettings` returns a handle to the `RenderSettings` entity.

### GetPreDefRenderSettings
```c++
virtual EMCPError GetPreDefRenderSettings(EMCPPreDefinedRenderSettings preDefinedRenderSettings,
        MCPRenderSettingsHandle_t * pRenderSettings) = 0;
```
### SetUpVector
```c++
virtual EMCPError SetUpVector(EMCPUpVector upVector, int sign,
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### GetUpVector
```c++
virtual EMCPError GetUpVector(EMCPUpVector * pUpVector, int * sign,
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### SetFrontVector
```c++
virtual EMCPError SetFrontVector(EMCPFrontVector frontVector, int sign,
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### GetFrontVector
```c++
virtual EMCPError GetFrontVector(EMCPFrontVector * pFrontVector, int * sign,
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### SetCoordSystem
```c++
virtual EMCPError SetCoordSystem(EMCPCoordSystem coordSystem,
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### GetCoordSystem
```c++
virtual EMCPError GetCoordSystem(EMCPCoordSystem * pCoordSystem,
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### SetRotatingDirection
```c++
virtual EMCPError SetRotatingDirection(EMCPRotatingDirection rotatingDirection,
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### GetRotatingDirection
```c++
virtual EMCPError GetRotatingDirection(EMCPRotatingDirection * pRotatingDirection,
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### SetUnit
```c++
virtual EMCPError SetUnit(EMCPUnit mcpUnit, 
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### GetUnit
```c++
virtual EMCPError GetUnit(EMCPUnit * mcpUnit, 
        MCPRenderSettingsHandle_t renderSettings) = 0;
```
### DestroyRenderSettings
```c++
virtual EMCPError DestroyRenderSettings(MCPRenderSettingsHandle_t renderSettings) = 0;
```
After destroying a `RenderSettings` entity, the entity represented by renderSettings is no longer available.

## IMCPSettings && MCPSettingsHandle_t
Use the following code to get a pointer to `IMCPSettings`:
```c++
MocapApi::IMCPSettings * mcpSettings = nullptr;
MocapApi::MCPGetGenericInterface(MocapApi::IMCPSettings_Version, 
        reinterpret_cast<void **>(&mcpSettings)
```
```c++
typedef uint64_t MCPSettingsHandle_t;
```
A Settings entity needs to be created using `CreateSettings`, with the following reference code:
```c++
MocapApi::MCPSettingsHandle_t mcpSettingsHandle = 0;
mcpSettings->CreateSettings(&mcpSettingsHandle);
```
Once the Settings entity is no longer needed, you need to destroy it. You can use the following reference code:
```c++
mcpSettings->DestroySettings(mcpSettingsHandle);
```

### CreateSettings
```c++
virtual EMCPError CreateSettings(MCPSettingsHandle_t * pSettingsHandle) = 0;
```
Create a Settings entity and manually call `DestroySettings` when the entity is no longer needed `pSettingsHandle` returns a handle to the Settings entity.
### DestroySettings
```c++
virtual EMCPError DestroySettings(MCPSettingsHandle_t ulSettingsHandle) = 0;
```
After destroying a Settings entity, the entity represented by `ulSettingsSettings` is no longer available.

### SetSettingsUDP
```c++
virtual EMCPError SetSettingsUDP(uint16_t localPort,
        MCPSettingsHandle_t ulSettingsHandle) = 0;
```
### SetSettingsUDPServer

&emsp;When connect with UDP protocol, set the IP and Port of Server:

```c++
virtual EMCPError SetSettingsUDPServer(const char* serverIp, uint16_t serverPort,
			MCPSettingsHandle_t ulSettingsHandle) = 0;
```

### SetSettingsTCP

```c++
virtual EMCPError SetSettingsTCP(const char * serverIp, uint16_t serverPort,
        MCPSettingsHandle_t ulSettingsHandle) = 0;
```
### SetSettingsBvhRotation
```c++
virtual EMCPError SetSettingsBvhRotation(EMCPBvhRotation bvhRotation,
        MCPSettingsHandle_t ulSettingsHandle) = 0;
```
### SetSettingsBvhTransformation
```c++
virtual EMCPError SetSettingsBvhTransformation(EMCPBvhTransformation bvhTransformation,
        MCPSettingsHandle_t ulSettingsHandle) = 0;
```
### SetSettingsBvhData
```c++
virtual EMCPError SetSettingsBvhData(EMCPBvhData bvhData,
            MCPSettingsHandle_t ulSettingsHandle) = 0;
```
### SetSettingsCalcData
```c++
virtual EMCPError SetSettingsCalcData(MCPSettingsHandle_t ulSettingsHandle) = 0;
```

## IMCPCommand

MocapApi Command interface,  it will be used to send command to the Server and receive the command reply from the Server. The command handle is  `MCPCommandHandle_t`.

### CreateCommand

  Create a command object based on the command flag.

```c++
 virtual EMCPError CreateCommand(uint32_t cmd, MCPCommandHandle_t* handle_) = 0;
```

### SetCommandExtraFlags

Set additional command flags

```c++
virtual EMCPError SetCommandExtraFlags(uint32_t extraFlags, MCPCommandHandle_t handle_) = 0;
```

For example: do `StopCapture`:

```c++
commandInterface->CreateCommand(CommandStopCapture, &command);
commandInterface->SetCommandExtraFlags(StopCatpureExtraFlag_SensorsModulesHibernate, command);
```

### SetCommandExtraLong

Set command parameters

```c++
virtual EMCPError SetCommandExtraLong(uint32_t extraLongIndex, intptr_t extraLong, 
            MCPCommandHandle_t handle_) = 0;
```

For example: do `StartCapture`:

```
commandInterface->CreateCommand(CommandStartCapture, &command);
commandInterface->SetCommandExtraLong(CommandExtraLong_DeviceRadio, 2471, command);
```

### GetCommandResultMessage

Gets the error message returned by the Server

```c++
virtual EMCPError GetCommandResultMessage(const char ** pMsg, MCPCommandHandle_t handle_) = 0;
```

### GetCommandResultCode

Gets the error code returned by the Server

```c++
virtual EMCPError GetCommandResultCode(uint32_t *pResCode, MCPCommandHandle_t handle_) = 0;
```

### GetCommandProgress

Get the command execution progress returned by the Server

```c++
virtual EMCPError GetCommandProgress(uint32_t progress, intptr_t extra, MCPCommandHandle_t handle_) =0;
```

### DestroyCommand

```c++
virtual EMCPError DestroyCommand(MCPCommandHandle_t handle_) = 0;
```

## IMCPCalibrateMotionProgress

The calibration progress interface, the corresponding Handle is ```MCPCalibrateMotionProgressHandle_t```. It can be obtained by `IMCPCommand::GetCommandProgress(CommandProgress_CalibrateMotion, ...)`

### GetCalibrateMotionProgressCountOfSupportPoses

The number of calibration positions required for this calibration.

```c++
virtual EMCPError GetCalibrateMotionProgressCountOfSupportPoses(uint32_t * pCount, 
            MCPCalibrateMotionProgressHandle_t handle_) = 0;
```

### GetCalibrateMotionProgressNameOfSupportPose

Gets the calibration pose name for this calibration.

```c++
virtual EMCPError GetCalibrateMotionProgressNameOfSupportPose(char* name, uint32_t* pLenOfName, uint32_t index, MCPCalibrateMotionProgressHandle_t handle_) = 0;
```

### GetCalibrateMotionProgressStepOfPose

Gets the calibration phase of the specified calibration position. 

```c++
virtual EMCPError GetCalibrateMotionProgressStepOfPose(uint32_t* pStep, 
        const char*name, MCPCalibrateMotionProgressHandle_t handle_) = 0;
```

### GetCalibrateMotionProgressCountdownOfPose

Gets the countdown to the specified calibration position.

```c++
virtual EMCPError GetCalibrateMotionProgressCountdownOfPose(uint32_t* pCountdown, 
        const char*name, MCPCalibrateMotionProgressHandle_t handle_) = 0;
```

### GetCalibrateMotionProgressProgressOfPose

Gets the calibration progress of the specified calibration position.

```c++
virtual EMCPError GetCalibrateMotionProgressProgressOfPose(uint32_t* pProgress, 
        const char*name, MCPCalibrateMotionProgressHandle_t handle_) = 0;
```

### GetCalibrateMotionProgressStepOfCurrentPose

Gets the current calibration position and calibration phase

```c++
virtual EMCPError GetCalibrateMotionProgressStepOfCurrentPose(uint32_t * pStep, 
        char* name, uint32_t * pLenOfName, MCPCalibrateMotionProgressHandle_t handle_) = 0;
```

### GetCalibrateMotionProgressCountdownOfCurrentPose

Gets the current calibration position and calibration countdown.

```c++
virtual EMCPError GetCalibrateMotionProgressCountdownOfCurrentPose(uint32_t* pCountdown, 
        char* name, uint32_t* pLenOfName, MCPCalibrateMotionProgressHandle_t handle_) = 0;
```

### GetCalibrateMotionProgressProgressOfCurrentPose

Get the current calibration execution and calibration progress.

```c++
virtual EMCPError GetCalibrateMotionProgressProgressOfCurrentPose(uint32_t* pProgress, 
        char* name, uint32_t* pLenOfName, MCPCalibrateMotionProgressHandle_t handle_) = 0;
```

## EMCPEventType

```c++
enum EMCPEventType
{
        MCPEvent_None,
        MCPEvent_AvatarUpdated,
        MCPEvent_RigidBodyUpdated,
        MCPEvent_Error,
        MCPEvent_SensorModulesUpdated,
        MCPEvent_TrackerUpdated,
        MCPEvent_CommandReply,
};
```

## EMCPBvhRotation
```C++
enum EMCPBvhRotation
{
        BvhRotation_XYZ,
        BvhRotation_XZY,
        BvhRotation_YXZ,
        BvhRotation_YZX,
        BvhRotation_ZXY,
        BvhRotation_ZYX,
};
```

## EMCPBvhData
```c++
enum EMCPBvhData
{
        BvhDataType_String,
        BvhDataType_BinaryWithOldFrameHeader,
        BvhDataType_Binary,
        BvhDataType_Mask_LegacyHumanHierarchy,
};
```

## EMCPBvhTransformation
```c++
enum EMCPBvhTransformation
{
        BvhTransformation_Disable,
        BvhTransformation_Enable,
};
```

## EMCPUpVector
```c++
enum EMCPUpVector 
{
        UpVector_XAxis = 1,
        UpVector_YAxis = 2,
        UpVector_ZAxis = 3
};
```
## EMCPFrontVector
```c++
enum EMCPFrontVector 
{
        FrontVector_ParityEven = 1, 
        FrontVector_ParityOdd = 2
};
```
## EMCPCoordSystem
```c++
enum EMCPCoordSystem 
{
        CoordSystem_RightHanded,
        CoordSystem_LeftHanded
};
```

## EMCPRotatingDirection
```c++
enum EMCPRotatingDirection 
{
        RotatingDirection_Clockwise,
        RotatingDirection_CounterClockwise,
};
```
## EMCPPreDefinedRenderSettings
```c++
enum EMCPPreDefinedRenderSettings 
{
        PreDefinedRenderSettings_Default,
        PreDefinedRenderSettings_UnrealEngine,
        PreDefinedRenderSettings_Unity3D,
        PreDefinedRenderSettings_Count,
};
```

## EMCPUnit
```c++
enum EMCPUnit 
{
        Unit_Centimeter,
        Uint_Meter,
};
```

## EMCPReplay
```c++
enum EMCPReplay
{
        MCPReplay_Response,
        MCPReplay_Running,
        MCPReplay_Result,
};
```
## EMCPCommand
```c++
enum EMCPCommand 
{
        CommandStartCapture,
        CommandStopCapture,
        CommandZeroPosition,
        CommandCalibrateMotion,
        CommandStartRecored,
        CommandStopRecored,
        CommandResumeOriginalPosture,
};
```

## EMCPCommandStopCatpureExtraFlag
```c++
enum EMCPCommandStopCatpureExtraFlag
{
        StopCatpureExtraFlag_SensorsModulesPowerOff,
        StopCatpureExtraFlag_SensorsModulesHibernate,
};
```
## EMCPCommandExtraLong
```c++
enum EMCPCommandExtraLong
{
        CommandExtraLong_DeviceRadio,
        CommandExtraLong_AvatarName,
};
```
## EMCPCommandProgress
```c++
enum EMCPCommandProgress 
{
        CommandProgress_CalibrateMotion,
};
```

## EMCP_CalibrateMotionProgressStep
```c++
 enum EMCPCalibrateMotionProgressStep 
{
        CalibrateMotionProgressStep_Prepare,
        CalibrateMotionProgressStep_Countdown,
        CalibrateMotionProgressStep_Progress,
};
```

## MCPEvent_t

```c++
struct MCPEvent_t
{
    uint32_t        size;
    EMCPEventType   eventType;
    double          fTimestamp;
    MCPEventData_t  eventData;
};
```
### Members：
- **size** : in, must be sizeof(MCPEvent_t)
- **eventType** : out
- **fTimestamp** : time since software started
- **eventData** : data of event

## MCPEventData_t
```c++
union MCPEventData_t
{
    MCPEvent_Reserved_t reserved;

    MCPEvent_MotionData_t motionData;

    MCPEvent_SystemError_t systemError;

    MCPEvent_SensorModuleData_t sensorModuleData;
    
    MCPEvent_TrackerData_t trackerData;

    MCPEvent_CommandRespond_t commandRespond;
};
```

## MCPEvent_Reserved_t
```c++
struct MCPEvent_Reserved_t
{
    uint64_t reserved0;
    uint64_t reserved1;
    uint64_t reserved2;
    uint64_t reserved3;
    uint64_t reserved4;
    uint64_t reserved5;
};
```

## MCPEvent_MotionData_t
```c++
struct MCPEvent_MotionData_t 
{
    MCPAvatarHandle_t   avatarHandle;
};
```

## MCPEvent_SystemError_t
```c++
struct MCPEvent_SystemError_t
{
    EMCPError error;
};
```

## MCPEvent_SensorModuleData_t
```c++
struct MCPEvent_SensorModuleData_t 
{
    MCPSensorModuleHandle_t _sensorModuleHandle;
};
```

## MCPEvent_TrackerData_t

```c++
struct MCPEvent_TrackerData_t
{
    MCPTrackerHandle_t _trackerHandle;
};
```

## MCPEvent_CommandRespond_t
```c++
struct MCPEvent_CommandRespond_t 
{
        MCPCommandHandle_t _commandHandle;
        EMCPReplay _replay;
};
```

[1]:data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAA7UAAAK5CAYAAABg7An6AAAAAXNSR0IArs4c6QAAPcp0RVh0bXhmaWxlACUzQ214ZmlsZSUyMGhvc3QlM0QlMjJhcHAuZGlhZ3JhbXMubmV0JTIyJTIwbW9kaWZpZWQlM0QlMjIyMDIyLTAzLTIzVDA3JTNBMTYlM0ExMS41MDhaJTIyJTIwYWdlbnQlM0QlMjI1LjAlMjAoV2luZG93cyUyME5UJTIwMTAuMCUzQiUyMFdpbjY0JTNCJTIweDY0KSUyMEFwcGxlV2ViS2l0JTJGNTM3LjM2JTIwKEtIVE1MJTJDJTIwbGlrZSUyMEdlY2tvKSUyMENocm9tZSUyRjk5LjAuNDg0NC44MiUyMFNhZmFyaSUyRjUzNy4zNiUyMiUyMGV0YWclM0QlMjJZWXJHcTdwNWVmdzJYLWVlelNUdSUyMiUyMHZlcnNpb24lM0QlMjIxNy4xLjMlMjIlMjB0eXBlJTNEJTIyZGV2aWNlJTIyJTIwcGFnZXMlM0QlMjI4JTIyJTNFJTNDZGlhZ3JhbSUyMGlkJTNEJTIyU1pYV0Zqa1R6a1MtNk9UOG5yR0clMjIlMjBuYW1lJTNEJTIyJUU3JUFDJUFDJTIwMSUyMCVFOSVBMSVCNSUyMiUzRTVaaGJiOW93R0laJTJGalM5YmhUakhTd2hRcG1sU0t6YnQyc1JPWXMySkkyTUs5TmZQSmc0RVFpVjJvSTYycXpqdjUxUGVKJTJGWW5HOENrM0QwSlZCZGZPQ1lNdUE3ZUFUZ0ZyanR5WUtRZVd0azNpdSUyQkZqWkFMaWsybGs3Q2tiNlJ0YWRRTnhXUjlWbEZ5emlTdHo4V1VWeFZKNVptR2hPRGI4Mm9aWiUyQmVqMWlnblBXR1pJdFpYdjFNc2kwYU5mT2VrTHdqTmkzYmtrV01pSldvckcyRmRJTXkzSFFuT0FFd0U1N0lwbGJ1RU1HMWU2MHZUYnY1TzlEZ3hRU3A1UzRPWGVZeW0lMkIzZzJENG8zTDY4JTJCdjdpVHhZUHA1Uld4amZuZ0pSR3ZSSmdweTMzcmclMkJDYkNoUGRsUVBnWkZ0UVNaWTFTblYwcThncnJaQWxVMjhqVmV4UHJSMkhDRWwySGNsTTlZbndra2l4VjFWTTFBdU5iZWElMkZPZHE0UFZFWXRWclJKUkFiRVJueSUyQmJIdmt6bXFZUHo1QmElMkZjbmxjSm8lMkZvTGJYdmx3c0Y1Qlh0ZWdWa0E0Z1NNRTFPWWpMOG16MkFXZ3ZFRWpPZGc1b05vRHNid3o4ek1CY0lhU2NJWkY0ZjJNQTZ4RTRZcWxsSEdPanIyU1lROXBhJTJCbDREOUlKeEs1S3hnRWxuOWplQzh5M2cxa3ZrMCUyRmdneEJ3ZXJnOHdXWkxFcEptbDRqczRwOHozZnVSTWEzVGNhJTJGamN6RHAlMkJkJTJGZkpGQTZ5aUMyN2F2JTJGd0JGWkJ0Rk9KaE1ZbmUlMkYlMkJ1MGtmemN5MFdBeWlkMDEweU5qZmZ1SzdXV1NMRU8lMkI0MXhaSkZtSzNlZ2FDaHpHSyUyQmRPaThSJTJCSm1uUHNUWlN5Y0JZV0U4bG8lMkY2cFJPOVBsN1ozVEQyZTJiVW5HSzJMSXczRWFGNnBjcW9NVXNkbE9OSEcwQlN4c1FtVUZHUGRvJTJCNmoxajJYdTF4ZmtUdzJ0eEp1ODlUZGFuJTJCY1IxOFh0WFd1TG1XTTFnc3ppWXJMdERERCUyRmdVdU1MallycndyWEp3UDVkSSUyRmslMkJpTVBnd3VualV1c1hVdSUyRlJQSlVMbmNtUVdFJTJGdDFZcU5mVEplQWgxcmxLaGJPZiUzQyUyRmRpYWdyYW0lM0UlM0NkaWFncmFtJTIwaWQlM0QlMjI3SEwycHd0LWtIcHVsSGhkYnhVcSUyMiUyMG5hbWUlM0QlMjIlRTclQUMlQUMlMjAyJTIwJUU5JUExJUI1JTIyJTNFN1Z2YmtwczRFUDBhUDY0TEpJVHhvJTJCZXlTWGFUcWxSTjFXNlNseTBaWkNEQnlDWGtzWjJ2WHdtRXVRaVBzUWRzRDRsZnh0MklCdnIwNlZhM21SRzhYMjdmTWJ3S1BsR1BSQ05nZU5zUmZCZ0JZQnJRRVglMkJrWnBkcGtEWEpGRDRMUGJXb1VEeUZQMGwlMkJwdEt1UTQ4a2xZV2Mwb2lIcTZyU3BYRk1YRjdSWWNib3BycHNRYVBxVlZmWUo1cml5Y1dScnYwMzlIaVFhUjFrRlByM0pQU0QlMkZNcW1vWTRzY2I1WUtaSUFlM1JUVXNISEVieG5sUExzMjNKN1R5THB2Tnd2SDdENzhQRnU1biUyQmI3ZXd2bHYzUDMzJTJCNW4lMkY3SWpQMTV5aW43UjJBazV0MmFWdUFtZkpmN2kzakNmVXFrakFmVXB6R09IZ3Z0SGFQcjJDUFNxaUdrWXMxSFNsZENhUXJsZDhMNVRzVUNYbk1xVkFGZlJ1cW9lQXEyJTJCeUxQSDZOYyUyRktyTXBjTER0aUx0bE5UU0RjcGRDVjB6bDd5d0RxaG94TXduTDltenNuWFNNYVdRVWs1JTJCUiUyQmlTaUpzVUN4aUpNQSUyQmZxM0dIVmZqNiUyQjNVRlJPS0xRdWtFeElDZEdYN0cwVnBkS2xtN0xra1NBVWtkVEU2MnZPcDdISVYlMkJMTDY3d29lRUNjVXpZVHdVZEptcEE4dlE4ektZU1JMJTJCeFBQVWxQVCUyQmlvWXhUNThHM1kzUWc5QkVlRTZpTyUyQnolMkI4Tk9RdUtjUlplbDE0U0w5dElkTWhhRzhGN0lkTmFRRmRSOFY1bFdjcjg0eXh1WjBvb0RkVlF5MVJrZlolMkZpd2Z0N1NFTGhhSkNKTTZmUHRiZUFXaUdxQjVMcXpEV1RCUFFya0pRazZlVmpnTjhvM0kzaldrNXdtTjFwek1tS3VZbUdvTHlSSml3aG45c1UlMkJNNEdTU2FZZ2RSQVlnT0xhblZXUnlwRFpGWXQ2bjM2Q1VsQzJqSnpKTk5OZGZJeDkybk5kZ3k3d0diaXF2d2VOUTZNRmFjcUtveml1NWJybjE1VVptdklqb3hnMHc0JTJCTkVlSUwlMkZCdzR3WmhHRnElMkZkbkFYRkM3RHQ2N09keFhvcDk2NUtoUHgxaTZGc3RReCUyRmRWT2hiZWtVbnNaY0dnb2pjT2NHRHJ3UVFPRmV1QkthcGdYQU5QcHk3TlU2bHo0U0Z3aHR5VzlmSGZobTFKSmZaZVdHcGJjbFVIRUhMcWNlUmJkVGlJN3RaZFdMM1d6ZWtCUTBqN3JOMEptR0NFTU9sTUp6V1hZJTJCQVh0RE1uTmFYb2JEMW03SE5URHhPV1hoVEJURyUyRjd4S3ZSQ05MbU1CRnRvUTR0VEFYaldkWFcwUlBQTmJ1QUFzdldBYlIxTHgyR2V4dkd5N3dXNFl4NW5KR2NHMUgyeFByMm81R2J5bFpkWjJYN0paNUthOHB0NUtYOU5rYmRsMnlHbFp4UjBEakM3RHNhJTJGTmxrS01hMDJsYm9PM2JJb0p6SEkwM082MUJEWlBLYTA5cnpOc1kxNUJ0eUV2bFEwaGZTMGVLNGlHRlhVbm9lWjhMMnM1OVFPZURuJTJCYlcxTGEwJTJGc2l5UWRWS3o2MXBYanIxM3RTTlFwTGU3REI3VTJSSSUyQmhyRkIxVndnTTVFTDJVWDVUTDRQV282U3VpMmpTdm8lMkZNZlpBNFEydFZrVG1sNTIxcFQ3NUZjWkV5TkQ1RkJuV255c2FqNjFHa2JHRjUwM0FmaW1hSHRHNmU2YTFVWmJWbDlvZ215YiUyQmhnVDFxeGtUOVVmcSUyRlVmZjRZMzZtcm9LQnZIeFJmdEtJRSUyQnZPJTJGS3gxY2JkVFU0JTJCdnFqTHRDbWRZJTJCOW1YelpUMGd4aldXaTlIQVM3Q3ZYMlIzNDBReFM4Z3BxY0VxdWUyV2lNVzFURzZuQW1yOFBKQm85YVUyMHBLWFo2bnNyMHFZYkhUaWlhRnJwTXN3S0lOQThGMXhIQTFlejFUTzRVRzhjZjAxd3F5ZzRaOU4xY3RSVzM0aTJhVGdIanFqOUVsMVIlMkZiV0I5blNkVkhvVTUyV3pmZU9zJTJGM2lYejNwcWFQZjdYckJzV3BLaWd6d3RZRjcxVnAydDluUkh4clNncjYwT2JIcVZLM3NUWklnSUlLc05BazIwUGdNQklSYiUyRmE1RlJwdmlQRmZqNFB3JTNEJTNEJTNDJTJGZGlhZ3JhbSUzRSUzQ2RpYWdyYW0lMjBpZCUzRCUyMjFnZnFJd3Blb0ZVanpFX2hoSDFvJTIyJTIwbmFtZSUzRCUyMiVFNSVBRSU5RSVFNCVCRCU5MyVFNSU4NSVCMyVFNyVCMyVCQiUyMiUzRTVWaGRiNXN3RlAwMVNOdERJOEJBNldOSTBrNWFLclhMcXE1UGs0T2Q0SmJneURnTjJhJTJCZkhTNEJBbXZTTmYzSUpsVXFQcjVjNG5QdWgyMEQ5V2JaaGNEejZKSVRHaHUyU1RJRDlRM2J0a3prcTM4YVdlV0k2NXptd0ZRd0FrWWxNR0slMkZhUEVtb0F0R2FGb3psSnpIa3MzcllNaVRoSWF5aG1FaCUyQkxKdU51RnglMkZhdHpQS1VOWUJUaXVJbmVNaUlqUUMzVExDZSUyQlVEYU40Tk8lMkJDeE16WEJnRGtFYVk4R1VGUWdNRDlRVG5NbiUyQmFaVDBhYSUyRklLWG01dkhCRmVYejNjZnd1Q3pMNzVIcDQ3dzVQYzJmbHpYdGtzUWRCRUh0YTFuYnQlMkJ4UEVDJTJCSUsxeWxWQm9PQ0xoRkR0eERKUWdNY3BqeGVTZGtVSVdtczBrck80TUNnblREV2NzRGp1OFppTHRUTTBtVXpzTUZSNEtnViUyRm9KVVo0bzA5MTFNemV5NFdTSG1rUXRLc0lqVXMlMkZvTHlHWlZpcFV4ZzF2WkFSd2hrJTJCd3pHeXpJc2JCJTJCd3FCSVJGZ0lRUXloT043NUx1dFVETVA0TTlsR0QlMkZSRVZha1hLV1M5bWV2MDYwQUpOaCUyQkNTTjZTcDBLNkNjNjdSV1RiVmVkeWhvb09UTU5Mc0JzdUlTVHFhNDFBYkxOWDAlMkZpemJlN05jWWRGeTNwUkZaemVMQnczcE5qcm5uQ1Z5dlRBM01OeCUyQlc0RDc5aGg1WGpNbGlFdDk0cnlHSk5tV0hCRDRsdCUyQngzVWJvVzNhTGFHZW80NzZTYW01REZVcFU0WVloRnpMaVU1N2dlRkNpUWFtYjFxRzBHWEklMkJCNjN1cVpRclVBc3ZWTTdVbEh4ZWJVbjVRb1QwQ1RzUCUyQmhrV1U3cUhaSHFCVHdvbWFJd2xlNngzcm9Nejd6WHlwYmN1TXJyT2ZGb2tndUw0ODY2VTJaMFIxZnlKMlRSUno2RmluWW8zTFBHdTNTenhtOFpmalhQbnRXclQ2WHRFdVdKVnJIN285MVgyd3ZDdU90ZlB3SGslMkJXc0hvd05uaEgyVjIlMkJHM1pjWXpKNExnZkxSbk8lMkZvR1NieFZubUYxUjdYeW9xQzVPWTdXTmZtVmptVzh6UFR6VDBacU0wJTJGVVdjZUFhZnQlMkZ3ejQyQmIzUUglMkJtJTJGZ0dWM1Q4RThCQ2N4V1JZZDRyQTZSVDBlOWptT21UbWxkbUpneFFuTEJhY3AlMkI0ZkhhbjltNnM5cFQwYk0lMkZKTXptcUFrZk1hcW51YlpFT2pFN05ySlJMWm1Lb3JXM2V1RDlTcSUyQm1Zc0luazVUS2hyeWJIJTJGRUN4YTJHNHQyTXBRb1p5UVZoemNQRWNSUTFhMnN2NjcxM1ViT2FKJTJCYiUyRnA4VmJhTTlxNkglMkJzYXRnOGVDdEhLTiUyRiUyRk1ybEM1RWczd05zOTMzdkxubzl3bHQ3MUxzM2h6JTJCdXYwVEJaWFdCQ1R0cjZ6bmE2SktTckwlMkZyVUtPRUpmVW43M2hsZkZSN2NGaG9LN0clMkZMZW5HMTVHJTJGUm1TY1NXRlh2NklvWFFUOSUyRlM3JTJCR296elJHbzUyTncwMUxHOHFjJTJGUHl2aGNOZmdNJTNEJTNDJTJGZGlhZ3JhbSUzRSUzQ2RpYWdyYW0lMjBpZCUzRCUyMllMV0c4N3NLWVFaMEpoUUJnWk9KJTIyJTIwbmFtZSUzRCUyMiVFNiU4RiVBMSVFNiU4OSU4QiUyMiUzRTdWdGRjNXM0RlAwMW51ayUyQk9BTUlzUDBZMjJtYWJicVROdDNadm5VVWtJMm1NdklJT1k3enNMJTJCOWtwR3drVENKRSUyQnlTajVjWVhhUXIwTG5uNkhJaEhUQ2EzWjB6T0UlMkIlMkIwQmlSanVmRWR4MHc3bmllNjRDJTJCJTJCSkdXVlc0SiUyRkY1dW1ESWNxMDRid3pXJTJCUjNxa3NpNXdqTEpTUjA0cDRYaGVOa1kwVFZIRVN6YklHRjJXdTAwb0tjODZoMU5rR2E0alNHenJmemptaWJLNmpyTTU4UW5oYWFLbTdnZnF4QXpxenNxUUpUQ215eTBUT091QUVhT1U1MGV6dXhFaWN2SDB1dHk3MGI5ZjAlMkIlMkZKJTJGS2Z6ZVhqYnpUN3pmMkEzZCUyRlp4bnlIRkxUQ1U4bVpkZTduclcwZ1dhcjFHQk10Wjhqdm1LNzJNNHVibjhuQXhJNWQ0Z2doT1JXc29Mb25qQ00lMkZoJTJCcnJFdWRPSVV5WlBJSVpuaUNOeFBDYXElMkY5WEdObHdtbUtQck9ZeWt6NldJUEdGTCUyQkl5SWxpc09SVEJ3S0lhd29rMEluR2Y0Wm4wNWpyQXdGQzFZaG0lMkZSTjVUbE1TZXR0MGhlRHlTbkJFOVRZZU5VT3M3RVBEaWRmcGVOTVFpRmhTNjR2S0pSRVhOeThDTlhXYUVocDBKM1d6R21WdjBjVVhHVGJDVzZxTE02b0JTQml2WnlFNDQ2eEpLdFFBUTZRcUZpd0xUd3ZFRlpIQ2lnOXdBZFdLQmZJeWJ1NXgzMDVrRDMlMkJtMUQzYlc1YmdHTzB2aFVhcTVvUlFSbUdZN0tFTzIzV2h5eUthcnJwd0lSeFNVSnQ5ZDBhODJDaWpYVE5vWUk1Q0k2eW50SXhUcXFHYTRvbGtxbklRdWRFOGNEZldjUXFMOGxBQUV3SEdaMHdTS2tmR3pycnVIVzh3WW5RYjJyZktFc1YydVlpMlY0QnZJMjRSTWtTSHlEb0MzMEVveExlQ095Z0JMeVVKRXJFc2l2eVd5eWJvYmpXUG9RSkJYY2hCdlN6dVZ0clc4MEdIYUM4UjVScEFQV0pGMlJMS2haU3Z0eEZSbWRFdyUyRjBlaVVNM09lRmpPNUNKNU1NSFFZMCUyRjloMHpRUDZZYm8lMkJTR3V2VmJRdU1qMkZ2QjhZQ3Z0WUlydkdSdTZiVXIyRHhnSWl1TnJxcGdpeDg0S0wxRm5MUlZCSzdNUkI3ckhaY0F0MkpnVWhuTWtOTmIzSjVNOUZPcUdtVFl6N29IczdGJTJCTyUyRk9wNDA2VXhTV05xc01wcG56MWFacnBBWlp4Q1dzT3UyWDJmQzF1bU05eUxUQnlQaDglMkZ0UGxSbGdPQUtIa1JuWGxKbiUyQk1XU21aMFhiUmlZcWxhVzl3cUdKMDBCNjRvUyUyQlh3TERhNUZzZkFYVG44dnUxNHY0N085NSUyQk9YaiUyQmJlZ082b0Q4djBoc3FtSFNCRCUyQndZZklhdGo3TzlPRWQ5Z2JLeGdOV2dmNzROaEp3dmJtWHh1SkxkbjlRYTljUFFoQmVYTjlhdlVnQUVlc0hsUXV0STYwOWxZUDZrUDJ0VmNQcWtGekxkQU9UTmp0ckw2V3NBOHp1OWNxWnZ0dVElMkZVRFB6eE8lMkZTRHdhdXNIOW5VRmg2MDNWQWVvdHpPUmVEWDFobnBtdnY1NlElMkZYOTI4WG9QNjVNdlJlYWN3d2FLam40JTJGbkZLRHI2cFRBMlhIS29Eemk2a0d5VUhwMEklMkYybGlHcU9mVG15eERlSGJaJTJCc0JxMGhMdWglMkJaYkRWM0gzcGY3NHNHbDNsRnpXWW54SUtObnFzbEw2a2NjU0MlMkZzZ3ZnYnlVdzBsMXFabVdqWCUyQnJudUNOcGlsempmaHJaWWtoQSUyQk1hJTJCd1JNcDAxRnhlWVNwRmVJek13aTZHdm9DWEdmV3glMkZ0cXppR3I2MlVCJTJCMmxuaXlwWjRSdUM2bWgwbG1NU1hjRVVYY20wekRxTmZ1alZNS01QM3NpcXRVUlduR1ZmZnh3S24xT05hamxRUU1ycElZeFNyUVJOeFhvMFJlNTlxcTJ0eDFrNFolMkZWVjgyT3Jwa0VCWEduQ25NRjNDak90TDBWJTJCeXlyTXh6SkwxZkU1VjZPMG5ZSSUyQnZjZmVNUjRIaTllbXk5Skd1clhFRFozZmdQTyUyRjd1S29hOTlDNSUyQiUyRkhqdzBqczQySTNBMjVlOVI5R2hBcThQTWYlMkZYMHZBVm56b0pTUm93Z3VFUnBUSTF4empsSzREWjRJSk1VenFuY1BsZXRUWU43RDNkcjZyTUNXQVVRNzV0aVNJcU50ek0lMkJvJTJGR2t6OUhQZEk3RXo1Ynd5N2locjFXY294WDcyejl5anM5YXNpSUt5SUFOYzdXQWhVVmJ5SHA1d3pmTFBneUMwenVMQjdPJTJCemc5Zkc2SU1temVGMkY2aE0wV1RRMyUyRnptUjclMkJPYiUyRno4Qlo3OEIlM0MlMkZkaWFncmFtJTNFJTNDZGlhZ3JhbSUyMGlkJTNEJTIyNThhMldFdERmUDZhZTFDTmxhN2MlMjIlMjBuYW1lJTNEJTIyJUU3JUFDJUFDJTIwNSUyMCVFOSVBMSVCNSUyMiUzRTdWeGJjNXM0RlA0MXpMUVAzdUZ1ZURTTzAlMkI1dWtrMlRUSnA5MnBHTllyT1ZFY1hDbCUyRjc2bFVCZ0E3Sk5IRERPTEozTUZCMkVrTTUzTHAlMkJPU0NSdE9GOSUyRkNVRXd1OFV1UkpJcXUydEp1NUpVVlpFMWklMkY3SEpKdEVZdWo5UkRBTlBaZDMyZ29ldlY4d2ZaSkxJOCUyQkZpMXhIZ2pFaVhwQVhUckR2d3duSnlVQVk0bFclMkIyeXRHJTJCYmNHWUFwTGdzY0pRR1hwZDg4bE15NVZaSGw3NHl2MHBqUCUyQmFzdmdOJTJCWWc3Y3dGaXhsdzhXcEhwSTBrYlJoaVRKS3IlMkJYb0lFVk5lcXBmcE9DQkltOXhHaTZYeTE5M0R3d2IlMkJqSHJKWU5kdmVTUmJRZ2g5Y3ZMUSUyRmpjM3duTDRVM3VlUG42ZFBuJTJGVHZrVGZlaWxLUzRBaXJyQmJQQUhCSVBENG9za20xU1JkZjhBdTU5aU5tTXdCeUp2NlZJRGdLNTJXc3dqQXhQT25OM0hyaXFwbzIyTkNKdzVES2xqQ2tIZ1VuQUclMkZRWEJBcFNHT2ZCZXllYktIRmlURVAlMkJBUUl4ekdMOWJrJTJCQiUyQjc4d09TQ1FORm9ZMktTdUhLWXklMkJHNngyVDRFcjZBdkVja25CRHUlMkZDN0dsY0p0M2ZWc0g0ekVzbHFhejltMm11Mll6cTBMN2RiYnJQVGJQQXRMdlNDUXlPR2FmVnladzQySm42JTJCYzU1ZXJxTmxyJTJGZndxNWVaNEJZTjZGSXo1MDBja2htZVloJTJCZzBWWmEwT3EyencxbVNvOTElMkJDOGtaTU45RmtRRVU5R016QkclMkZtMU4zSGhVZiUyQiUyRkROR0JBUVR1R2hmbWJTankzdElGSWhSSUI0eTd5SGk1VE9INzNISHAzZkZtSGJ5RU9zeSUyRmtoRmpnS0o1QSUyRlZZQXVtOFk3MEZSYlIzTVNoY3Q0TU5hQWE0JTJCOHNJR3BvU2V0diUyRmxyMlBYVmVyZXg0WTIzSVo4bzlJQktGTDFzSXVLT2N0MDJVdFV4SDE5ZXJuejBNbnNHZURVeEVYZ2ElMkZmTTlXMTk3U0o0Q1hwYko1S0lwbkFGV29SNWJRMVdZRmZVVFVNMnBzWkRRZHZFNkJlOWR1UFpueDdaY3N2YW8lMkZhNVVtVTU3aDlGY3dXVk1hcWowRXgyVUtzcjVYRUo0bjM4MVJUTVVyWjlMUW1hYTQxYzVsbHBtR1liY2xPYjZiUVN6Tkg0cHU5RXJpMlY3NGxmTnhPUzRMOGh0JTJCVUw4NkNBTXdXYW5ROENZeWVJQXdaSFZuRzNwaGxhd2ptVEVlbG5OeDh5RmphWSUyRiUyQjJPeW12YjVhVDdsJTJCU0I0d29tTkp4SWYlMkZ4SE4wMkhQamVBSElEREdoeVF3bDdBUHFlcXhGMFo2N01Pa3B6cmR5UVBmRlBuUjdCYkp6eUglMkYzVkdncEpxSWxiV0NuT0xNbnhHcjl6bHphaUNlTDJrRGVsY08xcEthcUV0TzVMMjRyc1h1NlR2M3FISklqNWZFMkQxZUZjdkdwRmRUJTJGbiUyRjg1bkVxdUlXTEJTdHpjamxkNExqWWw4cUNvbXdXc3FtbkZkbjBMY3IlMkJGNTYwMUtUNmw2MjFQRGJyJTJCJTJCcEJWcmRsdlo0MkFmd2dzNmFtTmNQdXA4JTJGSEpsN3dMWEdSczFBdHhiVFhLNHFqNHF2SGJOVjV4VDdoSVZSUmVmc2F6RDNFbk9RclJFdklSajJRTkp2eVZsM1AwMGxOTmN2ZWFncTgxVzdLVzAlMkZKYjdYdFZOSWMxTVpPNVZKS3FMcVJyNUxyJTJGVUxwZTA4SjlXZ3ROck90ZEtCa3hYWFVZb1dXWkFuaWZzRzBwdFJ5Z25mNlZuYSUyQkJjYnBzTElZbUZRdkJiV1U4Nk91Q3p3dWM4UDN1Sng0WlVwSlU4Z2J1MUZKWGMwZEV4MEpkZ2Z4UFI3czJsT3RXdExzSTZSY1BDeXJkdVhORVlqRFJwd3MlMkJCMmx1Z29uTXclMkI1TjJDREk3YXFCUUdUSDJuTG1lSFElMkIwV0hCVmx5b2I2WFppVHFscnM5SHRtVFBNYUZrR1hyJTJCeFFKcFNDNkJldGN4eHV3U0xkeEU0d1FDQmJlT0Z0R2twMGRUQWllaTBKbzdhaXY4N3d5RFdjQ09pcEtjSmJaa0Vsb2dyQlU0RkVrcGlNRkMyRVVjMSUyQm1ZVFNqSUNxNDVoNjN5em1zdnBVOGNFWG9lVG96ODF3WCUyQm5IR0pTQU5lQXhKWHJhaUV6VWMlMkJrUDFPV1Q1MDZBVEg5SzJzbTNUSDlZOUpFUHMwN1dBWk9jUHFlV3M0SUtjeUh2VXlrYVJuZ3RXcERuRmxGV2JGWlJMMlNYQWtSY0RtUUNlZnVDZ25JVDJuT0lXQjJ3TzcxUE1pM3BLeVFTMHNnbG9BcmdSR0VOMGp4Y2U4VEFiUDB6NkZzemdJcEEyMUlyJTJCM2hEUXhoNTM1eHNRNXVxZk82OCUyRmp5MllGV04lMkZZMTV2bG94aGlEekk2VzlIQjg1QkIlMkJqJTJCNXJMNFFMJTJGakE3VkhCck95VlZ3S0g2aXdXZTM0UUIxSXQ4MEh5dVg4amclMkIwWlF2bjVBUEN3enpSMFVTeCUyQnVtN0ElMkZhdDlCWlRGeXhtMlduZVRoR1R5ZThCb2FUQWp5V3FySlhDQlNzJTJCNTQ3c0txcVdWNGlPMWg1M05Ha0lGSm5LM2x1aUxIekhxMnI1RWFwV0tCWFpPakpTZlNWSzhUZmFIZjQxNEclMkZJJTJGWnJ3TDQzVU1QN2w4cUEwTWlSS05TMURHdG1TcFVrRGkwbWNnV1JyMG9qS0xjazIyWVZ0U1lOclVlZSUyQlJPT3NVejY1NG9sajk0RHBqVGxrNzRZall4T3JtVWZnSTgwZDdKMnJFSlIlMkZBZUJ0NWxhZDFmZk5QSkNXSWZpeVg3R01za25xOXZ0anUlMkZqZzNpcjc4aGxQc3Q3eXpYRHVReXMwam9ORENyTFU4RmM0Z3MlMkZ2THVrWENCbzh0QkpHaEhLRmdEazVkWFhIamklMkYwMVA5dGFUQ0szVjZXN0VFY0drYlN3SWdqd3JVMDBQN24lMkZxJTJGcCUyQmprRGdCQkt3Y2FlaFhKYmNqUU9FMlhJTkY3Ymp1VEVVWjdPenRFNktBc0hOb1l0WU9sbmplUmxuN3g3JTJCdk1lSTlTZDQxVXEzTlZqRjJyeFV4VkY0T0NpM1p1aXFBMTV1S2hvdyUyRmJ5MUhYbXlSNmUwdkhRRzBmayUyRjd1UlB4WTBEbnJjUmRidmhEUHVxcmp2cXVmVVl3VlZhM3VOV1VGWHhUMFQwbjJyR3RJMVZIR0ZFJTJCNnF1SmZqOVZiYnNWJTJGd2x4WW9RZnlkc2J4WFJxQTdqbmd1amxqY0JTcmFHU21pOEh5NiUyQjlxciUyRmdpUiUyQmR2SG9ZZUNUMnc3T3ppJTJGSGJST0VOTTQwekhFeHJGdW15SXElMkIwSiUyRnh4RmJzSVlHU1NKdGJ2OVNXSElPc1AxN2E5cm9QdyUzRCUzRCUzQyUyRmRpYWdyYW0lM0UlM0NkaWFncmFtJTIwaWQlM0QlMjJuYXBpQVFiQUczWllSMTlURzFkUSUyMiUyMG5hbWUlM0QlMjIlRTclQUMlQUMlMjA2JTIwJUU5JUExJUI1JTIyJTNFN1Z4Wms5bzRFUDQxZnB3dEg3S1JIMjFnc3FsTlVxbE1zc2Vqc1FXNFltektGZ1BzUTM3N3RtVDVBTWtNSmx5YllvYWFrZHBTUyUyQnF2MWVxV1pEUnJ1Tmk4eTRQbCUyRkdNV2tVUXo5V2lqV1NQTk5BM2R3dkNQVWJZbHhVYURrakRMNDBnVWFnZ3Y4YiUyQmtxaW1vcXpnaXhVNUJtbVVKalplN3hEQkxVeExTSFZxUTU5bDZ0OWcwUzNaYlhRWXpJaEZld2lDUnFYJTJGRkVaMExxcUhyellQZlNUeWJpNmF4TFI0c2dxcXdJQlR6SU1yV0xaSTExcXhobm1XMFRDMDJRNUl3NFZWeSUyQlRTbjY2ZVA3NmRmNmJkdzhrbiUyRm96RHo3VlBKN0xsUGxYb0lPVW5wbVZtYkFzM1hJRmtKaVdsalc4TzI1ZzFGd2pkNFlzalNrUENRNWlOdCUyRkt6NVE4MzFoR2pvdHBJM0pSdm9veiUyQm5pd1FJQmlRTG1tZmZ5VEJMc2h3b2FaWkNTWDhhSjhrZUtVamlXUXJaRUVaSmdPNiUyRmtwekdnS1FuSGl6aUtHTE4lMkJPdDVUTW5MTWdoWm0ydlFXNkRsMlNxTkNCdXJ6dGhuS1gwUm5US3FmS21iSnNzZktWRWhlZFlSc21ucGs1RHdPNUl0Q00yM1VFUThkWVd1aU1saVZMcXozbEc5a2padmFWMVZMaERhUHFzNU40aENRb0RhQzJBc0E3eVBHVWtqajAwMUp2d2tLSW80M0VXUWJHTDZONVBzYjFYdUg1NnpSVzYwRVdMbm1XMHI4NW5rTVF5RXdWblNVaGhVaXhYTHRuaXhiTU9NNTdidG5NUXVncmt1eHBIbGRKN05zalJJeGczVkQxZjVLOWVLJTJGcWdYMlNvUHlhRnlUbVhRZ254R0RuSzBoZEJaaHclMkZxVVV0UmJJV2VWTFNjSkFHTlgzZHRuRXA1UkF1ZnN4aUdYS3ZwMDJCWFR5MjBwMyUyRmwyRVd0dGxIWlkyVHVNOUwzR0pXaWtSaHhYYTZIJTJGVFBxN2I2dDNqT3dEY3VmblBIMUFoVk1LcmE2R2tHMUphZ0Ywd0lZWVFYQ2htNWZ5aFJZJTJCcGxsVlF2JTJGZEdrSjhaajJjZExCRjdPVGx2RzJjTUFEV0xLa0dGYmJSS3FXcEFLVXYxcDFtTFVDbWRBZ1RybjFNbmclMkJTWUpsRVpjeTRpWG1jUko5Q0xiWmlsYk5WTG1qalZlTjh0dUwxZzJsYlI0dmJSZ3lqWVBrQzFPbmRIYU00R1hCUm5tMiUyRkZwWmFVWllNb3RFOHZFclNMT29uQWJaSjZIWlVqeE15TFNxTzhrb3pSWWlrd3R4MVV5NXJHd2ZQaUM5SVZ2WWJCak5FUEpHazRjUEs1N1RZWmFDZnhURUhGTVNGSFJOaWw1b0glMkIlMkJpdklYMiUyRmhwd1JyUXRDVzM5ckhDM2JjMVprTXhBbnRPRWUwUno4RHBKMmdjUzh4UkluT3NpZ2lSRWpBY2l6ZXJ0eUt2MWxRR3lKWURPYXlIJTJGNXdDcElxc3JJJTJCUklDRmtQaEZvSTRac2pKTzl0b0FkQ3JlQU4zUndoZVhQQ2ZpRFVJS1NLR3ElMkJNa0J4Zk93JTJCRVdnamQzRk9vT0xjUUdqd1FhaEJDTiUyRmNVVUklMkJ0aFVld2U1NWdkN3NMNnUxaVh5VHZkTHlRSEVad3RUbXFtZGJVWnI5bm1haTdwem5BMiUyQkUlMkZXbm5HMHFLWFB6M1FSU2R0WlZ4NUtzczdHYiUyQnVyZTBCeUwyRXpVamUxM2pnYzA5Uk01TDNOYjZRZ3RDcldrTWNrakM4akRXY1lCdlolMkJtVkJ2SGxnalg3aEVLQSUyRkhyY1BvOUV2N1BEM3glMkJQMlFUTlMzTGtZMjVxUE5XJTJCZ2pRZnNSbzN2TTRvNzFqQWtYTTJGcDglMkI4aktkNWo5bDFYd0cyJTJCOERqbnNKcFc5N3dHTDRmSFRHTHhOVTBhYzEybllFVk9MS2ZRSXpJSmdPdHo4MjBBdENPMDltSEVqYlVVTDRJeWFBT3dQS01CbTJ0dUVWMGZmekZnVDEzdnp2WXJoM09LNm1HWXFlbGZYY1JQMnYlMkI0Q2MweFFreG1VeGxUWWtDZ3FmaFExUGU5cEh1UjFYa2ZabE9NeCUyQnU4bVRyNTBING5XMnBLZTE2eSUyQnczTjFGWkxvcHpzUGR4eHNBdnNoVWI3ZEZ5N3J4ZjFTMW54WDRYVXNqMWN2ZE5IWlZjbllUZkNZYSUyQk96T3VhUDJ1R0pmVm9UOHREb0s0YkZnaTVtQkJiRFMyMkY4TUxCM05nd1JNZUhDOHhocE1EYUM0a0g1dUZlWUp6JTJCUE5qalNYRjhhZWhrM21xRUhqR1BFeUl3MlB1T3ZtY3dxNGJzQVpNNFo0ckhrdWUlMkJTVmpaYTk5bHU5WG5aMzJuU0NCVk9mZEZJc2VWNVhrSXpmNEklMkZ1eDdRUkNldXR3enZwTUNmU0hkWkRFNXVMVEp4WWM5MmFybTkwbzZOUGIzZEJ5bk0yWUxEU2lvM0olMkJtajhNRmt2TFUlMkJxSWZER1RYZDJldWlibXNmeGdvRmdkMWo2U0d6T21EcGIyZm1vOFZEbVluQm9EVTRhY0xWaHFiY2g3V3BCMzhEb0paWWNicDk5RHJHdVdTQUZDJTJCTlFWU0hPU2Q1UExTd21jbXRYTFZ5bTdrSlRSMHpwSVFHTiUyQkE3WDFKYUNNbW5XY3dBekpOaGt4THhXT1QxOFhzdmx0VXEwQmx3V0R1czdlOFNYVTllb3loaW5qZ094Y2FBZjl1NUlWQ0RzciUyQlAxQk4xZjBIVWxpc2ZNZWJtUkdsZGRvVzBWTHlUemdncm1hYm9FTlMyNXBuTUJGYktaNkoxeXpwNHNNc21iT2hUcUNta2VGdVdaR2xNSnBZTUNGckZ0emJvY3doUGVhaEZlSFJEc0p2ZVZCM2hQcHQ3aFRhajlQdWtGbHl1ODBPSll1NDZiWlNodWFxdmVhRUhtR2U2eGYzdDlYbWNqNUgxemxnczhqJTJCZzJ4JTJGVHBpQ09ZZTdqeHI5cW1IVlRIMTIxSk9jN0ZmTndlcHlFM3VkUGVGVVoxQjE0eXFBZDFwTmRSb2hLZEMwWWc4bEZJSnpxUGF3SEhxUGxkb3kxZkYxVUZLT1g2MkxocDE5dDd2TGVEc1hwNjlFTFZNSyUyQkxxbnpGdENPMmFBV21ScDFDZFFyZkJ2RHAxTHdVNEpFemNXem5Nb0J2OXNCdUs4QUFYMVVCNUtPZVAwbGVNUFJsNE5FRDVQNGdxNDd6NnJmM3J3U3k0aVhUemtnUkd6JTJCY2glMkZYdWp6TlM0SXl2T3BlcjQ0d2pqMjBmMEI0TmJZVmoyJTJGMjYwRHF0REFlNjk2VGJVZjhvb01HeEd3SVhCTHVPZ2k1eFdiTTdqam9YMkxiaXdONjRGTnFIdjVmaG1tYzdLZnYlMkJpVXNFcUhzbk85VUJtaVBQS0lXeFBPR2NCN0xOZDlxVTM5UFFmRE9RTmY0UCUzQyUyRmRpYWdyYW0lM0UlM0NkaWFncmFtJTIwaWQlM0QlMjJLMFI3QzBiYTJUd2xFWjVXVlZXVCUyMiUyMG5hbWUlM0QlMjIlRTclQUMlQUMlMjA3JTIwJUU5JUExJUI1JTIyJTNFN1ZwdGM1czRFUDQxZkV6R1NMejVvMk83N1V6VE5uTzVYaSUyRjNUWWJGME1pSUU3S04lMkZldFBHR0dnY3Z5U0M3Vm5ZaWN6WVZjcmdmZlpaeDlCTVBCd2xuJTJGa0pJMiUyQnNBQ29nWHBCYnVDUmdaRFp3NTc4VTNoV3BjZTIzTkl4NVhHZ2dtckhZN3lHYXFieXp1TUFzbGFnWUl5S09HMDdmWllrNEl1V2ozRE9sdTJ3a05IMldWTXlCYzN4NkJPcWUzJTJGRWdZaVUxJTJCejE2b0ZQRUU4amRXclBWZ016VWdVclJ4YVJnQzBiTGp3MjhKQXpKc3FqV1Q0RVdpU3Z5Z3YlMkJ2a0NySCUyRmo3ZXYzbno5ajVuUFclMkJQSTF1eXNVJTJCbkRKbCUyQnhVNEpPTFZTeiUyRmtmOTI3YXd2enhUUDUlMkJ1MmZrVHZQJTJGNzJwVU1yRXFrb1lCREolMkZ5bVJjUkd6S0VrTEh0ZmZPbiUyRk1GRkt2MnBNSFpQQWsybGltdDdCbUVINm1oZXZZOVk2bUslMkJBbENyRlNaa0xsZzBoV0pHVldqOGd2eTFkJTJGRiUyRkZ1N01wJTJGVWNodGpsTGVzbGJLT3pKREtaTWJtM0lkOWFWR0ZTdmdVeEo0NFhNWVZLV3RVbThyJTJGUjJBemtCY3BBemhRSXVKRnV5U0pxdXpwTnE1R1R4NG9BRThCczF4M1FlaGNuV25JZ1FnWXBDbU5mWGtCTE5IUWJzTzNqR0lCanluWkpHY3BHMEliblJhNllVenBrRkhHTnd2aGdJQVglMkJrV1E0T3daR2lPTzc4RWtMR2F3UkNqa1RlZGswQmJBQmVSNzA2eEdjVlhWcW1WWnlselclMkZFY1Z5Nk1HOWJIVEVUQklBMllFUlpwVzd4MFo5JTJCelFtS2FXJTJCR3NEM0RhMmd3M1F1cWdHaURXZVBZSm9jT3dQa0VoeDZSTnhNczI2WTF3WWhzamZ5YmpBbVRpMmMwYkdtZWJaR1lldWpIdVpTUWNaNTE0VTQ2d0RqT3VlYXhlc2JxWjNkcTdoSzlkZTV0QkJydlV2aW11dXhyVnZLU1R2ZlF1SmJQUGNKTE92Sk51UkZ1OUlrcUdMSXBtbmtXeElXZmJ1YjZFZDk5eFNoaTd0UmkyUFJZTmswbnBxak5RVUs0eU9HTlklMkZrbUdtZDFFVTYyc1VlMkNVTmhqMkZYSXhYaFE1ZW85VXclMkJqY2dvYXVncmFQUm9jVjdjMGxiVE4xd0RsWk5RSlNGaWNpYTZ6OFVEanF3ckt3M1Nvc3ElMkYlMkZMUHdoT2k1Y0g1UlhVaGJYOUt2OWo4NlFMYmtuOXNXME1MTU96akxGcnlBdnhuTzZhUVdnWFB6dWJ3ZWF6b3hrVWRpT3UlMkZQeldSOXBINnJIQ3NJTW00V2pBR1dQSDZIdkc0TU8xZmV6cENnZmJ4MldwTlhKZmdXWkhBSm90JTJCR28wWHdDdzJxSlZtN0xHck4lMkIxUlRzYTlEZCUyRjFQQXF5WEM4ZHB1eFBYZXZaUHdhajNHJTJGZThsQXVtUVVhaUU3M1VBWGlTS2g5MlFDdEYxU2hNYlRSQjc3RW1hUVRmeXVhTVZ5JTJCMGtIYW1BV0IwRlp5cERGYXpMWnJGY1VpRXFoWE55JTJCTSUyQnpSenBMWlN5U3Q2VyUyRmZmVkJuTVpxdkYlMkJ3U2c1dmViZCUyQnRIbnRVbVMlMkJ0MDhxanhyTUtZV0dZZ2RDYXhsdWdodmJveFJhJTJCY2QlMkJRSlNWJTJGcFVmVzN0MUFWJTJGMkl6U2J6N0xEaTF6Mm9kJTJCaG13QVl2c0hicHY0Y21lS2YlMkJkNmJ6RnJwdDc4SnNSMWQ2MDdOMXBiZXRreFZBbXZYckt5WFE5VXRBZVB3ZiUzQyUyRmRpYWdyYW0lM0UlM0NkaWFncmFtJTIwaWQlM0QlMjJSWHpvOG13RUoxblQ1dnJYUDZDbiUyMiUyMG5hbWUlM0QlMjIlRTclQUMlQUMlMjA4JTIwJUU5JUExJUI1JTIyJTNFN1Z0Wmw2STRGUDQxUGxxSHNJbVBsbHJWWjA1WFQzWGJmYWJyTVVKWXVnSmhRbHglMkYlMkZRUUpBb0w3Z25ORUh5UTNDM0MlMkYlMkI5MTdzOWhTJTJCdjc4bGNMUWZTTVd3aTFac3VZdFpkQ1NaUmxvT3YlMkJKSll0RW9xbWRST0JRejBwRUlCT012Q1VTUWtsSUo1NkZva0pEUmdobVhsZ1VtaVFJa01rS01rZ3BtUldiMlFRWDd4cENCNVVFSXhQaXN2UWZ6Mkt1a0FKSnlpcSUyQklNOXh4YTBOVFZUNE1HMHNCSkVMTFRMTGlaUmhTJTJCbFRRbGh5NWMlMkY3Q01mS1MlMkZYaURLZSUyRk9tQTJuMHdsZ0xzOSUyQjJXS1h0dkpZQyUyRkhkRm0lMkZBa1VCTzNub1A5OTElMkJ3dGFEclJ3QnB6JTJCYjNlMCUyRlBWdk8zMDF0a2oxaFN5dVBsRWtsTG5FSVFIRXcwejZiRTdvRk1XRFNyeEF5U1N3VmlYQVM5RW5ZcVlycXJMZVh3a0pSWXMlMkZpTEdGc0JJNFlZU0xYT1pqVWN2Zmp5NSUyQnglMkYyZnRMVDRJWVpiRlFielFta2hTZ2NxU0NneUloTnFvaDFhU2UwVVVnZXhIZTNVcEYyc3NweXhDZlclMkZJdUlqJTJGcEM4QVVVWU1tOWF0RWdvRE50WnQ4dkE0eGNDdnlPd0ZPTk9JWjZJTyUyRlVwZ2d6MXdoQjdKbjhBRXBUUUxzSTNjejJHUmlGY0tXZkclMkZVRVJuUUs2dG9keG4yQkNWd01wRmtTR2JjYU5HQ1dmS0Zlam13WWEyM0VQRWpDQlBOQ1BCbTJLS0VQem5Xb1d0UXJRa2k3Q1k2bkN5R2NaJTJGZVdVNUc2TyUyQllwJTJCSldEa0VqQURGS3RwOGVqSWRLUzZvVkVhJTJGN2ZkciUyQjMxZiUyRnBkJTJCVCUyQjFSTE1SWWptSyUyRlVBY0tjcGx6QXVjNkhxRXMyMWJOaXNKWiUyQmxqWGROckpCd0FkUk5PYXdpM25VaDdDV2ZjRmVIMFBZUzdQdFh1T0xZQm8yNnFkUnFxYmFmUVhxb0IlMkJhNjRacFM0OW5lSWdrZFBJR1VOMUV5eWJrT3lLdkpJQjdMc3ZraVdQblolMkJDbzFKOVBBemFGMnBPNVlCVUZKOHZUeWJleXhITTE3NnlOVmtKSXNMMSUyQktZZkdna3EyMlpxdCUyRjklMkZ2YnB3JTJCWGJYNzJnYlN1Qk4lMkY0Wm5UUUR5TUM3SVY3biUyQmRJTmJWOFolMkJFcE55M1hDckRjdzN3am1XdGw4eWlTamdma1VtSlU2WVRZYW1HOEVzMXByQWx6ZXFuZ25HT2Z5MzI5b3pvYlRXQ2VQbUFncmN0M3pUZERzV096S2NmY253eGRmUWwxMTdWRUtGN2tHSWZFQ0Z1VkdmbzhGbVdHcDZhWmtlcXBDMnRpOFA2NDl2MGllSURPczlhdWNZV3ZsSFpXZkZKcWZpSEtoY0FMU095VW1paUl2Y0s3bkVXd3QlMkZsWjZoTldud2lQRTVWeTc1SE5OVDZFWEFWTGw4cFFaNkJXZUlnWDI4cDVDSzZIM2djcHI4WTMzeUp6Q2Z1OGhYZHA3bkFmeGVaT3NpeUlJQ3ZobGNHNUJNTTN0MG13dTElMkJ0bUN5ajFiUVdjRkRPNjhrWU1FQ2Z4dHNXTXpmYUswcjFvektoTVg4c3JxTjlJcFpGJTJCaFdPRWk3WUVzZWNFJTJGTnJrJTJCUElZb3p6SDdwZm5uYmduS256UHNoSWJScEczaE9QVmVMRmxDTlh4d2JYbmxqWlkyMHJKZjYlMkZQR1lyT3JmWHB2cnhaN1NEYjFnRFFscDZBSkF0Q0htY0lHWElDcVhiSEtIWWh0aDBoVnZJWGw0ang1WTJsSWVkY1Mza3BoM09YJTJCT05KdEQlMkJVRnlkJTJCTzFOOURSbVdXaFhZRFhtc1ZBYjJxd1Z3dGZOVVRQYTFxaEJ1YU9VUXJxbm4lMkIlMkZmcWxaMVRWcjBmZGRxOWZXM3M0R2xCWGFzckZTYzNpZSUyRkR3T0xDSHlqRWklMkY5N3ZsM2hodyUyQkRlQ3RkTmEzR2ZMc2F4RlBPVXRkQzFtdVQ3dHg5aHdPbndQSUdrZ25KUmElMkJ6SXVYMnR5cVF0RGVGJTJGSjROTnplNUNUcDN4MDI1NGVabGRoSHVrNXRLT1ltbGxEVFVMRkd6bzk0Yk5VODVRTldrdUpzYlJuZWY0bFlkemclMkJpaHFJVkN3bnk3U2pLaTluJTJGRnhObm5QMExWQm4lMkJCdyUzRCUzRCUzQyUyRmRpYWdyYW0lM0UlM0MlMkZteGZpbGUlM0UP2bPnAAAgAElEQVR4XuzdefxdRX3/8U9qzEYCJF8ikKAENASkSMACkTWohR+l0iJWkU1QfshiiSRswQBRAkEgwVA2+VFBFgUVqVBKQZA1NEBlFSHGYlgSlmxAAlmM/f4ec3W+zHcy5945+zKv/NPK95wzM8+Ze+5535lzTp/u7u5u4R8CCCCAQCKBea++JQ88uUBeW/KevLfqj7Ji5ZpEx2EnBBAoV2DwwH4yaEBf2bRrPdlrh5EyerMNy60QpSOAAAIIeAv0IdR6W7EhAggg0EvgqXmL5Zb758kmw9eXrTb/kIwaMQwhBBCoscD8hctk3stvymtvvi0HjR8tY0dvVOPWUHUEEEAgHAFCbTh9TUsRQCBDATVDe92dz7cC7T6f2jrDI3MoBBAoW+CuR16QNxa/I0fstw0ztmV3BuUjgAACHgKEWg8k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
