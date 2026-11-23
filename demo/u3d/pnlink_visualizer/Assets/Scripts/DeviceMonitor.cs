using System;
using System.Collections.Generic;
using MocapApi;
using UnityEngine;

public sealed class DeviceMonitor : MonoBehaviour
{
    DeviceSnapshot current=new DeviceSnapshot();
    double updated=-100;
    public DeviceSnapshot Capture(bool motion)
    {
        return new DeviceSnapshot { Available=current.Available,Online=current.Online&&(motion||Time.realtimeSinceStartupAsDouble-updated<3),
            Motion=motion,Serial=current.Serial,Firmware=current.Firmware,Sensors=current.Sensors };
    }
    public void Clear() { current=new DeviceSnapshot();updated=-100; }
    public void Read(ulong handle)
    {
        var d=new DeviceSnapshot(); var status=EMCPMasterStatus.MasterStatus_Offline;
        var api=IMCPSystem.System;
        if(api.GetMasterStatus(ref status,handle)!=EMCPError.Error_None)return;
        d.Available=true; d.Online=status==EMCPMasterStatus.MasterStatus_Online;
        api.GetMasterSerialNumber(ref d.Serial,handle); api.GetMasterVersion(ref d.Firmware,handle);
        uint count=0; api.GetSensorModules(null,ref count,handle);
        if(count>512)return;
        var handles=new ulong[count];
        var sensors=new List<SensorSnapshot>();
        if(count>0 && api.GetSensorModules(handles,ref count,handle)==EMCPError.Error_None) {
            for(int i=0;i<Math.Min(count,(uint)handles.Length);i++) {
                var s=new SensorSnapshot(); var module=IMCPSensorModule.SensorModule;
                if(module.GetSensorModuleId(ref s.Id,handles[i])!=EMCPError.Error_None)continue;
                var tag=EMCPJointTag.JointTag_Invalid; module.GetSensorModuleBoundBone(ref tag,handles[i]);
                IMCPJoint.Joint.GetJointNameByTag(ref s.Bone,tag);
                module.GetSensorModuleSerialNumber(ref s.Serial,handles[i]); module.GetSensorModuleFirmwareVersionString(ref s.Firmware,handles[i]);
                float rate=0; if(module.GetSesnorModulePacketRate(ref rate,handles[i])==EMCPError.Error_None && !float.IsNaN(rate) && !float.IsInfinity(rate))s.PacketRate=rate*100;
                var mag=EMCPMagEnv.kMagEnv_Unknown; if(module.GetSensorModuleMagEnvValue(ref mag,handles[i])==EMCPError.Error_None)s.Magnetic=(int)mag;
                sensors.Add(s);
            }
        }
        d.Sensors=sensors.ToArray(); current=d;updated=Time.realtimeSinceStartupAsDouble;
    }
}
