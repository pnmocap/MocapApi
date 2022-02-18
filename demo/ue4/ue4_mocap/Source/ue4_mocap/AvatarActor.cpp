// Fill out your copyright notice in the Description page of Project Settings.


#include "AvatarActor.h"
#include "logging/MessageLog.h"


// Sets default values
AAvatarActor::AAvatarActor()
    : _appcliation(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Query Application Interface [1/8/2021 brian.wang]
    MocapApi::IMCPApplication* mcpApplication = nullptr;
    MocapApi::EMCPError mcpError = MocapApi::MCPGetGenericInterface(MocapApi::IMCPApplication_Version,
        reinterpret_cast<void**>(&mcpApplication));
    check(mcpError == MocapApi::Error_None);

    // Create Communication Application With PNS [1/8/2021 brian.wang]
    mcpError = mcpApplication->CreateApplication(&_appcliation);

    {
        // Query Settings Interface [1/8/2021 brian.wang]
        MocapApi::IMCPSettings* mcpSettings = nullptr;
        mcpError = MocapApi::MCPGetGenericInterface(MocapApi::IMCPSettings_Version,
            reinterpret_cast<void**>(&mcpSettings));
        check(mcpError == MocapApi::Error_None);

        // Create Settings Object [1/8/2021 brian.wang]
        MocapApi::MCPSettingsHandle_t mcpSettingsHandle = 0;
        mcpError = mcpSettings->CreateSettings(&mcpSettingsHandle);
        check(mcpError == MocapApi::Error_None);

        // Follow settings value setting is same sa PNS Settings [1/8/2021 brian.wang]
        {
            /*
            * [1/8/2021 brian.wang]
            *   This settings same as "Bvh Broadcasting"-> "Frame Format" -> "Type", only support MocapApi::BvhDataType_Binary, 
                other MocapApi::BvhDataType_Mask_LegacyHumanHierarchy with MocapApi::BvhDataType_Binary Group same as "Skeleton"->"Perception Neuron/ Perception Neuron Pro" in PNS.
            */
            mcpError = mcpSettings->SetSettingsBvhData(MocapApi::BvhDataType_Binary, mcpSettingsHandle);
            check(mcpError == MocapApi::Error_None);
            // this settings same as "BVH Broadcasting"->"Bvh Format"->Rotation in pns [1/8/2021 brian.wang]
            mcpError = mcpSettings->SetSettingsBvhRotation(MocapApi::BvhRotation_YXZ, mcpSettingsHandle);
            check(mcpError == MocapApi::Error_None);
            // this settings same as pns [1/8/2021 brian.wang]
            mcpError = mcpSettings->SetSettingsBvhTransformation(MocapApi::BvhTransformation_Enable, mcpSettingsHandle);
            check(mcpError == MocapApi::Error_None);
#if 1   //  same as "Bvh Broadcasting" -> "Protocol" -> "UDP" -> "Port" in PNS Setting
            mcpError = mcpSettings->SetSettingsUDP(7002, mcpSettingsHandle);
#else   //  same as "Bvh Broadcasting" -> "Protocol" -> "TCP" -> "Local IP" -> "IP" and "Port" in PNS Setting
            mcpError = mcpSettings->SetSettingsTCP("127.0.0.1", 7002, mcpSettingsHandle);
#endif
            check(mcpError == MocapApi::Error_None);
        }
        
        // set Settings for Application [1/8/2021 brian.wang]
        mcpError = mcpApplication->SetApplicationSettings(mcpSettingsHandle, _appcliation);
        check(mcpError == MocapApi::Error_None);
        // Destroy Settings, if not used [1/8/2021 brian.wang]
        mcpError = mcpSettings->DestroySettings(mcpSettingsHandle);
        check(mcpError == MocapApi::Error_None);
    }

    {
        // Query RenderSettings Interface [1/8/2021 brian.wang]
        MocapApi::IMCPRenderSettings* renderSettings = nullptr;
        mcpError = MocapApi::MCPGetGenericInterface(MocapApi::IMCPRenderSettings_Version,
            reinterpret_cast<void**>(&renderSettings));
        check(mcpError == MocapApi::Error_None);

        MocapApi::MCPRenderSettingsHandle_t renderSettingsHandle = 0;
#if 0   
    /*
    * Menu set Render Settings same as engine, same as follow code
    */
        // Create Render Settings [1/8/2021 brian.wang]
        mcpError = renderSettings->CreateRenderSettings(&renderSettingsHandle);
        check(mcpError == MocapApi::Error_None);
        // set coord system[1/8/2021 brian.wang]
        renderSettings->SetUpVector(MocapApi::UpVector_ZAxis, 1, renderSettingsHandle);
        renderSettings->SetFrontVector(MocapApi::FrontVector_ParityEven, 1, renderSettingsHandle);
        renderSettings->SetCoordSystem(MocapApi::CoordSystem_LeftHanded, renderSettingsHandle);
        renderSettings->SetRotatingDirection(MocapApi::RotatingDirection_Clockwise, renderSettingsHandle);

        // Set Render Settings for Application [1/8/2021 brian.wang]
        mcpError = mcpApplication->SetApplicationRenderSettings(renderSettingsHandle, _appcliation);
        check(mcpError == MocapApi::Error_None);
        // Destroy Render settings if not used [1/8/2021 brian.wang]
        mcpError = renderSettings->DestroyRenderSettings(renderSettingsHandle);
        check(mcpError == MocapApi::Error_None);
#endif
        // Get Pre-Defined RenderSetting suport PreDefinedRenderSettings_UnrealEngine, or PreDefinedRenderSettings_Unity3D [1/8/2021 brian.wang]
        renderSettings->GetPreDefRenderSettings(MocapApi::PreDefinedRenderSettings_UnrealEngine, &renderSettingsHandle);// MUST NOT Destroy this [1/8/2021 brian.wang]
        // Set Render Settings for Application [1/8/2021 brian.wang]
        mcpError = mcpApplication->SetApplicationRenderSettings(renderSettingsHandle, _appcliation);
        check(mcpError == MocapApi::Error_None);
    }
}

