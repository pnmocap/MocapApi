using MocapApi;
using UnityEngine;

public class AvatarController : MonoBehaviour
{
  const string JointPrefix = "Noitom_";
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {

  }

  ulong avatarHandle = 0;

  void transformJointsInternal(ulong jointHandle, Transform jointTranform) {
    Vector3 v = new Vector3();
    if (IMCPJoint.Joint.GetJointLocalPosition(ref v.x, ref v.y, ref v.z, jointHandle) == EMCPError.Error_None)
    {
      EMCPJointTag jointTag = EMCPJointTag.JointTag_Invalid;
      IMCPJoint.Joint.GetJointTag(ref jointTag, jointHandle);
      if (jointTag != EMCPJointTag.JointTag_Hips)
      {
        jointTranform.localPosition = v;
      }
      else {
        var vt = jointTranform.localPosition;
        vt.y = v.y;
        jointTranform.localPosition = vt;
      }
      
    }

    Quaternion q = new Quaternion();
    IMCPJoint.Joint.GetJointLocalRotation(ref q.x, ref q.y, ref q.z, ref q.w, jointHandle);
    jointTranform.localRotation = q;
    uint numberOfChild = 0;
    IMCPJoint.Joint.GetJointChild(null, ref numberOfChild, jointHandle);
    if (numberOfChild > 0)
    {
      ulong[] children = new ulong[numberOfChild];
      IMCPJoint.Joint.GetJointChild(children, ref numberOfChild, jointHandle);
      foreach (var child in children)
      {
        transformJoints(child, jointTranform);
      }
    }
  }

  void transformJoints(ulong jointHandle, Transform parentTransform_)
  {
    string name = "";
    IMCPJoint.Joint.GetJointName(ref name, jointHandle);
    var jointTranform = parentTransform_.Find(JointPrefix + name);
    if (null != jointTranform)
    {
      transformJointsInternal(jointHandle, jointTranform);
    }
    else if (name.IndexOf("InHand") > 0)
    {
      jointTranform = parentTransform_.Find(JointPrefix + name.Replace("InHand", "Hand"));
      if (null != jointTranform)
      {
        transformJointsInternal(jointHandle, jointTranform);
      }
      else {
        Debug.LogError(name);
      }
    }
    else {
      Debug.LogError(name);
    }
  }

  public void updateMotion() {
    ulong rootJoint = 0;
    IMCPAvatar.Avatar.GetAvatarRootJoint(ref rootJoint, avatarHandle);

    transformJoints(rootJoint, transform);
  }
  public void attachAvatar(ulong avatarHandle) {
    this.avatarHandle = avatarHandle;
    updateMotion();
  }
}

