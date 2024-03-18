using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
#if UNITY_5_3_OR_NEWER
    using UnityEngine;
#endif
namespace MocapApi
{
    public class InterfaceNotFountException : Exception 
    {
        public InterfaceNotFountException(string interfaceName)
            : base(String.Format("Interface {0} not found!", interfaceName)) 
        {
        }
    }
    public enum EMCPError
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
    public enum EMCPJointTag
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
    public class IMCPRigidBody
    {
        static public IMCPRigidBody RigidBody
        {
            get
            {
                if (null == rigidBody)
                {
                    rigidBody = new IMCPRigidBody();
                }
                return rigidBody;
            }
        }
        public EMCPError GetRigidBodyRotation(ref float x, ref float y, ref float z, ref float w, ulong ulRigidBodyHandle)
        {
            return ProcTable.GetRigidBodyRotation(ref x, ref y, ref z, ref w, ulRigidBodyHandle);
        }
        public EMCPError GetRigidBodyPosition(ref float x, ref float y, ref float z, ulong ulRigidBodyHandle)
        {
            return ProcTable.GetRigidBodyPosition(ref x, ref y, ref z, ulRigidBodyHandle);
        }
        public EMCPError GetRigidBodyStatus(ref int status, ulong ulRigidBodyHandle)
        {
            return ProcTable.GetRigidBodyStatus(ref status, ulRigidBodyHandle);
        }
        public EMCPError GetRigidBodyId(ref int id, ulong ulRigidBodyHandle)
        {
            return ProcTable.GetRigidBodyId(ref id, ulRigidBodyHandle);
        }
        public EMCPError GetRigidBodyJointTag(ref EMCPJointTag jointTag_, ulong ulRigidBodyHandle)
        {
            return ProcTable.GetRigidBodyJointTag(ref jointTag_, ulRigidBodyHandle);
        }
        public EMCPError GetRigidBodyAxisAngle(ref float x, ref float y, ref float z, ref float angle, ulong ulRigidBodyHandle)
        {
            return ProcTable.GetRigidBodyAxisAngle(ref x, ref y, ref z, ref angle, ulRigidBodyHandle);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPRigidBody_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetRigidBodyRotation(ref float x, ref float y, ref float z, ref float w, ulong ulRigidBodyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetRigidBodyRotation GetRigidBodyRotation;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetRigidBodyPosition(ref float x, ref float y, ref float z, ulong ulRigidBodyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetRigidBodyPosition GetRigidBodyPosition;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetRigidBodyStatus(ref int status, ulong ulRigidBodyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetRigidBodyStatus GetRigidBodyStatus;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetRigidBodyId(ref int id, ulong ulRigidBodyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetRigidBodyId GetRigidBodyId;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetRigidBodyJointTag(ref EMCPJointTag jointTag_, ulong ulRigidBodyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetRigidBodyJointTag GetRigidBodyJointTag;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetRigidBodyAxisAngle(ref float x, ref float y, ref float z, ref float angle, ulong ulRigidBodyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetRigidBodyAxisAngle GetRigidBodyAxisAngle;

        };
        private MCPRigidBody_ProcTable ProcTable;
        private static IMCPRigidBody rigidBody;
        private const string IMCPRigidBody_Version = "IMCPRigidBody_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPRigidBody()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPRigidBody_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPRigidBody_Version);
            }
            ProcTable = (MCPRigidBody_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPRigidBody_ProcTable));
        }
    };
    public class IMCPTracker
    {
        static public IMCPTracker Tracker
        {
            get
            {
                if (null == tracker)
                {
                    tracker = new IMCPTracker();
                }
                return tracker;
            }
        }
        public EMCPError SendMessageData(string message, int len, ulong ulTrackerHandle)
        {
            return ProcTable.SendMessageData(message, len, ulTrackerHandle);
        }
        public EMCPError GetTrackerRotation(ref float x, ref float y, ref float z, ref float w, string deviceName, ulong ulTrackerHandle)
        {
            return ProcTable.GetTrackerRotation(ref x, ref y, ref z, ref w, deviceName, ulTrackerHandle);
        }
        public EMCPError GetTrackerPosition(ref float x, ref float y, ref float z, string deviceName, ulong ulTrackerHandle)
        {
            return ProcTable.GetTrackerPosition(ref x, ref y, ref z, deviceName, ulTrackerHandle);
        }
        public EMCPError GetTrackerEulerAng(ref float x, ref float y, ref float z, string deviceName, ulong ulTrackerHandle)
        {
            return ProcTable.GetTrackerEulerAng(ref x, ref y, ref z, deviceName, ulTrackerHandle);
        }
        public EMCPError GetDeviceCount(ref int devCount, ulong ulTrackerHandle)
        {
            return ProcTable.GetDeviceCount(ref devCount, ulTrackerHandle);
        }
        public EMCPError GetDeviceName(int serialNum, ref string name, ulong ulTrackerHandle)
        {
            IntPtr name_ = IntPtr.Zero;
            var r = ProcTable.GetDeviceName(serialNum, ref name_, ulTrackerHandle);
            if (r == EMCPError.Error_None)
            {
                name = Marshal.PtrToStringAnsi(name_);
            }
            return r;
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPTracker_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SendMessageData(string message, int len, ulong ulTrackerHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SendMessageData SendMessageData;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetTrackerRotation(ref float x, ref float y, ref float z, ref float w, string deviceName, ulong ulTrackerHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetTrackerRotation GetTrackerRotation;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetTrackerPosition(ref float x, ref float y, ref float z, string deviceName, ulong ulTrackerHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetTrackerPosition GetTrackerPosition;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetTrackerEulerAng(ref float x, ref float y, ref float z, string deviceName, ulong ulTrackerHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetTrackerEulerAng GetTrackerEulerAng;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetDeviceCount(ref int devCount, ulong ulTrackerHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetDeviceCount GetDeviceCount;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetDeviceName(int serialNum, ref IntPtr name, ulong ulTrackerHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetDeviceName GetDeviceName;

        };
        private MCPTracker_ProcTable ProcTable;
        private static IMCPTracker tracker;
        private const string IMCPTracker_Version = "IMCPTracker_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPTracker()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPTracker_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPTracker_Version);
            }
            ProcTable = (MCPTracker_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPTracker_ProcTable));
        }
    };
    public class IMCPSensorModule
    {
        static public IMCPSensorModule SensorModule
        {
            get
            {
                if (null == sensorModule)
                {
                    sensorModule = new IMCPSensorModule();
                }
                return sensorModule;
            }
        }
        public EMCPError GetSensorModulePosture(ref float x, ref float y, ref float z, ref float w, ulong sensorModuleHandle)
        {
            return ProcTable.GetSensorModulePosture(ref x, ref y, ref z, ref w, sensorModuleHandle);
        }
        public EMCPError GetSensorModuleAngularVelocity(ref float x, ref float y, ref float z, ulong sensorModuleHandle)
        {
            return ProcTable.GetSensorModuleAngularVelocity(ref x, ref y, ref z, sensorModuleHandle);
        }
        public EMCPError GetSensorModuleAcceleratedVelocity(ref float x, ref float y, ref float z, ulong sensorModuleHandle)
        {
            return ProcTable.GetSensorModuleAcceleratedVelocity(ref x, ref y, ref z, sensorModuleHandle);
        }
        public EMCPError GetSensorModuleId(ref uint id, ulong sensorModuleHandle)
        {
            return ProcTable.GetSensorModuleId(ref id, sensorModuleHandle);
        }
        public EMCPError GetSensorModuleCompassValue(ref float x, ref float y, ref float z, ulong sensorModuleHandle)
        {
            return ProcTable.GetSensorModuleCompassValue(ref x, ref y, ref z, sensorModuleHandle);
        }
        public EMCPError GetSensorModuleTemperature(ref float temperature, ulong sensorModuleHandle)
        {
            return ProcTable.GetSensorModuleTemperature(ref temperature, sensorModuleHandle);
        }
        public EMCPError GetSensorModulePostureTimeCode(ref uint hour, ref uint minute, ref uint second, ref uint frame, ref uint rate, ulong sensorModuleHandle)
        {
            return ProcTable.GetSensorModulePostureTimeCode(ref hour, ref minute, ref second, ref frame, ref rate, sensorModuleHandle);
        }
        public EMCPError GetSensorModulePostureTime(ref uint hour, ref uint minute, [In, Out]uint[] second, ref uint millisecond, ulong sensorModuleHandle)
        {
            return ProcTable.GetSensorModulePostureTime(ref hour, ref minute, second, ref millisecond, sensorModuleHandle);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPSensorModule_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModulePosture(ref float x, ref float y, ref float z, ref float w, ulong sensorModuleHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModulePosture GetSensorModulePosture;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModuleAngularVelocity(ref float x, ref float y, ref float z, ulong sensorModuleHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModuleAngularVelocity GetSensorModuleAngularVelocity;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModuleAcceleratedVelocity(ref float x, ref float y, ref float z, ulong sensorModuleHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModuleAcceleratedVelocity GetSensorModuleAcceleratedVelocity;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModuleId(ref uint id, ulong sensorModuleHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModuleId GetSensorModuleId;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModuleCompassValue(ref float x, ref float y, ref float z, ulong sensorModuleHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModuleCompassValue GetSensorModuleCompassValue;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModuleTemperature(ref float temperature, ulong sensorModuleHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModuleTemperature GetSensorModuleTemperature;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModulePostureTimeCode(ref uint hour, ref uint minute, ref uint second, ref uint frame, ref uint rate, ulong sensorModuleHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModulePostureTimeCode GetSensorModulePostureTimeCode;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModulePostureTime(ref uint hour, ref uint minute, [In, Out]uint[] second, ref uint millisecond, ulong sensorModuleHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModulePostureTime GetSensorModulePostureTime;

        };
        private MCPSensorModule_ProcTable ProcTable;
        private static IMCPSensorModule sensorModule;
        private const string IMCPSensorModule_Version = "IMCPSensorModule_002";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPSensorModule()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPSensorModule_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPSensorModule_Version);
            }
            ProcTable = (MCPSensorModule_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPSensorModule_ProcTable));
        }
    };
    public class IMCPBodyPart
    {
        static public IMCPBodyPart BodyPart
        {
            get
            {
                if (null == bodyPart)
                {
                    bodyPart = new IMCPBodyPart();
                }
                return bodyPart;
            }
        }
        public EMCPError GetJointPosition(ref float x, ref float y, ref float z, ulong bodyPartHandle)
        {
            return ProcTable.GetJointPosition(ref x, ref y, ref z, bodyPartHandle);
        }
        public EMCPError GetJointDisplacementSpeed(ref float x, ref float y, ref float z, ulong bodyPartHandle)
        {
            return ProcTable.GetJointDisplacementSpeed(ref x, ref y, ref z, bodyPartHandle);
        }
        public EMCPError GetBodyPartPosture(ref float x, ref float y, ref float z, ref float w, ulong bodyPartHandle)
        {
            return ProcTable.GetBodyPartPosture(ref x, ref y, ref z, ref w, bodyPartHandle);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPBodyPart_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointPosition(ref float x, ref float y, ref float z, ulong bodyPartHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointPosition GetJointPosition;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointDisplacementSpeed(ref float x, ref float y, ref float z, ulong bodyPartHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointDisplacementSpeed GetJointDisplacementSpeed;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetBodyPartPosture(ref float x, ref float y, ref float z, ref float w, ulong bodyPartHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetBodyPartPosture GetBodyPartPosture;

        };
        private MCPBodyPart_ProcTable ProcTable;
        private static IMCPBodyPart bodyPart;
        private const string IMCPBodyPart_Version = "IMCPBodyPart_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPBodyPart()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPBodyPart_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPBodyPart_Version);
            }
            ProcTable = (MCPBodyPart_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPBodyPart_ProcTable));
        }
    };
    public class IMCPJoint
    {
        static public IMCPJoint Joint
        {
            get
            {
                if (null == joint)
                {
                    joint = new IMCPJoint();
                }
                return joint;
            }
        }
        public EMCPError GetJointName(ref string ppStr, ulong ulJointHandle)
        {
            IntPtr ppStr_ = IntPtr.Zero;
            var r = ProcTable.GetJointName(ref ppStr_, ulJointHandle);
            if (r == EMCPError.Error_None)
            {
                ppStr = Marshal.PtrToStringAnsi(ppStr_);
            }
            return r;
        }
        public EMCPError GetJointLocalRotation(ref float x, ref float y, ref float z, ref float w, ulong ulJointHandle)
        {
            return ProcTable.GetJointLocalRotation(ref x, ref y, ref z, ref w, ulJointHandle);
        }
        public EMCPError GetJointLocalRotationByEuler(ref float x, ref float y, ref float z, ulong ulJointHandle)
        {
            return ProcTable.GetJointLocalRotationByEuler(ref x, ref y, ref z, ulJointHandle);
        }
        public EMCPError GetJointLocalPosition(ref float x, ref float y, ref float z, ulong ulJointHandle)
        {
            return ProcTable.GetJointLocalPosition(ref x, ref y, ref z, ulJointHandle);
        }
        public EMCPError GetJointDefaultLocalPosition(ref float x, ref float y, ref float z, ulong ulJointHandle)
        {
            return ProcTable.GetJointDefaultLocalPosition(ref x, ref y, ref z, ulJointHandle);
        }
        public EMCPError GetJointChild([In, Out]ulong[] pJointHandle, ref uint punSizeOfJointHandle, ulong ulJointHandle)
        {
            return ProcTable.GetJointChild(pJointHandle, ref punSizeOfJointHandle, ulJointHandle);
        }
        public EMCPError GetJointBodyPart(ref ulong pBodyPartHandle, ulong ulJointHandle)
        {
            return ProcTable.GetJointBodyPart(ref pBodyPartHandle, ulJointHandle);
        }
        public EMCPError GetJointSensorModule(ref ulong pSensorModuleHandle, ulong ulJointHandle)
        {
            return ProcTable.GetJointSensorModule(ref pSensorModuleHandle, ulJointHandle);
        }
        public EMCPError GetJointTag(ref EMCPJointTag pJointTag, ulong ulJointHandle)
        {
            return ProcTable.GetJointTag(ref pJointTag, ulJointHandle);
        }
        public EMCPError GetJointNameByTag(ref string ppStr, EMCPJointTag jointTag)
        {
            IntPtr ppStr_ = IntPtr.Zero;
            var r = ProcTable.GetJointNameByTag(ref ppStr_, jointTag);
            if (r == EMCPError.Error_None)
            {
                ppStr = Marshal.PtrToStringAnsi(ppStr_);
            }
            return r;
        }
        public EMCPError GetJointChildJointTag([In, Out]EMCPJointTag[] pJointTag, ref uint punSizeOfJointTag, EMCPJointTag jointTag)
        {
            return ProcTable.GetJointChildJointTag(pJointTag, ref punSizeOfJointTag, jointTag);
        }
        public EMCPError GetJointParentJointTag(ref EMCPJointTag pJointTag, EMCPJointTag jointTag)
        {
            return ProcTable.GetJointParentJointTag(ref pJointTag, jointTag);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPJoint_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointName(ref IntPtr ppStr, ulong ulJointHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointName GetJointName;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointLocalRotation(ref float x, ref float y, ref float z, ref float w, ulong ulJointHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointLocalRotation GetJointLocalRotation;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointLocalRotationByEuler(ref float x, ref float y, ref float z, ulong ulJointHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointLocalRotationByEuler GetJointLocalRotationByEuler;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointLocalPosition(ref float x, ref float y, ref float z, ulong ulJointHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointLocalPosition GetJointLocalPosition;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointDefaultLocalPosition(ref float x, ref float y, ref float z, ulong ulJointHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointDefaultLocalPosition GetJointDefaultLocalPosition;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointChild([In, Out]ulong[] pJointHandle, ref uint punSizeOfJointHandle, ulong ulJointHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointChild GetJointChild;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointBodyPart(ref ulong pBodyPartHandle, ulong ulJointHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointBodyPart GetJointBodyPart;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointSensorModule(ref ulong pSensorModuleHandle, ulong ulJointHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointSensorModule GetJointSensorModule;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointTag(ref EMCPJointTag pJointTag, ulong ulJointHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointTag GetJointTag;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointNameByTag(ref IntPtr ppStr, EMCPJointTag jointTag);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointNameByTag GetJointNameByTag;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointChildJointTag([In, Out]EMCPJointTag[] pJointTag, ref uint punSizeOfJointTag, EMCPJointTag jointTag);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointChildJointTag GetJointChildJointTag;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetJointParentJointTag(ref EMCPJointTag pJointTag, EMCPJointTag jointTag);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetJointParentJointTag GetJointParentJointTag;

        };
        private MCPJoint_ProcTable ProcTable;
        private static IMCPJoint joint;
        private const string IMCPJoint_Version = "IMCPJoint_003";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPJoint()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPJoint_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPJoint_Version);
            }
            ProcTable = (MCPJoint_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPJoint_ProcTable));
        }
    };
    public class IMCPAvatar
    {
        static public IMCPAvatar Avatar
        {
            get
            {
                if (null == avatar)
                {
                    avatar = new IMCPAvatar();
                }
                return avatar;
            }
        }
        public EMCPError GetAvatarIndex(ref uint index, ulong ulAvatarHandle)
        {
            return ProcTable.GetAvatarIndex(ref index, ulAvatarHandle);
        }
        public EMCPError GetAvatarRootJoint(ref ulong pJointHandle, ulong ulAvatarHandle)
        {
            return ProcTable.GetAvatarRootJoint(ref pJointHandle, ulAvatarHandle);
        }
        public EMCPError GetAvatarJoints([In, Out]ulong[] pJointHandle, ref uint punSizeOfJointHandle, ulong ulAvatarHandle)
        {
            return ProcTable.GetAvatarJoints(pJointHandle, ref punSizeOfJointHandle, ulAvatarHandle);
        }
        public EMCPError GetAvatarJointByName(string name, ref ulong pJointHandle, ulong ulAvatarHandle)
        {
            return ProcTable.GetAvatarJointByName(name, ref pJointHandle, ulAvatarHandle);
        }
        public EMCPError GetAvatarName(ref string ppStr, ulong ulAvatarHandle)
        {
            IntPtr ppStr_ = IntPtr.Zero;
            var r = ProcTable.GetAvatarName(ref ppStr_, ulAvatarHandle);
            if (r == EMCPError.Error_None)
            {
                ppStr = Marshal.PtrToStringAnsi(ppStr_);
            }
            return r;
        }
        public EMCPError GetAvatarRigidBodies([In, Out]ulong[] vRigidBodies, ref uint punSizeOfRigidBodies, ulong ulAvatarHandle)
        {
            return ProcTable.GetAvatarRigidBodies(vRigidBodies, ref punSizeOfRigidBodies, ulAvatarHandle);
        }
        public EMCPError GetAvatarJointHierarchy(ref string ppStr)
        {
            IntPtr ppStr_ = IntPtr.Zero;
            var r = ProcTable.GetAvatarJointHierarchy(ref ppStr_);
            if (r == EMCPError.Error_None)
            {
                ppStr = Marshal.PtrToStringAnsi(ppStr_);
            }
            return r;
        }
        public EMCPError GetAvatarPostureIndex(ref uint postureIndex, ulong ulAvatarHandle)
        {
            return ProcTable.GetAvatarPostureIndex(ref postureIndex, ulAvatarHandle);
        }
        public EMCPError GetAvatarPostureTimeCode(ref uint hour, ref uint minute, ref uint second, ref uint frame, ref uint rate, ulong ulAvatarHandle)
        {
            return ProcTable.GetAvatarPostureTimeCode(ref hour, ref minute, ref second, ref frame, ref rate, ulAvatarHandle);
        }
        public EMCPError GetAvatarPostureTime(ref uint hour, ref uint minute, [In, Out]uint[] second, ref uint millisecond, ulong ulAvatarHandle)
        {
            return ProcTable.GetAvatarPostureTime(ref hour, ref minute, second, ref millisecond, ulAvatarHandle);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPAvatar_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarIndex(ref uint index, ulong ulAvatarHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarIndex GetAvatarIndex;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarRootJoint(ref ulong pJointHandle, ulong ulAvatarHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarRootJoint GetAvatarRootJoint;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarJoints([In, Out]ulong[] pJointHandle, ref uint punSizeOfJointHandle, ulong ulAvatarHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarJoints GetAvatarJoints;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarJointByName(string name, ref ulong pJointHandle, ulong ulAvatarHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarJointByName GetAvatarJointByName;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarName(ref IntPtr ppStr, ulong ulAvatarHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarName GetAvatarName;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarRigidBodies([In, Out]ulong[] vRigidBodies, ref uint punSizeOfRigidBodies, ulong ulAvatarHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarRigidBodies GetAvatarRigidBodies;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarJointHierarchy(ref IntPtr ppStr);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarJointHierarchy GetAvatarJointHierarchy;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarPostureIndex(ref uint postureIndex, ulong ulAvatarHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarPostureIndex GetAvatarPostureIndex;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarPostureTimeCode(ref uint hour, ref uint minute, ref uint second, ref uint frame, ref uint rate, ulong ulAvatarHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarPostureTimeCode GetAvatarPostureTimeCode;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetAvatarPostureTime(ref uint hour, ref uint minute, [In, Out]uint[] second, ref uint millisecond, ulong ulAvatarHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetAvatarPostureTime GetAvatarPostureTime;

        };
        private MCPAvatar_ProcTable ProcTable;
        private static IMCPAvatar avatar;
        private const string IMCPAvatar_Version = "IMCPAvatar_004";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPAvatar()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPAvatar_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPAvatar_Version);
            }
            ProcTable = (MCPAvatar_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPAvatar_ProcTable));
        }
    };
    public class IMCPMarker
    {
        static public IMCPMarker Marker
        {
            get
            {
                if (null == marker)
                {
                    marker = new IMCPMarker();
                }
                return marker;
            }
        }
        public EMCPError GetMarkerPosition(ref float x, ref float y, ref float z, ulong handle)
        {
            return ProcTable.GetMarkerPosition(ref x, ref y, ref z, handle);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPMarker_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetMarkerPosition(ref float x, ref float y, ref float z, ulong handle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetMarkerPosition GetMarkerPosition;

        };
        private MCPMarker_ProcTable ProcTable;
        private static IMCPMarker marker;
        private const string IMCPMarker_Version = "IMCPMarker_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPMarker()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPMarker_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPMarker_Version);
            }
            ProcTable = (MCPMarker_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPMarker_ProcTable));
        }
    };
    public class IMCPPWR
    {
        static public IMCPPWR PWR
        {
            get
            {
                if (null == pWR)
                {
                    pWR = new IMCPPWR();
                }
                return pWR;
            }
        }
        public EMCPError GetPWRId(ref uint id, ulong handle)
        {
            return ProcTable.GetPWRId(ref id, handle);
        }
        public EMCPError GetPWRStatus(ref int status, ulong handle)
        {
            return ProcTable.GetPWRStatus(ref status, handle);
        }
        public EMCPError GetPWRPosition(ref float x, ref float y, ref float z, ulong handle)
        {
            return ProcTable.GetPWRPosition(ref x, ref y, ref z, handle);
        }
        public EMCPError GetPWRQuaternion(ref float x, ref float y, ref float z, ref float w, ulong handle)
        {
            return ProcTable.GetPWRQuaternion(ref x, ref y, ref z, ref w, handle);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPPWR_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetPWRId(ref uint id, ulong handle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetPWRId GetPWRId;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetPWRStatus(ref int status, ulong handle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetPWRStatus GetPWRStatus;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetPWRPosition(ref float x, ref float y, ref float z, ulong handle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetPWRPosition GetPWRPosition;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetPWRQuaternion(ref float x, ref float y, ref float z, ref float w, ulong handle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetPWRQuaternion GetPWRQuaternion;

        };
        private MCPPWR_ProcTable ProcTable;
        private static IMCPPWR pWR;
        private const string IMCPPWR_Version = "IMCPPWR_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPPWR()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPPWR_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPPWR_Version);
            }
            ProcTable = (MCPPWR_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPPWR_ProcTable));
        }
    };
    public class IMCPAliceHub
    {
        static public IMCPAliceHub AliceHub
        {
            get
            {
                if (null == aliceHub)
                {
                    aliceHub = new IMCPAliceHub();
                }
                return aliceHub;
            }
        }
        public EMCPError GetSensorModuleList([In, Out]ulong[] pHandles, ref uint nHandles)
        {
            return ProcTable.GetSensorModuleList(pHandles, ref nHandles);
        }
        public EMCPError GetSensorModuleTimestamp(ref ulong timestamp)
        {
            return ProcTable.GetSensorModuleTimestamp(ref timestamp);
        }
        public EMCPError GetMarkerList([In, Out]ulong[] pHandles, ref uint nHandles)
        {
            return ProcTable.GetMarkerList(pHandles, ref nHandles);
        }
        public EMCPError GetMarkerTimestamp(ref ulong timestamp)
        {
            return ProcTable.GetMarkerTimestamp(ref timestamp);
        }
        public EMCPError GetRigidBodyList([In, Out]ulong[] pHandles, ref uint nHandles)
        {
            return ProcTable.GetRigidBodyList(pHandles, ref nHandles);
        }
        public EMCPError GetRigidBodyTimestamp(ref ulong timestamp)
        {
            return ProcTable.GetRigidBodyTimestamp(ref timestamp);
        }
        public EMCPError GetPWRList([In, Out]ulong[] pHandles, ref uint nHandles)
        {
            return ProcTable.GetPWRList(pHandles, ref nHandles);
        }
        public EMCPError GetPWRTimestamp(ref ulong timestamp)
        {
            return ProcTable.GetPWRTimestamp(ref timestamp);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPAliceHub_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModuleList([In, Out]ulong[] pHandles, ref uint nHandles);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModuleList GetSensorModuleList;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetSensorModuleTimestamp(ref ulong timestamp);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetSensorModuleTimestamp GetSensorModuleTimestamp;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetMarkerList([In, Out]ulong[] pHandles, ref uint nHandles);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetMarkerList GetMarkerList;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetMarkerTimestamp(ref ulong timestamp);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetMarkerTimestamp GetMarkerTimestamp;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetRigidBodyList([In, Out]ulong[] pHandles, ref uint nHandles);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetRigidBodyList GetRigidBodyList;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetRigidBodyTimestamp(ref ulong timestamp);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetRigidBodyTimestamp GetRigidBodyTimestamp;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetPWRList([In, Out]ulong[] pHandles, ref uint nHandles);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetPWRList GetPWRList;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetPWRTimestamp(ref ulong timestamp);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetPWRTimestamp GetPWRTimestamp;

        };
        private MCPAliceHub_ProcTable ProcTable;
        private static IMCPAliceHub aliceHub;
        private const string IMCPAliceHub_Version = "IMCPAliceHub_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPAliceHub()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPAliceHub_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPAliceHub_Version);
            }
            ProcTable = (MCPAliceHub_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPAliceHub_ProcTable));
        }
    };
    public enum EMCPCommand
    {
        CommandStartCapture=0,
        CommandStopCapture=1,
        CommandZeroPosition=2,
        CommandCalibrateMotion=3,
        CommandStartRecored=4,
        CommandStopRecored=5,
        CommandResumeOriginalPosture=6
    };
    public enum EMCPCalibrateMotionFlag
    {
        CalibrateMotionFlag_AutoNextStep=0,
        CalibrateMotionFlag_ManualNextStep=1
    };
    public enum EMCPCalibrateMotionOperation
    {
        CalibrateMotionOperation_Next=0,
        CalibrateMotionOperation_Cancel=1
    };
    public enum EMCPCommandStopCatpureExtraFlag
    {
        StopCatpureExtraFlag_SensorsModulesPowerOff=0,
        StopCatpureExtraFlag_SensorsModulesHibernate=1
    };
    public enum EMCPCommandExtraLong
    {
        CommandExtraLong_DeviceRadio=0,
        CommandExtraLong_AvatarName=1,
        CommandExtraLong_Extra0=0,
        CommandExtraLong_Extra1=1,
        CommandExtraLong_Extra2=2,
        CommandExtraLong_Extra3=3
    };
    public enum EMCPCommandProgress
    {
        CommandProgress_CalibrateMotion=0
    };
    public class IMCPCommand
    {
        static public IMCPCommand Command
        {
            get
            {
                if (null == command)
                {
                    command = new IMCPCommand();
                }
                return command;
            }
        }
        public EMCPError CreateCommand(uint cmd, ref ulong handle_)
        {
            return ProcTable.CreateCommand(cmd, ref handle_);
        }
        public EMCPError SetCommandExtraFlags(uint extraFlags, ulong handle_)
        {
            return ProcTable.SetCommandExtraFlags(extraFlags, handle_);
        }
        public EMCPError SetCommandExtraLong(uint extraLongIndex, IntPtr extraLong, ulong handle_)
        {
            return ProcTable.SetCommandExtraLong(extraLongIndex, extraLong, handle_);
        }
        public EMCPError GetCommandResultMessage(ref string pMsg, ulong handle_)
        {
            IntPtr pMsg_ = IntPtr.Zero;
            var r = ProcTable.GetCommandResultMessage(ref pMsg_, handle_);
            if (r == EMCPError.Error_None)
            {
                pMsg = Marshal.PtrToStringAnsi(pMsg_);
            }
            return r;
        }
        public EMCPError GetCommandResultCode(ref uint pResCode, ulong handle_)
        {
            return ProcTable.GetCommandResultCode(ref pResCode, handle_);
        }
        public EMCPError GetCommandProgress(uint progress, IntPtr extra, ulong handle_)
        {
            return ProcTable.GetCommandProgress(progress, extra, handle_);
        }
        public EMCPError DestroyCommand(ulong handle_)
        {
            return ProcTable.DestroyCommand(handle_);
        }
        public EMCPError DuplicateCommand(ref ulong duplicateCmd, ulong handle_)
        {
            return ProcTable.DuplicateCommand(ref duplicateCmd, handle_);
        }
        public EMCPError AddCommandExtraLong(uint extraLongIndex, IntPtr extraLong, ulong handle_)
        {
            return ProcTable.AddCommandExtraLong(extraLongIndex, extraLong, handle_);
        }
        public EMCPError GetCommandExtraLong(uint extraLongIndex, ref IntPtr pExtraLong, ulong handle_)
        {
            IntPtr pExtraLong_ = IntPtr.Zero;
            var r = ProcTable.GetCommandExtraLong(extraLongIndex, ref pExtraLong_, handle_);
            if (r == EMCPError.Error_None)
            {
                pExtraLong = Marshal.ReadIntPtr(pExtraLong_);
            }
            return r;
        }
        public EMCPError RemoveCommandExtraLong(uint extraLongIndex, ulong handle_)
        {
            return ProcTable.RemoveCommandExtraLong(extraLongIndex, handle_);
        }
        public EMCPError GetCommandTag(ref uint pTag, ulong handle_)
        {
            return ProcTable.GetCommandTag(ref pTag, handle_);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPCommand_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _CreateCommand(uint cmd, ref ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _CreateCommand CreateCommand;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetCommandExtraFlags(uint extraFlags, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetCommandExtraFlags SetCommandExtraFlags;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetCommandExtraLong(uint extraLongIndex, IntPtr extraLong, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetCommandExtraLong SetCommandExtraLong;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCommandResultMessage(ref IntPtr pMsg, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCommandResultMessage GetCommandResultMessage;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCommandResultCode(ref uint pResCode, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCommandResultCode GetCommandResultCode;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCommandProgress(uint progress, IntPtr extra, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCommandProgress GetCommandProgress;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _DestroyCommand(ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _DestroyCommand DestroyCommand;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _DuplicateCommand(ref ulong duplicateCmd, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _DuplicateCommand DuplicateCommand;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _AddCommandExtraLong(uint extraLongIndex, IntPtr extraLong, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _AddCommandExtraLong AddCommandExtraLong;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCommandExtraLong(uint extraLongIndex, ref IntPtr pExtraLong, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCommandExtraLong GetCommandExtraLong;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _RemoveCommandExtraLong(uint extraLongIndex, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _RemoveCommandExtraLong RemoveCommandExtraLong;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCommandTag(ref uint pTag, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCommandTag GetCommandTag;

        };
        private MCPCommand_ProcTable ProcTable;
        private static IMCPCommand command;
        private const string IMCPCommand_Version = "IMCPCommand_002";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPCommand()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPCommand_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPCommand_Version);
            }
            ProcTable = (MCPCommand_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPCommand_ProcTable));
        }
    };
    public enum EMCPCalibrateMotionProgressStep
    {
        CalibrateMotionProgressStep_Prepare=0,
        CalibrateMotionProgressStep_Countdown=1,
        CalibrateMotionProgressStep_Progress=2
    };
    public class IMCPCalibrateMotionProgress
    {
        static public IMCPCalibrateMotionProgress CalibrateMotionProgress
        {
            get
            {
                if (null == calibrateMotionProgress)
                {
                    calibrateMotionProgress = new IMCPCalibrateMotionProgress();
                }
                return calibrateMotionProgress;
            }
        }
        public EMCPError GetCalibrateMotionProgressCountOfSupportPoses(ref uint pCount, ulong handle_)
        {
            return ProcTable.GetCalibrateMotionProgressCountOfSupportPoses(ref pCount, handle_);
        }
        public EMCPError GetCalibrateMotionProgressNameOfSupportPose(ref char name, ref uint pLenOfName, uint index, ulong handle_)
        {
            return ProcTable.GetCalibrateMotionProgressNameOfSupportPose(ref name, ref pLenOfName, index, handle_);
        }
        public EMCPError GetCalibrateMotionProgressStepOfPose(ref uint pStep, string name, ulong handle_)
        {
            return ProcTable.GetCalibrateMotionProgressStepOfPose(ref pStep, name, handle_);
        }
        public EMCPError GetCalibrateMotionProgressCountdownOfPose(ref uint pCountdown, string name, ulong handle_)
        {
            return ProcTable.GetCalibrateMotionProgressCountdownOfPose(ref pCountdown, name, handle_);
        }
        public EMCPError GetCalibrateMotionProgressProgressOfPose(ref uint pProgress, string name, ulong handle_)
        {
            return ProcTable.GetCalibrateMotionProgressProgressOfPose(ref pProgress, name, handle_);
        }
        public EMCPError GetCalibrateMotionProgressStepOfCurrentPose(ref uint pStep, ref char name, ref uint pLenOfName, ulong handle_)
        {
            return ProcTable.GetCalibrateMotionProgressStepOfCurrentPose(ref pStep, ref name, ref pLenOfName, handle_);
        }
        public EMCPError GetCalibrateMotionProgressCountdownOfCurrentPose(ref uint pCountdown, ref char name, ref uint pLenOfName, ulong handle_)
        {
            return ProcTable.GetCalibrateMotionProgressCountdownOfCurrentPose(ref pCountdown, ref name, ref pLenOfName, handle_);
        }
        public EMCPError GetCalibrateMotionProgressProgressOfCurrentPose(ref uint pProgress, ref char name, ref uint pLenOfName, ulong handle_)
        {
            return ProcTable.GetCalibrateMotionProgressProgressOfCurrentPose(ref pProgress, ref name, ref pLenOfName, handle_);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPCalibrateMotionProgress_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCalibrateMotionProgressCountOfSupportPoses(ref uint pCount, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCalibrateMotionProgressCountOfSupportPoses GetCalibrateMotionProgressCountOfSupportPoses;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCalibrateMotionProgressNameOfSupportPose(ref char name, ref uint pLenOfName, uint index, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCalibrateMotionProgressNameOfSupportPose GetCalibrateMotionProgressNameOfSupportPose;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCalibrateMotionProgressStepOfPose(ref uint pStep, string name, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCalibrateMotionProgressStepOfPose GetCalibrateMotionProgressStepOfPose;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCalibrateMotionProgressCountdownOfPose(ref uint pCountdown, string name, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCalibrateMotionProgressCountdownOfPose GetCalibrateMotionProgressCountdownOfPose;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCalibrateMotionProgressProgressOfPose(ref uint pProgress, string name, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCalibrateMotionProgressProgressOfPose GetCalibrateMotionProgressProgressOfPose;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCalibrateMotionProgressStepOfCurrentPose(ref uint pStep, ref char name, ref uint pLenOfName, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCalibrateMotionProgressStepOfCurrentPose GetCalibrateMotionProgressStepOfCurrentPose;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCalibrateMotionProgressCountdownOfCurrentPose(ref uint pCountdown, ref char name, ref uint pLenOfName, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCalibrateMotionProgressCountdownOfCurrentPose GetCalibrateMotionProgressCountdownOfCurrentPose;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCalibrateMotionProgressProgressOfCurrentPose(ref uint pProgress, ref char name, ref uint pLenOfName, ulong handle_);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCalibrateMotionProgressProgressOfCurrentPose GetCalibrateMotionProgressProgressOfCurrentPose;

        };
        private MCPCalibrateMotionProgress_ProcTable ProcTable;
        private static IMCPCalibrateMotionProgress calibrateMotionProgress;
        private const string IMCPCalibrateMotionProgress_Version = "IMCPCalibrateMotionProgress_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPCalibrateMotionProgress()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPCalibrateMotionProgress_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPCalibrateMotionProgress_Version);
            }
            ProcTable = (MCPCalibrateMotionProgress_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPCalibrateMotionProgress_ProcTable));
        }
    };
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_Reserved_t
    {
        public ulong reserved0;
        public ulong reserved1;
        public ulong reserved2;
        public ulong reserved3;
        public ulong reserved4;
        public ulong reserved5;
    };
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_MotionData_t
    {
        public ulong avatarHandle;
    };
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_SystemError_t
    {
        public EMCPError error;
        public ulong info0;
    };
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_SensorModuleData_t
    {
        public ulong _sensorModuleHandle;
    };
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_TrackerData_t
    {
        public ulong _trackerHandle;
    };
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_MarkerData_t
    {
        public ulong _markerHandle;
    };
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_PWRData_t
    {
        public ulong _pwrHandle;
    };
    public enum EMCPNotify
    {
        Notify_RecordStarted=0,
        Notify_RecordStoped=1,
        Notify_RecordFinished=2
    };
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_NotifyData_t
    {
        public EMCPNotify _notify;
        public ulong _notifyHandle;
    };
    public enum EMCPReplay
    {
        MCPReplay_Response=0,
        MCPReplay_Running=1,
        MCPReplay_Result=2
    };
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_CommandRespond_t
    {
        public ulong _commandHandle;
        public EMCPReplay _replay;
    };
    [StructLayout(LayoutKind.Explicit)]
    public struct MCPEventData_t
    {
        [FieldOffset(0)] public MCPEvent_Reserved_t reserved;
        [FieldOffset(0)] public MCPEvent_MotionData_t motionData;
        [FieldOffset(0)] public MCPEvent_SystemError_t systemError;
        [FieldOffset(0)] public MCPEvent_SensorModuleData_t sensorModuleData;
        [FieldOffset(0)] public MCPEvent_TrackerData_t trackerData;
        [FieldOffset(0)] public MCPEvent_CommandRespond_t commandRespond;
        [FieldOffset(0)] public MCPEvent_MarkerData_t markerData;
        [FieldOffset(0)] public MCPEvent_PWRData_t pwrData;
        [FieldOffset(0)] public MCPEvent_NotifyData_t notifyData;
    };
    public enum EMCPEventType
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
    [StructLayout(LayoutKind.Sequential)]
    public struct MCPEvent_t
    {
        public uint size;
        public EMCPEventType eventType;
        public double fTimestamp;
        public MCPEventData_t eventData;
    };
    public enum EMCPBvhRotation
    {
        BvhRotation_XYZ=0,
        BvhRotation_XZY=1,
        BvhRotation_YXZ=2,
        BvhRotation_YZX=3,
        BvhRotation_ZXY=4,
        BvhRotation_ZYX=5
    };
    public enum EMCPBvhData
    {
        BvhDataType_String=0,
        BvhDataType_BinaryWithOldFrameHeader=1,
        BvhDataType_Binary=2,
        BvhDataType_Mask_LegacyHumanHierarchy=4
    };
    public enum EMCPBvhTransformation
    {
        BvhTransformation_Disable=0,
        BvhTransformation_Enable=1
    };
    public class IMCPSettings
    {
        static public IMCPSettings Settings
        {
            get
            {
                if (null == settings)
                {
                    settings = new IMCPSettings();
                }
                return settings;
            }
        }
        public EMCPError CreateSettings(ref ulong pSettingsHandle)
        {
            return ProcTable.CreateSettings(ref pSettingsHandle);
        }
        public EMCPError DestroySettings(ulong ulSettingsHandle)
        {
            return ProcTable.DestroySettings(ulSettingsHandle);
        }
        public EMCPError SetSettingsUDP(ushort localPort, ulong ulSettingsHandle)
        {
            return ProcTable.SetSettingsUDP(localPort, ulSettingsHandle);
        }
        public EMCPError SetSettingsTCP(string serverIp, ushort serverPort, ulong ulSettingsHandle)
        {
            return ProcTable.SetSettingsTCP(serverIp, serverPort, ulSettingsHandle);
        }
        public EMCPError SetSettingsBvhRotation(EMCPBvhRotation bvhRotation, ulong ulSettingsHandle)
        {
            return ProcTable.SetSettingsBvhRotation(bvhRotation, ulSettingsHandle);
        }
        public EMCPError SetSettingsBvhTransformation(EMCPBvhTransformation bvhTransformation, ulong ulSettingsHandle)
        {
            return ProcTable.SetSettingsBvhTransformation(bvhTransformation, ulSettingsHandle);
        }
        public EMCPError SetSettingsBvhData(EMCPBvhData bvhData, ulong ulSettingsHandle)
        {
            return ProcTable.SetSettingsBvhData(bvhData, ulSettingsHandle);
        }
        public EMCPError SetSettingsCalcData(ulong ulSettingsHandle)
        {
            return ProcTable.SetSettingsCalcData(ulSettingsHandle);
        }
        public EMCPError SetSettingsUDPServer(string serverIp, ushort serverPort, ulong ulSettingsHandle)
        {
            return ProcTable.SetSettingsUDPServer(serverIp, serverPort, ulSettingsHandle);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPSettings_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _CreateSettings(ref ulong pSettingsHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _CreateSettings CreateSettings;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _DestroySettings(ulong ulSettingsHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _DestroySettings DestroySettings;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetSettingsUDP(ushort localPort, ulong ulSettingsHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetSettingsUDP SetSettingsUDP;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetSettingsTCP(string serverIp, ushort serverPort, ulong ulSettingsHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetSettingsTCP SetSettingsTCP;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetSettingsBvhRotation(EMCPBvhRotation bvhRotation, ulong ulSettingsHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetSettingsBvhRotation SetSettingsBvhRotation;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetSettingsBvhTransformation(EMCPBvhTransformation bvhTransformation, ulong ulSettingsHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetSettingsBvhTransformation SetSettingsBvhTransformation;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetSettingsBvhData(EMCPBvhData bvhData, ulong ulSettingsHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetSettingsBvhData SetSettingsBvhData;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetSettingsCalcData(ulong ulSettingsHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetSettingsCalcData SetSettingsCalcData;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetSettingsUDPServer(string serverIp, ushort serverPort, ulong ulSettingsHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetSettingsUDPServer SetSettingsUDPServer;

        };
        private MCPSettings_ProcTable ProcTable;
        private static IMCPSettings settings;
        private const string IMCPSettings_Version = "IMCPSettings_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPSettings()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPSettings_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPSettings_Version);
            }
            ProcTable = (MCPSettings_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPSettings_ProcTable));
        }
    };
    public enum EMCPUpVector
    {
        UpVector_XAxis=1,
        UpVector_YAxis=2,
        UpVector_ZAxis=3
    };
    public enum EMCPFrontVector
    {
        FrontVector_ParityEven=1,
        FrontVector_ParityOdd=2
    };
    public enum EMCPCoordSystem
    {
        CoordSystem_RightHanded=0,
        CoordSystem_LeftHanded=1
    };
    public enum EMCPRotatingDirection
    {
        RotatingDirection_Clockwise=0,
        RotatingDirection_CounterClockwise=1
    };
    public enum EMCPPreDefinedRenderSettings
    {
        PreDefinedRenderSettings_Default=0,
        PreDefinedRenderSettings_UnrealEngine=1,
        PreDefinedRenderSettings_Unity3D=2,
        PreDefinedRenderSettings_Count=3
    };
    public enum EMCPUnit
    {
        Unit_Centimeter=0,
        Uint_Meter=1
    };
    public class IMCPRenderSettings
    {
        static public IMCPRenderSettings RenderSettings
        {
            get
            {
                if (null == renderSettings)
                {
                    renderSettings = new IMCPRenderSettings();
                }
                return renderSettings;
            }
        }
        public EMCPError CreateRenderSettings(ref ulong pRenderSettings)
        {
            return ProcTable.CreateRenderSettings(ref pRenderSettings);
        }
        public EMCPError GetPreDefRenderSettings(EMCPPreDefinedRenderSettings preDefinedRenderSettings, ref ulong pRenderSettings)
        {
            return ProcTable.GetPreDefRenderSettings(preDefinedRenderSettings, ref pRenderSettings);
        }
        public EMCPError SetUpVector(EMCPUpVector upVector, int sign, ulong renderSettings)
        {
            return ProcTable.SetUpVector(upVector, sign, renderSettings);
        }
        public EMCPError GetUpVector(ref EMCPUpVector pUpVector, ref int sign, ulong renderSettings)
        {
            return ProcTable.GetUpVector(ref pUpVector, ref sign, renderSettings);
        }
        public EMCPError SetFrontVector(EMCPFrontVector frontVector, int sign, ulong renderSettings)
        {
            return ProcTable.SetFrontVector(frontVector, sign, renderSettings);
        }
        public EMCPError GetFrontVector(ref EMCPFrontVector pFrontVector, ref int sign, ulong renderSettings)
        {
            return ProcTable.GetFrontVector(ref pFrontVector, ref sign, renderSettings);
        }
        public EMCPError SetCoordSystem(EMCPCoordSystem coordSystem, ulong renderSettings)
        {
            return ProcTable.SetCoordSystem(coordSystem, renderSettings);
        }
        public EMCPError GetCoordSystem(ref EMCPCoordSystem pCoordSystem, ulong renderSettings)
        {
            return ProcTable.GetCoordSystem(ref pCoordSystem, renderSettings);
        }
        public EMCPError SetRotatingDirection(EMCPRotatingDirection rotatingDirection, ulong renderSettings)
        {
            return ProcTable.SetRotatingDirection(rotatingDirection, renderSettings);
        }
        public EMCPError GetRotatingDirection(ref EMCPRotatingDirection pRotatingDirection, ulong renderSettings)
        {
            return ProcTable.GetRotatingDirection(ref pRotatingDirection, renderSettings);
        }
        public EMCPError SetUnit(EMCPUnit mcpUnit, ulong renderSettings)
        {
            return ProcTable.SetUnit(mcpUnit, renderSettings);
        }
        public EMCPError GetUnit(ref EMCPUnit mcpUnit, ulong renderSettings)
        {
            return ProcTable.GetUnit(ref mcpUnit, renderSettings);
        }
        public EMCPError DestroyRenderSettings(ulong renderSettings)
        {
            return ProcTable.DestroyRenderSettings(renderSettings);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPRenderSettings_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _CreateRenderSettings(ref ulong pRenderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _CreateRenderSettings CreateRenderSettings;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetPreDefRenderSettings(EMCPPreDefinedRenderSettings preDefinedRenderSettings, ref ulong pRenderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetPreDefRenderSettings GetPreDefRenderSettings;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetUpVector(EMCPUpVector upVector, int sign, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetUpVector SetUpVector;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetUpVector(ref EMCPUpVector pUpVector, ref int sign, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetUpVector GetUpVector;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetFrontVector(EMCPFrontVector frontVector, int sign, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetFrontVector SetFrontVector;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetFrontVector(ref EMCPFrontVector pFrontVector, ref int sign, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetFrontVector GetFrontVector;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetCoordSystem(EMCPCoordSystem coordSystem, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetCoordSystem SetCoordSystem;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetCoordSystem(ref EMCPCoordSystem pCoordSystem, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetCoordSystem GetCoordSystem;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetRotatingDirection(EMCPRotatingDirection rotatingDirection, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetRotatingDirection SetRotatingDirection;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetRotatingDirection(ref EMCPRotatingDirection pRotatingDirection, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetRotatingDirection GetRotatingDirection;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetUnit(EMCPUnit mcpUnit, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetUnit SetUnit;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetUnit(ref EMCPUnit mcpUnit, ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetUnit GetUnit;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _DestroyRenderSettings(ulong renderSettings);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _DestroyRenderSettings DestroyRenderSettings;

        };
        private MCPRenderSettings_ProcTable ProcTable;
        private static IMCPRenderSettings renderSettings;
        private const string IMCPRenderSettings_Version = "IMCPRenderSettings_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPRenderSettings()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPRenderSettings_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPRenderSettings_Version);
            }
            ProcTable = (MCPRenderSettings_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPRenderSettings_ProcTable));
        }
    };
    public delegate void MCPEventHandleProc(MCPEvent_t ev);
    public class IMCPApplication
    {
        static public IMCPApplication Application
        {
            get
            {
                if (null == application)
                {
                    application = new IMCPApplication();
                }
                return application;
            }
        }
        public EMCPError CreateApplication(ref ulong ulApplicationHandle)
        {
            return ProcTable.CreateApplication(ref ulApplicationHandle);
        }
        public EMCPError DestroyApplication(ulong ulApplicationHandle)
        {
            return ProcTable.DestroyApplication(ulApplicationHandle);
        }
        public EMCPError SetApplicationSettings(ulong ulSettingsHandle, ulong ulApplicationHandle)
        {
            return ProcTable.SetApplicationSettings(ulSettingsHandle, ulApplicationHandle);
        }
        public EMCPError SetApplicationRenderSettings(ulong ulRenderSettings, ulong ulApplicationHandle)
        {
            return ProcTable.SetApplicationRenderSettings(ulRenderSettings, ulApplicationHandle);
        }
        public EMCPError OpenApplication(ulong ulApplicationHandle)
        {
            return ProcTable.OpenApplication(ulApplicationHandle);
        }
        public EMCPError EnableApplicationCacheEvents(ulong ulApplicationHandle)
        {
            return ProcTable.EnableApplicationCacheEvents(ulApplicationHandle);
        }
        public EMCPError DisableApplicationCacheEvents(ulong ulApplicationHandle)
        {
            return ProcTable.DisableApplicationCacheEvents(ulApplicationHandle);
        }
        public EMCPError ApplicationCacheEventsIsEnabled(ref bool isEnabled, ulong ulApplicationHandle)
        {
            return ProcTable.ApplicationCacheEventsIsEnabled(ref isEnabled, ulApplicationHandle);
        }
        public EMCPError CloseApplication(ulong ulApplicationHandle)
        {
            return ProcTable.CloseApplication(ulApplicationHandle);
        }
        public EMCPError GetApplicationRigidBodies(ref ulong pRigidBodyHandle, ref uint punRigidBodyHandleSize, ulong ulApplicationHandle)
        {
            return ProcTable.GetApplicationRigidBodies(ref pRigidBodyHandle, ref punRigidBodyHandleSize, ulApplicationHandle);
        }
        public EMCPError GetApplicationAvatars([In, Out]ulong[] pAvatarHandle, ref uint punAvatarHandle, ulong ulApplicationHandle)
        {
            return ProcTable.GetApplicationAvatars(pAvatarHandle, ref punAvatarHandle, ulApplicationHandle);
        }
        public EMCPError PollApplicationNextEvent([In, Out]MCPEvent_t[] pEvent, ref uint punSizeOfEvent, ulong ulApplicationHandle)
        {
            return ProcTable.PollApplicationNextEvent(pEvent, ref punSizeOfEvent, ulApplicationHandle);
        }
        public EMCPError GetApplicationSensorModules([In, Out]ulong[] pSensorModuleHandle, ref uint punSensorModuleHandle, ulong ulApplicationHandle)
        {
            return ProcTable.GetApplicationSensorModules(pSensorModuleHandle, ref punSensorModuleHandle, ulApplicationHandle);
        }
        public EMCPError GetApplicationTrackers([In, Out]ulong[] pTrackerHandle, ref uint punTrackerHandle, ulong ulApplicationHandle)
        {
            return ProcTable.GetApplicationTrackers(pTrackerHandle, ref punTrackerHandle, ulApplicationHandle);
        }
        public EMCPError QueuedServerCommand(ulong cmdHandle, ulong ulApplicationHandle)
        {
            return ProcTable.QueuedServerCommand(cmdHandle, ulApplicationHandle);
        }
        public EMCPError RegisterEventHandler(MCPEventHandleProc eventHandleProc, ulong ulApplicationHandle)
        {
            lock (eventHandleProcList)
            {
                eventHandleProcList.Add(eventHandleProc);
            }
            if (eventHandleProcList.Count == 1)
            {
                var handle = GCHandle.Alloc(this, GCHandleType.Pinned);
                return ProcTable.RegisterEventHandler(mcpEventHandleProc, GCHandle.ToIntPtr(handle), ulApplicationHandle);
            }
            else
            {
                return EMCPError.Error_None;
            }
        }
        public EMCPError UnregisterEventHandler(MCPEventHandleProc eventHandleProc, ulong ulApplicationHandle)
        {
            lock (eventHandleProcList)
            {
                eventHandleProcList.Remove(eventHandleProc);
            }
            if (eventHandleProcList.Count > 0)
            {
                return EMCPError.Error_None;
            }
            else
            {
                IntPtr userData = new IntPtr();
                var r = ProcTable.UnregisterEventHandler(mcpEventHandleProc, ref userData, ulApplicationHandle);
                if (r == EMCPError.Error_None)
                {
                    GCHandle.FromIntPtr(userData).Free();
                }
                return r;
            }
        }
        internal static void mcpEventHandleProc(IntPtr ev, IntPtr userData)
        {
            var self_ = (IMCPApplication)GCHandle.FromIntPtr(userData).Target;
            var eve = Marshal.PtrToStructure<MCPEvent_t>(ev);
            lock (self_.eventHandleProcList)
            {
                foreach (var e in self_.eventHandleProcList)
                {
                    e(eve);
                }
            }
        }

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        internal delegate void _MCPEventHandleProc(IntPtr ev, IntPtr userData);

        [StructLayout(LayoutKind.Sequential)]
        private struct MCPApplication_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _CreateApplication(ref ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _CreateApplication CreateApplication;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _DestroyApplication(ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _DestroyApplication DestroyApplication;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetApplicationSettings(ulong ulSettingsHandle, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetApplicationSettings SetApplicationSettings;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _SetApplicationRenderSettings(ulong ulRenderSettings, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _SetApplicationRenderSettings SetApplicationRenderSettings;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _OpenApplication(ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _OpenApplication OpenApplication;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _EnableApplicationCacheEvents(ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _EnableApplicationCacheEvents EnableApplicationCacheEvents;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _DisableApplicationCacheEvents(ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _DisableApplicationCacheEvents DisableApplicationCacheEvents;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _ApplicationCacheEventsIsEnabled(ref bool isEnabled, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _ApplicationCacheEventsIsEnabled ApplicationCacheEventsIsEnabled;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _CloseApplication(ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _CloseApplication CloseApplication;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetApplicationRigidBodies(ref ulong pRigidBodyHandle, ref uint punRigidBodyHandleSize, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetApplicationRigidBodies GetApplicationRigidBodies;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetApplicationAvatars([In, Out]ulong[] pAvatarHandle, ref uint punAvatarHandle, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetApplicationAvatars GetApplicationAvatars;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _PollApplicationNextEvent([In, Out]MCPEvent_t[] pEvent, ref uint punSizeOfEvent, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _PollApplicationNextEvent PollApplicationNextEvent;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetApplicationSensorModules([In, Out]ulong[] pSensorModuleHandle, ref uint punSensorModuleHandle, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetApplicationSensorModules GetApplicationSensorModules;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _GetApplicationTrackers([In, Out]ulong[] pTrackerHandle, ref uint punTrackerHandle, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _GetApplicationTrackers GetApplicationTrackers;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _QueuedServerCommand(ulong cmdHandle, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _QueuedServerCommand QueuedServerCommand;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _RegisterEventHandler(_MCPEventHandleProc eventHandleProc, IntPtr userData, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _RegisterEventHandler RegisterEventHandler;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _UnregisterEventHandler(_MCPEventHandleProc eventHandleProc, ref IntPtr userData, ulong ulApplicationHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _UnregisterEventHandler UnregisterEventHandler;

        };
        private MCPApplication_ProcTable ProcTable;
        private static IMCPApplication application;
        private const string IMCPApplication_Version = "IMCPApplication_004";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPApplication()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPApplication_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPApplication_Version);
            }
            ProcTable = (MCPApplication_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPApplication_ProcTable));
        }
    };
    public class IMCPRecordNotify
    {
        static public IMCPRecordNotify RecordNotify
        {
            get
            {
                if (null == recordNotify)
                {
                    recordNotify = new IMCPRecordNotify();
                }
                return recordNotify;
            }
        }
        public EMCPError DuplicateRecordNotify(ref ulong duplicatedRecordNotifyHandle, ulong recordNotifyHandle)
        {
            return ProcTable.DuplicateRecordNotify(ref duplicatedRecordNotifyHandle, recordNotifyHandle);
        }
        public EMCPError RecordNotifyGetTakeName(ref string takeName, ulong recordNotifyHandle)
        {
            IntPtr takeName_ = IntPtr.Zero;
            var r = ProcTable.RecordNotifyGetTakeName(ref takeName_, recordNotifyHandle);
            if (r == EMCPError.Error_None)
            {
                takeName = Marshal.PtrToStringAnsi(takeName_);
            }
            return r;
        }
        public EMCPError RecordNotifyGetTakePath(ref string takePath, ulong recordNotifyHandle)
        {
            IntPtr takePath_ = IntPtr.Zero;
            var r = ProcTable.RecordNotifyGetTakePath(ref takePath_, recordNotifyHandle);
            if (r == EMCPError.Error_None)
            {
                takePath = Marshal.PtrToStringAnsi(takePath_);
            }
            return r;
        }
        public EMCPError RecordNotifyGetTakeSaveDir(ref string takeSaveDir, ulong recordNotifyHandle)
        {
            IntPtr takeSaveDir_ = IntPtr.Zero;
            var r = ProcTable.RecordNotifyGetTakeSaveDir(ref takeSaveDir_, recordNotifyHandle);
            if (r == EMCPError.Error_None)
            {
                takeSaveDir = Marshal.PtrToStringAnsi(takeSaveDir_);
            }
            return r;
        }
        public EMCPError RecordNotifyGetTakeFileSuffix(ref string takeFileSuffix, ulong recordNotifyHandle)
        {
            IntPtr takeFileSuffix_ = IntPtr.Zero;
            var r = ProcTable.RecordNotifyGetTakeFileSuffix(ref takeFileSuffix_, recordNotifyHandle);
            if (r == EMCPError.Error_None)
            {
                takeFileSuffix = Marshal.PtrToStringAnsi(takeFileSuffix_);
            }
            return r;
        }
        public EMCPError DestroyRecordNotify(ulong recordNotifyHandle)
        {
            return ProcTable.DestroyRecordNotify(recordNotifyHandle);
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct MCPRecordNotify_ProcTable
        {
            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _DuplicateRecordNotify(ref ulong duplicatedRecordNotifyHandle, ulong recordNotifyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _DuplicateRecordNotify DuplicateRecordNotify;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _RecordNotifyGetTakeName(ref IntPtr takeName, ulong recordNotifyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _RecordNotifyGetTakeName RecordNotifyGetTakeName;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _RecordNotifyGetTakePath(ref IntPtr takePath, ulong recordNotifyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _RecordNotifyGetTakePath RecordNotifyGetTakePath;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _RecordNotifyGetTakeSaveDir(ref IntPtr takeSaveDir, ulong recordNotifyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _RecordNotifyGetTakeSaveDir RecordNotifyGetTakeSaveDir;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _RecordNotifyGetTakeFileSuffix(ref IntPtr takeFileSuffix, ulong recordNotifyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _RecordNotifyGetTakeFileSuffix RecordNotifyGetTakeFileSuffix;

            [UnmanagedFunctionPointer(CallingConvention.StdCall)]
            internal delegate EMCPError _DestroyRecordNotify(ulong recordNotifyHandle);
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal _DestroyRecordNotify DestroyRecordNotify;

        };
        private MCPRecordNotify_ProcTable ProcTable;
        private static IMCPRecordNotify recordNotify;
        private const string IMCPRecordNotify_Version = "IMCPRecordNotify_001";
        private List<MCPEventHandleProc> eventHandleProcList = new List<MCPEventHandleProc>();
        private IMCPRecordNotify()
        {
            IntPtr pp = IntPtr.Zero;
            var er = Interop.MCPGetGenericInterface(Interop.ProcTable_Prefix + IMCPRecordNotify_Version, ref pp);
            if (er != EMCPError.Error_None)
            {
                throw new InterfaceNotFountException(IMCPRecordNotify_Version);
            }
            ProcTable = (MCPRecordNotify_ProcTable)Marshal.PtrToStructure(pp, typeof(MCPRecordNotify_ProcTable));
        }
    };
    public struct Version
    {
        public static Version libMocapApi
        {
            get
            {
                var libMocapApi = new Version();
                Interop.MCPGetMocapApiVersion(ref libMocapApi.major, ref libMocapApi.minor, ref libMocapApi.build, ref libMocapApi.revision);
                return libMocapApi;
            }
        }
        public static Version mocapApi
        {
            get
            {
                var mocapApi = new Version();
                mocapApi.major = 0;
                mocapApi.minor = 0;
                mocapApi.build = 16;
                mocapApi.revision = 0xc304fc96u;
                return mocapApi;
            }
        }
        public static string libMocapApiString
        {
            get
            {
                return Marshal.PtrToStringAnsi(Interop.MCPGetMocapApiVersionString());
            }
        }
        public static string mocapApiString
        {
            get
            {
                return "0.0.16.c304fc96";
            }
        }
        public uint major;
        public uint minor;
        public uint build;
        public uint revision;
    };
    internal class Utils
    {
        public static IntPtr ToUtf8(string managedString)
        {
            if (managedString == null)
            {
                return IntPtr.Zero;
            }
            int size = System.Text.Encoding.UTF8.GetByteCount(managedString) + 1;
            if (buffer.Length < size) buffer = new byte[size];
            int written = System.Text.Encoding.UTF8.GetBytes(managedString, 0, managedString.Length, buffer, 0);
            buffer[written] = 0x00; // null terminate
            IntPtr nativeUtf8 = Marshal.AllocHGlobal(written + 1);
            Marshal.Copy(buffer, 0, nativeUtf8, written + 1);
            return nativeUtf8;
        }
        private static byte[] buffer = new byte[1024];
    }
    internal class Interop
    {
# if UNITY_IOS
        [DllImportAttribute("__Internal", EntryPoint="MCPGetMocapApiVersionString",CallingConvention=CallingConvention.Cdecl)]
# else
        [DllImportAttribute("MocapApi", EntryPoint="MCPGetMocapApiVersionString",CallingConvention=CallingConvention.Cdecl)]
# endif
        internal static extern IntPtr MCPGetMocapApiVersionString();
# if UNITY_IOS
        [DllImportAttribute("__Internal", EntryPoint="MCPGetMocapApiVersion",CallingConvention=CallingConvention.Cdecl)]
# else
        [DllImportAttribute("MocapApi", EntryPoint="MCPGetMocapApiVersion",CallingConvention=CallingConvention.Cdecl)]
# endif
        internal static extern void MCPGetMocapApiVersion(ref uint major, ref uint minor, ref uint build, ref uint revision);
# if UNITY_IOS
        [DllImportAttribute("__Internal", EntryPoint="MCPGetGenericInterface",CallingConvention=CallingConvention.Cdecl)]
# else
        [DllImportAttribute("MocapApi", EntryPoint="MCPGetGenericInterface",CallingConvention=CallingConvention.Cdecl)]
# endif
        internal static extern EMCPError MCPGetGenericInterface([In, MarshalAs(UnmanagedType.LPStr)] string pchInterfaceVersion, ref IntPtr ppInterface);
        internal const string ProcTable_Prefix = "PROC_TABLE:";
    };
}
