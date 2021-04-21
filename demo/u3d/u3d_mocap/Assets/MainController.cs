using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MocapApi;
using System;
using System.Runtime.InteropServices;

public class MainController : MonoBehaviour
{
    private ulong applicationHandle = 0;
    public Transform AvatarRoot;
    public string JointPrefix;
    // Start is called before the first frame update
    void Start()
    {
        /*
        string hierarchy = "";
        IMCPAvatar.Avatar.GetAvatarJointHierarchy(ref hierarchy);
        EMCPJointTag[] children = new EMCPJointTag[3];
        uint size = 3;
        IMCPJoint.Joint.GetJointChildJointTag(children, ref size, EMCPJointTag.JointTag_Hips);
        Debug.Log(hierarchy);
        foreach (var child in children)
        {
            string ss = "";
            IMCPJoint.Joint.GetJointNameByTag(ref ss, child);
            Debug.Log(child.ToString() + " : " + ss);
            EMCPJointTag p = EMCPJointTag.JointTag_Invalid;
            IMCPJoint.Joint.GetJointParentJointTag(ref p, child);
            Debug.Assert(p == EMCPJointTag.JointTag_Hips);
        }
        */

        IMCPApplication.Application.CreateApplication(ref applicationHandle);

        {
            ulong settings = 0;
            IMCPSettings.Settings.CreateSettings(ref settings);
            IMCPSettings.Settings.SetSettingsBvhData(EMCPBvhData.BvhDataType_Binary, settings);
            IMCPSettings.Settings.SetSettingsBvhRotation(EMCPBvhRotation.BvhRotation_YXZ, settings);
            IMCPSettings.Settings.SetSettingsBvhTransformation(EMCPBvhTransformation.BvhTransformation_Enable, settings);
            IMCPSettings.Settings.SetSettingsUDP(8088, settings);
            IMCPApplication.Application.SetApplicationSettings(settings, applicationHandle);
            IMCPSettings.Settings.DestroySettings(settings);
            IMCPApplication.Application.OpenApplication(applicationHandle);
        }

        {
            ulong renderSettings = 0;
            /*
            IMCPRenderSettings.RenderSettings.CreateRenderSettings(ref renderSettings);
            IMCPRenderSettings.RenderSettings.SetUpVector(EMCPUpVector.UpVector_YAxis, 1, renderSettings);
            IMCPRenderSettings.RenderSettings.SetFrontVector(EMCPFrontVector.FrontVector_ParityEven, 1, renderSettings);
            IMCPRenderSettings.RenderSettings.SetCoordSystem(EMCPCoordSystem.CoordSystem_LeftHanded, renderSettings);
            IMCPRenderSettings.RenderSettings.SetRotatingDirection(EMCPRotatingDirection.RotatingDirection_Clockwise, renderSettings);
            IMCPRenderSettings.RenderSettings.SetUnit(EMCPUnit.Uint_Meter, renderSettings);
            IMCPApplication.Application.SetApplicationRenderSettings(renderSettings, applicationHandle);
            IMCPRenderSettings.RenderSettings.DestroyRenderSettings(renderSettings);
            */
            IMCPRenderSettings.RenderSettings.GetPreDefRenderSettings(EMCPPreDefinedRenderSettings.PreDefinedRenderSettings_Unity3D, ref renderSettings);
            IMCPApplication.Application.SetApplicationRenderSettings(renderSettings, applicationHandle);
        }
        


        //IMCPApplication.Application.DestroyApplication(applicationHandle);
    }

    // Update is called once per frame
    void Update()
    {
        MCPEvent_t[] ev = new MCPEvent_t[3];
        for (uint i =0; i< ev.Length; ++i) {
            ev[i].size = (uint)Marshal.SizeOf<MCPEvent_t>();
        }
        uint count = 3;
        var er = IMCPApplication.Application.PollApplicationNextEvent(ev, ref count, applicationHandle);
        if (    er == EMCPError.Error_None 
            &&  count > 0) {
            if (ev[0].eventType == EMCPEventType.MCPEvent_AvatarUpdated){
                handleAvatarUpdated(ev[0].eventData.motionData.avatarHandle);
            }
            else if (ev[0].eventType == EMCPEventType.MCPEvent_Error) {
                Debug.LogErrorFormat("Application open failed : {0}", ev[0].eventData.systemError.error);
            }
        }
    }

    void transformJoints(ulong jointHandle, Transform parentTransform_) 
    {
        string name = "";
        IMCPJoint.Joint.GetJointName(ref name, jointHandle);
        var jointTranform = parentTransform_.Find(JointPrefix+name);
        if (null != jointTranform)
        {
            Vector3 v = new Vector3();
            if (IMCPJoint.Joint.GetJointLocalPosition(ref v.x, ref v.y, ref v.z, jointHandle) == EMCPError.Error_None) {
                jointTranform.localPosition = v;
            }
            
            Quaternion q = new Quaternion();
            IMCPJoint.Joint.GetJointLocalRotaion(ref q.x, ref q.y, ref q.z, ref q.w, jointHandle);
            jointTranform.localRotation = q;
            uint numberOfChild = 0;
            IMCPJoint.Joint.GetJointChild(null, ref numberOfChild, jointHandle);
            if (numberOfChild > 0) {
                ulong[] children = new ulong[numberOfChild];
                IMCPJoint.Joint.GetJointChild(children, ref numberOfChild, jointHandle);
                foreach (var child in children) {
                    transformJoints(child, jointTranform);
                }
            }
        }
        else {
            Debug.Log(name);
        }
    }

    void handleAvatarUpdated(ulong avatarHandle) 
    {
        ulong rootJoint = 0;
        IMCPAvatar.Avatar.GetAvatarRootJoint(ref rootJoint, avatarHandle);

        transformJoints(rootJoint, AvatarRoot);
        
        /*
        string name = "";
        IMCPAvatar.Avatar.GetAvatarName(ref name, avatarHandle);
        uint joints = 0;
        IMCPAvatar.Avatar.GetAvatarJoints(null, ref joints, avatarHandle);
        if (joints > 0) {
            ulong[] vJoints = new ulong[joints];
            IMCPAvatar.Avatar.GetAvatarJoints(vJoints, ref joints, avatarHandle);
            string log="";
            for (uint i = 0; i < vJoints.Length; ++i) {
                string jointName = "";
                IMCPJoint.Joint.GetJointName(ref jointName, vJoints[i]);
                log += jointName;
                log += "; ";
            }
            Debug.Log(log);
        }
        */
    }
}