// Called when the game starts or when spawned
void AAvatarActor::BeginPlay()
{
	Super::BeginPlay();

    // Query Application Interface [1/8/2021 brian.wang]
    MocapApi::IMCPApplication* mcpApplication = nullptr;
    MocapApi::MCPGetGenericInterface(MocapApi::IMCPApplication_Version,
        reinterpret_cast<void**>(&mcpApplication));

    // open application [1/8/2021 brian.wang]
    mcpApplication->OpenApplication(_appcliation);
	
}

void AAvatarActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if(_appcliation != 0){
        // Query Application Interface [1/8/2021 brian.wang]
        MocapApi::IMCPApplication* mcpApplication = nullptr;
        MocapApi::MCPGetGenericInterface(MocapApi::IMCPApplication_Version,
            reinterpret_cast<void**>(&mcpApplication));

        // Close Application [1/8/2021 brian.wang]
        mcpApplication->CloseApplication(_appcliation);
    }
}

// Called every frame
void AAvatarActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    // Query Application Interface [1/8/2021 brian.wang]
    MocapApi::IMCPApplication* mcpApplication = nullptr;
    MocapApi::MCPGetGenericInterface(MocapApi::IMCPApplication_Version,
        reinterpret_cast<void**>(&mcpApplication));

    TArray<MocapApi::MCPEvent_t> events;
    uint32_t unEvent = 0;
    // poll next event in application event queue [1/8/2021 brian.wang]
    MocapApi::EMCPError mcpError = mcpApplication->PollApplicationNextEvent(nullptr, &unEvent, _appcliation);
    if(unEvent > 0){
        events.AddUninitialized(unEvent);
        for (auto & e : events){
            e.size = sizeof(MocapApi::MCPEvent_t);
        }
        mcpError = mcpApplication->PollApplicationNextEvent(events.GetData(), &unEvent, _appcliation);
    }
    if (unEvent > 0) {
        // update avatar posture [1/8/2021 brian.wang]
        for (const auto & e : events){
            if(e.eventType == MocapApi::MCPEvent_AvatarUpdated){
                updateAvatarTransform(e.eventData.motionData.avatarHandle);
            }
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("%d"), mcpError);
}

void AAvatarActor::updateAvatarTransform(MocapApi::MCPAvatarHandle_t avatar) 
{
    // Query Avatar Interface [1/8/2021 brian.wang]
    MocapApi::IMCPAvatar* avatarMgr = nullptr;
    MocapApi::MCPGetGenericInterface(MocapApi::IMCPAvatar_Version, 
        reinterpret_cast<void**>(&avatarMgr));

    // Get Root Joint at Avatar [1/8/2021 brian.wang]
    MocapApi::MCPJointHandle_t joint = 0;
    avatarMgr->GetAvatarRootJoint(&joint, avatar);
    // Update joint posture recursived [1/8/2021 brian.wang]
    updateJointsTransform(joint);
}

void AAvatarActor::updateJointsTransform(MocapApi::MCPJointHandle_t joint) 
{
    // Get Joint Interface [1/8/2021 brian.wang]
    MocapApi::IMCPJoint* jointMgr = nullptr;
    MocapApi::MCPGetGenericInterface(MocapApi::IMCPJoint_Version,
        reinterpret_cast<void**>(&jointMgr));

    // Get Joint Name [1/8/2021 brian.wang]
    const char* name = nullptr;
    jointMgr->GetJointName(&name, joint);

    // FindCommpone for joint [1/8/2021 brian.wang]
    auto jointComponent = FindComponent<USceneComponent>(name);
    if (nullptr != jointComponent) {

        // Get Joint Current Position in local coord-sys [1/8/2021 brian.wang]
        FVector v;
        if(MocapApi::Error_None == jointMgr->GetJointLocalPosition(&v.X, &v.Y, &v.Z, joint)){
            // Get Joint Current Rotation in local coord-sys [1/8/2021 brian.wang]
            FQuat q;
            jointMgr->GetJointLocalRotation(&q.X, &q.Y, &q.Z, &q.W, joint);
            // Drive joint [1/8/2021 brian.wang]
            jointComponent->SetRelativeLocationAndRotation(v, q);
        }
        else{
            // Get Joint Current Rotation in local coord-sys [1/8/2021 brian.wang]
            FQuat q;
            jointMgr->GetJointLocalRotation(&q.X, &q.Y, &q.Z, &q.W, joint);
            // Drive joint [1/8/2021 brian.wang]
            jointComponent->SetRelativeRotation(q);
        }
        // Get Child Joint [1/8/2021 brian.wang]
        uint32_t numberOfChildren = 0;
        jointMgr->GetJointChild(nullptr, &numberOfChildren, joint);
        if (numberOfChildren > 0) {
            TArray<MocapApi::MCPJointHandle_t> joints;
            joints.SetNumZeroed(numberOfChildren);
            jointMgr->GetJointChild(&joints[0], &numberOfChildren, joint);

            // Update joint posture recursived [1/8/2021 brian.wang]
            for (auto j : joints){
                updateJointsTransform(j);
            }
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("%S"), name);
    }
}

