#include <algorithm>
#include <iostream>
#include <vector>

#include "MocapApi.h"

#define VERIFY(r)                                                    \
  if (auto r_ = (r); r_ != Error_None) {                             \
    std::cout << "\'" << #r "\' return Error : " << r_ << std::endl; \
    throw std::runtime_error("");                                    \
  }

using namespace MocapApi;

void printJointPosture(MCPJointHandle_t jointHandle) {
  IMCPJoint* mcpJoint = nullptr;
  VERIFY(MCPGetGenericInterface(IMCPJoint_Version,
                                reinterpret_cast<void**>(&mcpJoint)));
  const char* szJointName = nullptr;
  VERIFY(mcpJoint->GetJointName(&szJointName, jointHandle));
  std::cout << "\t" << szJointName << ":";
  float qx, qy, qz, qw;
  VERIFY(mcpJoint->GetJointLocalRotation(&qx, &qy, &qz, &qw, jointHandle));
  std::cout << "(" << qx << "," << qy << "," << qz << "," << qw << ") ";
  float px, py, pz;
  VERIFY(mcpJoint->GetJointLocalPosition(&px, &py, &pz, jointHandle));
  std::cout << "(" << px << "," << py << "," << pz << ")" << std::endl;
  uint32_t unSizeOfJointHandle = 0;
  VERIFY(mcpJoint->GetJointChild(nullptr, &unSizeOfJointHandle, jointHandle));
  if (unSizeOfJointHandle > 0) {
    std::vector<MCPJointHandle_t> vJointHandles;
    vJointHandles.resize(unSizeOfJointHandle);
    VERIFY(mcpJoint->GetJointChild(vJointHandles.data(), &unSizeOfJointHandle,
                                   jointHandle));
    std::for_each(vJointHandles.begin(), vJointHandles.end(),
                  [](MCPJointHandle_t childJointHandle) {
                    printJointPosture(childJointHandle);
                  });
  }
}

void handleAvatarUpdated(const MCPEvent_MotionData_t& motionData) {
  IMCPAvatar* mcpvatar = nullptr;
  VERIFY(MCPGetGenericInterface(IMCPAvatar_Version,
                                reinterpret_cast<void**>(&mcpvatar)));
  const char* szAvatarName = nullptr;
  VERIFY(mcpvatar->GetAvatarName(&szAvatarName, motionData.avatarHandle));
  std::cout << "Avatar \'" << szAvatarName << "\' updated" << std::endl;

  MCPJointHandle_t rootJointHandle = 0;
  VERIFY(
      mcpvatar->GetAvatarRootJoint(&rootJointHandle, motionData.avatarHandle));
  printJointPosture(rootJointHandle);
}

void handleEvent(const MCPEvent_t& ev) {
  switch (ev.eventType) {
    case MCPEvent_AvatarUpdated:
      handleAvatarUpdated(ev.eventData.motionData);
      break;
    case MCPEvent_Error:
      std::cout << "Error occur " << ev.eventData.systemError.error
                << std::endl;
      break;
    default:
      std::cout << "Unhandled event" << std::endl;
      break;
  }
}

int main() {
  std::cout << "MocapApi Version:" << MCPGetMocapApiVersionString()
            << std::endl;
  IMCPSettings* mcpSettings = nullptr;
  VERIFY(MocapApi::MCPGetGenericInterface(
      MocapApi::IMCPSettings_Version, reinterpret_cast<void**>(&mcpSettings)));
  MCPSettingsHandle_t mcpSettingsHandle = 0;
  VERIFY(mcpSettings->CreateSettings(&mcpSettingsHandle));
  VERIFY(mcpSettings->SetSettingsUDP(8088, mcpSettingsHandle));
  VERIFY(mcpSettings->SetSettingsBvhData(MocapApi::BvhDataType_Binary,
                                         mcpSettingsHandle));
  VERIFY(mcpSettings->SetSettingsBvhTransformation(
      MocapApi::BvhTransformation_Enable, mcpSettingsHandle));
  VERIFY(mcpSettings->SetSettingsBvhRotation(MocapApi::BvhRotation_YXZ,
                                             mcpSettingsHandle));

  IMCPApplication* mcpApplication = nullptr;
  VERIFY(MCPGetGenericInterface(MocapApi::IMCPApplication_Version,
                                reinterpret_cast<void**>(&mcpApplication)));
  MCPApplicationHandle_t _appcliation = 0;
  VERIFY(mcpApplication->CreateApplication(&_appcliation));
  VERIFY(
      mcpApplication->SetApplicationSettings(mcpSettingsHandle, _appcliation));
  VERIFY(mcpSettings->DestroySettings(mcpSettingsHandle));
  VERIFY(mcpApplication->OpenApplication(_appcliation));

  for (;;) {
    std::vector<MCPEvent_t> vEvents;
    uint32_t unEvent = 0;
    VERIFY(mcpApplication->PollApplicationNextEvent(nullptr, &unEvent,
                                                    _appcliation));
    vEvents.resize(unEvent);
    std::for_each(vEvents.begin(), vEvents.end(),
                  [](MCPEvent_t& ev) { ev.size = sizeof(MCPEvent_t); });
    auto mcpError = mcpApplication->PollApplicationNextEvent(
        vEvents.data(), &unEvent, _appcliation);
    if (mcpError == Error_None) {
      vEvents.resize(unEvent);
      std::for_each(vEvents.begin(), vEvents.end(),
                    [](const MCPEvent_t& ev) { handleEvent(ev); });
    } else if (mcpError != Error_MoreEvent) {
      std::cout << "\'mcpApplication->PollApplicationNextEvent(nullptr, "
                   "&unEvent, _appcliation)\' return Error : "
                << mcpError << std::endl;
      throw std::runtime_error("");
    }
  }

  VERIFY(mcpApplication->CloseApplication(_appcliation));
  VERIFY(mcpApplication->DestroyApplication(_appcliation));
  return 0;
}