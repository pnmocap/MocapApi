using MocapApi;
using TMPro;
using UnityEngine;

public class SensorItemGUI : MonoBehaviour
{
  public TextMeshProUGUI ID;
  public TextMeshProUGUI Rate;
  public TextMeshProUGUI Mag;
  public TextMeshProUGUI FirwmareVersion;
  public TextMeshProUGUI SerialNumber;
  uint _id;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }


  public void updateGUI(ulong sensorModuleHandle) {
    uint id = 0;
    IMCPSensorModule.SensorModule.GetSensorModuleId(ref id, sensorModuleHandle);
    Debug.Assert(id == _id);

    EMCPJointTag jointTag = EMCPJointTag.JointTag_Invalid;
    IMCPSensorModule.SensorModule.GetSensorModuleBoundBone(ref jointTag, sensorModuleHandle);
    string bone = null;
    IMCPJoint.Joint.GetJointNameByTag(ref bone, jointTag);
    ID.SetText(bone);

    string ver = null;
    IMCPSensorModule.SensorModule.GetSensorModuleFirmwareVersionString(ref ver, sensorModuleHandle);
    FirwmareVersion.SetText(ver);

    string sn = null;
    IMCPSensorModule.SensorModule.GetSensorModuleSerialNumber(ref sn, sensorModuleHandle);
    SerialNumber.SetText(sn);

    EMCPMagEnv magEnv = EMCPMagEnv.kMagEnv_Unknown;
    if (IMCPSensorModule.SensorModule.GetSensorModuleMagEnvValue(ref magEnv, sensorModuleHandle) == EMCPError.Error_None)
    {
      switch (magEnv)
      {
        case EMCPMagEnv.kMagEnv_Unknown:
          Mag.SetText("Unknown");
          break;
        case EMCPMagEnv.kMagEnv_Good:
          Mag.SetText("Good");
          break;
        case EMCPMagEnv.kMagEnv_Bad:
          Mag.SetText("Bad");
          break;
        case EMCPMagEnv.kMagEnv_Worst:
          Mag.SetText("Worst");
          break;
        default:
          break;
      }
    }
    else { 
      Mag.SetText("-");
    }
    float rate = 0.0f;
    if (IMCPSensorModule.SensorModule.GetSesnorModulePacketRate(ref rate, sensorModuleHandle) == EMCPError.Error_None)
    {
      Rate.SetText(string.Format("{0}", rate * 100.0f));
    }
    else { 
      Rate.SetText("Unknown");
    }
  }
  public void setId(uint id) {
    _id = id;
  }
}
