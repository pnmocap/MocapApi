from ctypes import *
from collections import namedtuple
from platform import *
import time
import os

__version__ = "0.1.5"

architecture = machine()
mocap_file_path = None
mocap_file_name = None

if architecture == "arm64" or architecture == "aarch64":
    mocap_file_path = 'lib/arm64'
    mocap_file_name = 'libMocapApi.so'
elif architecture == "x86_64":
    mocap_file_path = 'lib/x86_64'
    mocap_file_name = 'libMocapApi.so'
elif architecture == "AMD64":
    mocap_file_path = "lib/amd64"
    mocap_file_name = "MocapApi.dll"
else:
    raise Exception("Unsupported architecture")

mocap_lib_file = os.path.join(os.path.dirname(__file__), mocap_file_path, mocap_file_name)
print(f'mocap_lib_file={mocap_lib_file}')

MocapApi = cdll.LoadLibrary(mocap_lib_file)

MCPError = namedtuple('EMCPError', [
  'NoError',
  'MoreEvent',
  'InsufficientBuffer',
  'InvalidObject',
  'InvalidHandle',
  'InvalidParameter',
  'NotSupported',
  'IgnoreUDPSettings',
  'IgnoreTCPSettings',
  'IgnoreBvhSettings',
  'JointNotFound',
  'WithoutTransformation',
  'NoneMessage',
  'NoneParent',
  'NoneChild',
  'AddressInUse'
])._make(range(16))

EMCPRecoredFlag = namedtuple('EMCPRecoredFlag', [
  'RecoredFlag_AutoRecalculating',
  'RecoredFlag_AutoExportingBvh'
])._make([1, 2])

MCPJointTag = namedtuple('EMCPJointTag', [
  'Hips',
  'RightUpLeg',
  'RightLeg',
  'RightFoot',
  'LeftUpLeg',
  'LeftLeg',
  'LeftFoot',
  'Spine',
  'Spine1',
  'Spine2',
  'Neck',
  'Neck1',
  'Head',
  'RightShoulder',
  'RightArm',
  'RightForeArm',
  'RightHand',
  'RightHandThumb1',
  'RightHandThumb2',
  'RightHandThumb3',
  'RightInHandIndex',
  'RightHandIndex1',
  'RightHandIndex2',
  'RightHandIndex3',
  'RightInHandMiddle',
  'RightHandMiddle1',
  'RightHandMiddle2',
  'RightHandMiddle3',
  'RightInHandRing',
  'RightHandRing1',
  'RightHandRing2',
  'RightHandRing3',
  'RightInHandPinky',
  'RightHandPinky1',
  'RightHandPinky2',
  'RightHandPinky3',
  'LeftShoulder',
  'LeftArm',
  'LeftForeArm',
  'LeftHand',
  'LeftHandThumb1',
  'LeftHandThumb2',
  'LeftHandThumb3',
  'LeftInHandIndex',
  'LeftHandIndex1',
  'LeftHandIndex2',
  'LeftHandIndex3',
  'LeftInHandMiddle',
  'LeftHandMiddle1',
  'LeftHandMiddle2',
  'LeftHandMiddle3',
  'LeftInHandRing',
  'LeftHandRing1',
  'LeftHandRing2',
  'LeftHandRing3',
  'LeftInHandPinky',
  'LeftHandPinky1',
  'LeftHandPinky2',
  'LeftHandPinky3',
  'Spine3',
  'JointsCount',
])._make([x for x in range(61)])

MCPRigidBodyHandle = c_uint64

class MCPRigidBody(object):
  IMCPRigidBodyApi_Version = c_char_p(b'PROC_TABLE:IMCPRigidBody_001')

  class MCPRigidBodyApi(Structure):
    _fields_ = [
      ('GetRigidBodyRotation',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPRigidBodyHandle)),
      ('GetRigidBodyPosition',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPRigidBodyHandle)),
      ('GetRigidBodyStatus', CFUNCTYPE(c_int32, POINTER(c_int32), MCPRigidBodyHandle)),
      ('GetRigidBodyId', CFUNCTYPE(c_int32, POINTER(c_int32), MCPRigidBodyHandle))
    ]

  api = POINTER(MCPRigidBodyApi)()

  def __init__(self, rigid_body_handle):
    if not self.api:
      err = MocapApi.MCPGetGenericInterface(self.IMCPRigidBodyApi_Version, pointer(self.api))
      if err != MCPError.NoError:
        raise RuntimeError('Can not get MCPSensorModule interface: {0}'.format(MCPError._fields[err]))
    self.handle = rigid_body_handle

  def get_rotation(self):
    x = c_float()
    y = c_float()
    z = c_float()
    w = c_float()
    err = self.api.contents.GetRigidBodyRotation(pointer(x), pointer(y), pointer(z), pointer(w), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get rigid body rotation: {0}'.format(MCPError._fields[err]))
    return w.value, x.value, y.value, z.value

  def get_position(self):
    x = c_float()
    y = c_float()
    z = c_float()
    err = self.api.contents.GetRigidBodyPosition(pointer(x), pointer(y), pointer(z), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get rigid body position: {0}'.format(MCPError._fields[err]))
    return x.value, y.value, z.value

  def get_status(self):
    status = c_int32()
    err = self.api.contents.GetRigidBodyStatus(pointer(status), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get rigid body status: {0}'.format(MCPError._fields[err]))
    return status.value

  def get_id(self):
    rigid_id = c_int32()
    err = self.api.contents.GetRigidBodyId(pointer(rigid_id), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get rigid body id: {0}'.format(err))
    return rigid_id.value

MCPSensorModuleHandle = c_uint64

class MCPSensorModule(object):
  IMCPSensorModuleApi_Version = c_char_p(b'PROC_TABLE:IMCPSensorModule_001')

  class MCPSensorModuleApi(Structure):
    _fields_ = [
      ('GetSensorModulePosture',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), POINTER(c_float),
                 MCPSensorModuleHandle)),
      ('GetSensorModuleAngularVelocity',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPSensorModuleHandle)),
      ('GetSensorModuleAcceleratedVelocity',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPSensorModuleHandle)),
       ('GetSensorModuleId',
       CFUNCTYPE(c_int32, POINTER(c_int32), MCPSensorModuleHandle))
    ]

  api = POINTER(MCPSensorModuleApi)()

  def __init__(self, sensor_handle):
    if not self.api:
      err = MocapApi.MCPGetGenericInterface(self.IMCPSensorModuleApi_Version, pointer(self.api))
      if err != MCPError.NoError:
        raise RuntimeError('Can not get MCPSensorModule interface: {0}'.format(MCPError._fields[err]))
    self.handle = sensor_handle

  def get_posture(self):
    x = c_float()
    y = c_float()
    z = c_float()
    w = c_float()
    err = self.api.contents.GetSensorModulePosture(pointer(x), pointer(y), pointer(z), pointer(w), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get sensor module posture: {0}'.format(MCPError._fields[err]))
    return w.value, x.value, y.value, z.value

  def get_angular_velocity(self):
    x = c_float()
    y = c_float()
    z = c_float()
    err = self.api.contents.GetSensorModuleAngularVelocity(pointer(x), pointer(y), pointer(z), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get sensor module angular velocity: {0}'.format(MCPError._fields[err]))
    return x.value, y.value, z.value

  def get_accelerated_velocity(self):
    x = c_float()
    y = c_float()
    z = c_float()
    err = self.api.contents.GetSensorModuleAcceleratedVelocity(pointer(x), pointer(y), pointer(z), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get sensor module accelerated velocity: {0}'.format(MCPError._fields[err]))
    return x.value, y.value, z.value

  def get_sensor_module_id(self):
    sensor_id = c_int32()
    err = self.api.contents.GetSensorModuleId(pointer(sensor_id), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get sensor module id: {0}'.format(err))
    return sensor_id.value

MCPBodyPartHandle = c_uint64

class MCPBodyPart(object):
  IMCPBodyPartApi_Version = c_char_p(b'PROC_TABLE:IMCPBodyPart_001')

  class MCPBodyPartApi(Structure):
    _fields_ = [
      ('GetJointPosition', CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPBodyPartHandle)),
      ('GetJointDisplacementSpeed',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPBodyPartHandle)),
      ('GetBodyPartPosture',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPBodyPartHandle))
    ]

  api = POINTER(MCPBodyPartApi)()

  def __init__(self, body_part_handle):
    if not self.api:
      err = MocapApi.MCPGetGenericInterface(self.IMCPBodyPartApi_Version, pointer(self.api))
      if err != MCPError.NoError:
        raise RuntimeError('Can not get MCPBodyPartApi interface: {0}'.format(MCPError._fields[err]))
    self.handle = body_part_handle

  def get_position(self):
    x = c_float()
    y = c_float()
    z = c_float()
    err = self.api.contents.GetJointPosition(pointer(x), pointer(y), pointer(z), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint position:{0}'.format(MCPError._fileds[err]))
    return x.value, y.value, z.value

  def get_displacement_speed(self):
    x = c_float()
    y = c_float()
    z = c_float()
    err = self.api.contents.GetJointDisplacementSpeed(pointer(x), pointer(y), pointer(z), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint displacement speed: {0}'.format(MCPError._fields[err]))
    return x.value, y.value, z.value

  def get_posture(self):
    x = c_float()
    y = c_float()
    z = c_float()
    w = c_float()
    err = self.api.contents.GetBodyPartPosture(pointer(x), pointer(y), pointer(z), pointer(w), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get body part posture: {0}'.format(MCPError._fields[err]))
    return w.value, x.value, y.value, z.value

MCPJointHandle = c_uint64

class MCPJoint(object):
  IMCPJointApi_Version = c_char_p(b"PROC_TABLE:IMCPJoint_003")

  class MCPJointApi(Structure):
    _fields_ = [
      ('GetJointName', CFUNCTYPE(c_int32, POINTER(c_char_p), MCPJointHandle)),
      ('GetJointLocalRotation',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPJointHandle)),
      ('GetJointLocalRotationByEuler',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPJointHandle)),
      ('GetJointLocalTransformation',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPJointHandle)),
      ('GetJointDefaultLocalTransformation',
       CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPJointHandle)),
      ('GetJointChild', CFUNCTYPE(c_int32, POINTER(MCPJointHandle), POINTER(c_uint32), MCPJointHandle)),
      ('GetJointBodyPart', CFUNCTYPE(c_int32, POINTER(MCPBodyPartHandle), MCPJointHandle)),
      ('GetJointSensorModule', CFUNCTYPE(c_int32, POINTER(MCPSensorModuleHandle), MCPJointHandle)),
      ('GetJointTag', CFUNCTYPE(c_int32, POINTER(c_int32), MCPJointHandle)),
      ('GetJointNameByTag', CFUNCTYPE(c_int32, POINTER(c_char_p), c_int32)),
      ('GetJointChildJointTag', CFUNCTYPE(c_int32, POINTER(c_int32), POINTER(c_uint32), c_int32)),
      ('GetJointParentJointTag', CFUNCTYPE(c_int32, POINTER(c_int32), c_int32)),
      ('GetJointGroundingState', CFUNCTYPE(c_int32, POINTER(c_int32), MCPJointHandle)),
      ('GetJointGroundablePoints', CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_uint32), POINTER(c_uint32), MCPJointHandle)),
      ('GetJointGlobalRotation', CFUNCTYPE(c_int32, POINTER(c_float),c_int32, c_int32, POINTER(c_float), MCPJointHandle)),
      ('GetJointGlobalPosition', CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPJointHandle))
    ]

  api = POINTER(MCPJointApi)()

  def __init__(self, joint_handle):
    if not self.api:
      err = MocapApi.MCPGetGenericInterface(self.IMCPJointApi_Version, pointer(self.api))
      if err != MCPError.NoError:
        raise RuntimeError('Can not get MCPJointApi interface: {0}'.format(MCPError._fields[err]))
    self.handle = joint_handle

  def get_grounding_state(self):
    grounding_state = c_int32()
    err = self.api.contents.GetJointGroundingState(pointer(grounding_state), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint grounding state: {0}'.format(MCPError._fields[err]))
    return grounding_state.value


  def get_groundable_points(self):
    points_buffer = None
    number_of_points = c_uint32(0)
    lowest_index = c_uint32(0)
    
    err = self.api.contents.GetJointGroundablePoints(None, pointer(number_of_points), pointer(lowest_index), self.handle)
    if err != MCPError.NoError and err != MCPError.NoneMessage:
      raise RuntimeError('Can not get joint groundable points: {0}'.format(MCPError._fields[err]))
    
    points = []
    if err == MCPError.NoError and number_of_points.value > 0:
      buffer_size = number_of_points.value * 3
      points_buffer = (c_float * buffer_size)()
      
      err = self.api.contents.GetJointGroundablePoints(points_buffer, pointer(number_of_points), pointer(lowest_index), self.handle)
      if err != MCPError.NoError:
        raise RuntimeError('Can not get joint groundable points: {0}'.format(MCPError._fields[err]))
      
      for i in range(number_of_points.value):
        x = points_buffer[i * 3]
        y = points_buffer[i * 3 + 1]
        z = points_buffer[i * 3 + 2]
        points.append((x, y, z))
    
    return points, number_of_points.value, lowest_index.value

  def get_name(self):
    joint_name = c_char_p()
    err = self.api.contents.GetJointName(pointer(joint_name), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint name: {0}'.format(MCPError._fields[err]))
    return str(joint_name.value, encoding='utf8')

  def get_local_rotation(self):
    x = c_float()
    y = c_float()
    z = c_float()
    w = c_float()
    err = self.api.contents.GetJointLocalRotation(pointer(x), pointer(y), pointer(z), pointer(w), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint local rotation: {0}'.format(MCPError._fields[err]))
    return w.value, x.value, y.value, z.value

  def get_local_rotation_by_euler(self):
    x = c_float()
    y = c_float()
    z = c_float()
    err = self.api.contents.GetJointLocalRotationByEuler(pointer(x), pointer(y), pointer(z), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint euler: {0}'.format(MCPError._fields[err]))
    return x.value, y.value, z.value

  def get_local_position(self):
    x = c_float()
    y = c_float()
    z = c_float()
    err = self.api.contents.GetJointLocalTransformation(pointer(x), pointer(y), pointer(z), self.handle)
    if err != MCPError.NoError:
      return None,None,None
    return x.value, y.value, z.value

  def get_default_local_position(self):
    x = c_float()
    y = c_float()
    z = c_float()
    err = self.api.contents.GetJointDefaultLocalTransformation(pointer(x), pointer(y), pointer(z), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint default local transformation: {0}'.format(MCPError._fields[err]))
    return x.value, y.value, z.value

  def get_children(self):
    joint_count = c_uint32()
    err = self.api.contents.GetJointChild(POINTER(MCPJointHandle)(), pointer(joint_count), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint child count: {0}'.format(MCPError._fields[err]))
    joint_handles = (MCPJointHandle * joint_count.value)()
    err = self.api.contents.GetJointChild(joint_handles, pointer(joint_count), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint child: {0}'.format(MCPError._fields[err]))
    return [MCPJoint(joint_handles[i]) for i in range(joint_count.value)]

  def get_body_part(self):
    body_part_handle = MCPBodyPartHandle()
    err = self.api.contents.GetJointBodyPart(pointer(body_part_handle), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint body part: {0}'.format(MCPError._fields[err]))
    return MCPBodyPart(body_part_handle)

  def get_sensor_module(self):
    sensor_handle = MCPSensorModuleHandle()
    err = self.api.contents.GetJointSensorModule(pointer(sensor_handle), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get sensor module: {0}'.format(err))
    return MCPSensorModule(sensor_handle)

  def get_tag(self):
    tag = c_int32()
    err = self.api.contents.GetJointTag(pointer(tag), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint tag: {0}'.format(MCPError._fields[err]))
    return tag.value

  def get_name_by_tag(self, tag):
    joint_name = c_char_p()
    err = self.api.contents.GetJointNameByTag(pointer(joint_name), c_int32(tag))
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint name of joint tag {0}: {1}'.format(tag, err))
    return str(joint_name.value, encoding='utf8')

  def get_child_joint_tag(self, tag):
    joint_count = c_uint32()
    err = self.api.contents.GetJointChildJointTag(POINTER(c_int32)(), pointer(joint_count), c_int32(tag))
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint child joint tag: {0}'.format(MCPError._fields[err]))
    joints_tag = (c_int32 * joint_count.value)()
    err = self.api.contents.GetJointChildJointTag(joints_tag, pointer(joint_count), c_int32(tag))
    if err != MCPError.NoError:
      raise RuntimeError('Can not get joint child joint tag: {0}'.format(MCPError._fields[err]))
    return [joints_tag[i].value for i in range(joint_count.value)]

  def get_parent_joint_tag(self, tag):
    joint_tag = c_int32()
    err = self.api.contents.GetJointParentJointTag(pointer(joint_tag), c_int32(tag))
    if err != MCPError.NoError:
      raise RuntimeError('can not get joint parent tag: {0}'.format(MCPError._fields[err]))
    return joint_tag.value

MCPAvatarHandle = c_uint64

class MCPAvatar(object):
  IMCPAvatarApi_Version = c_char_p(b'PROC_TABLE:IMCPAvatar_003')

  class MCPAvatarApi(Structure):
    _fields_ = [
      ('GetAvatarIndex', CFUNCTYPE(c_int32, POINTER(c_uint32), MCPAvatarHandle)),
      ('GetAvatarRootJoint', CFUNCTYPE(c_int32, POINTER(MCPJointHandle), MCPAvatarHandle)),
      ('GetAvatarJoints', CFUNCTYPE(c_int32, POINTER(MCPJointHandle), POINTER(c_uint32), MCPAvatarHandle)),
      ('GetAvatarJointByName', CFUNCTYPE(c_int32, c_char_p, POINTER(MCPJointHandle), MCPAvatarHandle)),
      ('GetAvatarName', CFUNCTYPE(c_int32, POINTER(c_char_p), MCPAvatarHandle)),
      ('GetAvatarRigidBodies', CFUNCTYPE(c_int32, POINTER(MCPRigidBodyHandle), POINTER(c_uint32), MCPAvatarHandle)),
      ('GetAvatarJointHierarchy', CFUNCTYPE(c_int32, POINTER(c_char_p))),
      ('GetAvatarPostureIndex', CFUNCTYPE(c_int32, POINTER(c_uint32), MCPAvatarHandle)),
      ('GetAvatarPostureTimeCode', CFUNCTYPE(c_int32, POINTER(c_uint32), POINTER(c_uint32), POINTER(c_uint32), POINTER(c_uint32), POINTER(c_uint32), MCPAvatarHandle)),
      ('GetAvatarPostureTime',  CFUNCTYPE(c_int32, POINTER(c_uint32), POINTER(c_uint32), POINTER(c_uint32), POINTER(c_uint32), MCPAvatarHandle)),
      ('GetAvatarPosturePtpTime',  CFUNCTYPE(c_int32, POINTER(c_uint32), POINTER(c_uint32), MCPAvatarHandle)),
    ]

  api = POINTER(MCPAvatarApi)()

  def __init__(self, avatar_handle):
    if not self.api:
      err = MocapApi.MCPGetGenericInterface(self.IMCPAvatarApi_Version, pointer(self.api))
      if err != MCPError.NoError:
        raise RuntimeError('Can not get MCPAvatar interface: {0}'.format(MCPError._fields[err]))
    self.handle = avatar_handle

  def get_index(self):
    index = c_uint32(0)
    err = self.api.contents.GetAvatarIndex(pointer(index), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar index: {0}'.format(MCPError._fields[err]))
    return index.value

  def get_root_joint(self):
    joint_handle = MCPJointHandle()
    err = self.api.contents.GetAvatarRootJoint(pointer(joint_handle), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar root joint: {0}'.format(err))
    return MCPJoint(joint_handle)

  def get_joints(self):
    joint_count = c_uint32()
    err = self.api.contents.GetAvatarJoints(POINTER(MCPJointHandle)(), pointer(joint_count), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar joints: {0}'.format(MCPError._fields[err]))
    joints_handle = (MCPJointHandle * joint_count.value)()
    err = self.api.contents.GetAvatarJoints(joints_handle, pointer(joint_count), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar Joints: {0}'.format(MCPError._fields[err]))
    return [MCPJoint(joints_handle[i]) for i in range(joint_count.value)]

  def get_joint_by_name(self, name):
    joint_name = c_char_p(bytes(name, encoding='utf8'))
    joint_handle = MCPJointHandle()
    err = self.api.contents.GetAvatarJointByName(joint_name, pointer(joint_handle), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar Joints: {0}'.format(MCPError._fields[err]))
    return MCPJoint(joint_handle)

  def get_name(self):
    avatar_name = c_char_p()
    err = self.api.contents.GetAvatarName(pointer(avatar_name), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar name: {0}'.format(MCPError._fields[err]))
    return str(avatar_name.value, encoding='utf8')

  def get_rigid_bodies(self):
    rigid_body_count = c_uint32()
    err = self.api.contents.GetAvatarRigidBodies(POINTER(MCPRigidBodyHandle)(), pointer(rigid_body_count), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar rigid bodies: {0}'.format(MCPError._fields[err]))
    rigid_body_handles = (MCPRigidBodyHandle * rigid_body_count.value)()
    err = self.api.contents.GetAvatarRigidBodies(rigid_body_handles, pointer(rigid_body_count), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar rigid bodies: {0}'.format(MCPError._fields[err]))
    return [MCPRigidBody(rigid_body_handles[i]) for i in range(rigid_body_count.value)]

  def get_joint_hierarchy(self):
    hierarchy = c_char_p()
    err = self.api.contents.GetAvatarJointHierarchy(pointer(hierarchy))
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar joint hierarchy: {0}'.format(err))
    return str(hierarchy.value, encoding='utf8')

  def get_avatar_posture_index(self):
    postureIndex = c_uint32(0)
    err = self.api.contents.GetAvatarPostureIndex(byref(postureIndex), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar posture index: {0}'.format(MCPError._fields[err]))
    return postureIndex.value

  def get_avatar_posture_time(self):
    hour = c_uint32(0)  
    minute = c_uint32(0)  
    second = c_uint32(0)  
    millisecond = c_uint32(0)  
    err = self.api.contents.GetAvatarPostureTime(byref(hour), byref(minute), byref(second), byref(millisecond), self.handle)
    print(f"get_avatar_posture_time err: {err}")
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar posture time: {0}'.format(MCPError._fields[err]))
    return hour.value, minute.value, second.value, millisecond.value
  
  def get_avatar_posture_time_code(self):
    hour = c_uint32(0)  
    minute = c_uint32(0)  
    second = c_uint32(0)  
    frame = c_uint32(0)  
    rate = c_uint32(0)  
    err = self.api.contents.GetAvatarPostureTimeCode(byref(hour), byref(minute), byref(second), byref(frame), byref(rate), self.handle)
    print(f"get_avatar_posture_time_code err: {err}")
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar posture time code: {0}'.format(MCPError._fields[err]))
    return hour.value, minute.value, second.value, frame.value, rate.value
  
  def get_avatar_posture_ptp_time(self):
    second = c_uint32(0)   
    nanosecond = c_uint32(0)   
    err = self.api.contents.GetAvatarPosturePtpTime(byref(second), byref(nanosecond), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get avatar posture ptp time : {0}'.format(MCPError._fields[err]))
    return second.value, nanosecond.value


MCPCalibrateMotionProgressStep = namedtuple('EMCPCalibrateMotionProgressStep', [
  'CalibrateMotionProgressStep_Prepare',
  'CalibrateMotionProgressStep_Countdown',
  'CalibrateMotionProgressStep_Progress'
])(0, 1, 2)

MCPCalibrateMotionProgressHandle = c_uint64
class MCPCalibrateMotionProgress(object):
    IMCPCalibrateMotionProgress_Version = c_char_p(b"PROC_TABLE:IMCPCalibrateMotionProgress_001")
    class MCPCalibrateMotionProgressApi(Structure):
        _fields_ = [
            ('GetCalibrateMotionProgressCountOfSupportPoses', CFUNCTYPE(c_int32, POINTER(c_uint32), MCPCalibrateMotionProgressHandle)),
            ('GetCalibrateMotionProgressNameOfSupportPose', CFUNCTYPE(c_int32,c_char_p, POINTER(c_uint32), c_int32, MCPCalibrateMotionProgressHandle)),
            ('GetCalibrateMotionProgressStepOfPose', CFUNCTYPE(c_int32,POINTER(c_uint32),c_char_p, MCPCalibrateMotionProgressHandle)),
            ('GetCalibrateMotionProgressCountdownOfPose', CFUNCTYPE(c_int32,POINTER(c_uint32),c_char_p, MCPCalibrateMotionProgressHandle)),
            ('GetCalibrateMotionProgressProgressOfPose', CFUNCTYPE(c_int32,POINTER(c_uint32),c_char_p, MCPCalibrateMotionProgressHandle)),
            ('GetCalibrateMotionProgressStepOfCurrentPose', CFUNCTYPE(c_int32,POINTER(c_uint32),c_char_p,POINTER(c_uint32), MCPCalibrateMotionProgressHandle)),
            ('GetCalibrateMotionProgressCountdownOfCurrentPose', CFUNCTYPE(c_int32,POINTER(c_uint32),c_char_p,POINTER(c_uint32), MCPCalibrateMotionProgressHandle)),
            ('GetCalibrateMotionProgressProgressOfCurrentPose', CFUNCTYPE(c_int32,POINTER(c_uint32),c_char_p,POINTER(c_uint32), MCPCalibrateMotionProgressHandle)),
        ]
    api = POINTER(MCPCalibrateMotionProgressApi)()

    def __init__(self, progressHandle):
      if not self.api:
            err = MocapApi.MCPGetGenericInterface(self.IMCPCalibrateMotionProgress_Version, pointer(self.api))
            if err != MCPError.NoError:
                raise RuntimeError('Cannot get MCPCommandApi {0}'.format(MCPError._fields[err]))
      self.handle = progressHandle

    def get_count_of_support_poses(self):
        count = c_uint32()
        err = self.api.contents.GetCalibrateMotionProgressCountOfSupportPoses(count, self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('GetProgress Count Of Support Poses with error {0}'.format(MCPError._fields[err]))        
        return count.value
    
    def get_name_of_support_poses(self, index):
        pName = None
        pName_ = None
        pLenOfName = c_uint32(0)
        err = self.api.contents.GetCalibrateMotionProgressNameOfSupportPose(None, byref(pLenOfName), index, self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('GetProgress Name Of Support Pose with error {0}'.format(MCPError._fields[err]))        
        
        pName_ = create_string_buffer(pLenOfName.value)
        err_ = self.api.contents.GetCalibrateMotionProgressNameOfSupportPose(pName_, byref(pLenOfName), index, self.handle)
        if err_ != MCPError.NoError:
            raise RuntimeError('GetProgress Name Of Support Pose  with error {0}'.format(MCPError._fields[err_]))   
                   
        pName = pName_.value.decode('utf-8')
        return  pName

    def get_step_current_pose(self):
        current_step = c_uint32(0)
        p_len_of_name = c_uint32(0)
        p_name = None

        err = self.api.contents.GetCalibrateMotionProgressStepOfCurrentPose(byref(current_step), p_name, byref(p_len_of_name), self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('GetProgress step Of current Pose  with error {0}'.format(MCPError._fields[err])) 

        p_name = create_string_buffer(p_len_of_name.value)

        err_ = self.api.contents.GetCalibrateMotionProgressStepOfCurrentPose(byref(current_step), p_name, byref(p_len_of_name), self.handle)
        if err_ != MCPError.NoError:
            raise RuntimeError('GetProgress step Of current Pose  with error {0}'.format(MCPError._fields[err_])) 

        p_name_str = p_name.value.decode('utf-8')
        return  current_step.value, p_name_str
    
    def get_countdown_current_pose(self):
        countdown = c_uint32(0)
        p_len_of_name = c_uint32(0)
        p_name = None

        err = self.api.contents.GetCalibrateMotionProgressCountdownOfCurrentPose(byref(countdown), p_name, byref(p_len_of_name), self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('GetProgress step Of current Pose  with error {0}'.format(MCPError._fields[err])) 

        p_name = create_string_buffer(p_len_of_name.value)

        err_ = self.api.contents.GetCalibrateMotionProgressCountdownOfCurrentPose(byref(countdown), p_name, byref(p_len_of_name), self.handle)
        if err_ != MCPError.NoError:
            raise RuntimeError('GetProgress step Of current Pose  with error {0}'.format(MCPError._fields[err_])) 

        p_name_str = p_name.value.decode('utf-8')
        return  countdown.value, p_name_str
    

    def get_progress_current_pose(self):
        progress = c_uint32(0)
        p_len_of_name = c_uint32(0)
        p_name = None

        err = self.api.contents.GetCalibrateMotionProgressProgressOfCurrentPose(byref(progress), p_name, byref(p_len_of_name), self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('GetProgress step Of current Pose  with error {0}'.format(MCPError._fields[err])) 

        p_name = create_string_buffer(p_len_of_name.value)

        err_ = self.api.contents.GetCalibrateMotionProgressProgressOfCurrentPose(byref(progress), p_name, byref(p_len_of_name), self.handle)
        if err_ != MCPError.NoError:
            raise RuntimeError('GetProgress step Of current Pose  with error {0}'.format(MCPError._fields[err_])) 

        p_name_str = p_name.value.decode('utf-8')
        return  progress.value, p_name_str

MCPCommandProgress = namedtuple('MCPCommandProgress', [
  'CommandProgress_CalibrateMotion'
])(0)

MCPCommandHandle = c_uint64
class MCPCommand(object):
    IMCPCommand_Version = c_char_p(b"PROC_TABLE:IMCPCommand_002")
    class MCPCommandApi(Structure):
        _fields_ = [
            ('CreateCommand', CFUNCTYPE(c_int32, c_int32, POINTER(MCPCommandHandle))),
            ('SetCommandExtraFlags', CFUNCTYPE(c_int32, c_int32, POINTER(MCPCommandHandle))),
            ('SetCommandExtraLong', CFUNCTYPE(c_int32, c_int, c_uint64, MCPCommandHandle)),
            ('GetCommandResultMessage', CFUNCTYPE(c_int32, POINTER(c_char_p), MCPCommandHandle)),
            ('GetCommandResultCode', CFUNCTYPE(c_int32, POINTER(c_uint32), MCPCommandHandle)),
            ('GetCommandProgress', CFUNCTYPE(c_int32, c_int32,  POINTER(c_ulonglong), MCPCommandHandle)),
            ('DestroyCommand', CFUNCTYPE(c_int32, MCPCommandHandle)),
        ]
    api = POINTER(MCPCommandApi)()
    def __init__(self):
        if not self.api:
            err = MocapApi.MCPGetGenericInterface(self.IMCPCommand_Version, pointer(self.api))
            if err != MCPError.NoError:
                raise RuntimeError('Cannot get MCPCommandApi {0}'.format(MCPError._fields[err]))
        self.handle = MCPCommandHandle()
    def create_command(self, commandType):
        err = self.api.contents.CreateCommand(commandType, self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('CreateCommand failed with error {0}'.format(MCPError._fields[err])) 
          
    def set_command_extra_long(self, data=0, flag=0):
        # 保存缓冲区引用，防止垃圾回收
        self._buffer = None
        
        if isinstance(data, str):
            # Convert string to bytes
            data_bytes = data.encode('utf-8')
            # Create a buffer to hold the bytes
            data_buffer = create_string_buffer(data_bytes)
            # 保存缓冲区引用
            self._buffer = data_buffer
            # Get the pointer to the buffer
            # 使用cast将缓冲区转换为POINTER(c_char)，然后获取其地址
            data_ptr = c_uint64(addressof(data_buffer))
        else:
            # If data is not a numeric value, convert it to integer first
            data_ptr = c_uint64(int(data))
        
        err = self.api.contents.SetCommandExtraLong(c_int(flag), data_ptr, self.handle)# 0 is CommandExtraLong_Extra0
        if err != MCPError.NoError:
            raise RuntimeError('SetCommandExtraLong failed with error {0}'.format(MCPError._fields[err]))     
 

    def get_result_code(self, commandHandle):
          pCode = c_uint32()
          err = self.api.contents.GetCommandResultCode(byref(pCode), commandHandle)
          if err != MCPError.NoError:
              raise RuntimeError('Get command result code failed with error {0}'.format(MCPError._fields[err])) 
          return pCode.value
    

    def get_result_message(self, commandHandle):
          pMsg = c_char_p()
          err = self.api.contents.GetCommandResultMessage(pointer(pMsg), commandHandle)
          if err != MCPError.NoError:
              raise RuntimeError('Get command result message failed with error {0}'.format(MCPError._fields[err])) 
          return str(pMsg.value, encoding='utf8')

    def get_progress(self, commandHandle):
        pExtraLong = c_ulonglong()
        err = self.api.contents.GetCommandProgress(MCPCommandProgress.CommandProgress_CalibrateMotion, byref(pExtraLong), commandHandle)
        if err != MCPError.NoError:
            raise RuntimeError('GetProgress failed with error {0}'.format(MCPError._fields[err]))
        return pExtraLong.value
    
    def destroy_command(self, commandHandle):
      err = self.api.contents.DestroyCommand(commandHandle)
      if err != MCPError.NoError:
          raise RuntimeError('Destroy command failed with error {0}'.format(MCPError._fields[err]))
           
class MCPEventDataReserved(Structure):
  _fields_ = [
    ('reserved0', c_uint64),
    ('reserved1', c_uint64),
    ('reserved2', c_uint64),
    ('reserved3', c_uint64),
    ('reserved4', c_uint64),
    ('reserved5', c_uint64),
  ]

class MCPEventCommandRespond(Structure):
  _fields_ = [
    ('_commandHandle', MCPCommandHandle),
    ('_replay', c_int32)
  ]

MCPReplay = namedtuple('EMCPReplay', [
  'MCPReplay_Response',
  'MCPReplay_Running',
  'MCPReplay_Result'
])(0, 1, 2)



MCPPWRHandle = c_uint64
class MCPPWR(object):
    IMCPPWR_Version = c_char_p(b"PROC_TABLE:IMCPPWR_001")

    class MCPPWRApi(Structure):
        _fields_ = [
            ('GetPWRId', CFUNCTYPE(c_int32, POINTER(c_uint32), MCPPWRHandle)),
            ('GetPWRStatus', CFUNCTYPE(c_int32, POINTER(c_int32), MCPPWRHandle)),
            ('GetPWRPosition', CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPPWRHandle)),
            ('GetPWRQuaternion', CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPPWRHandle))
        ]
    api = POINTER(MCPPWRApi)()
    def __init__(self, handle):
        if not self.api:
            err = MocapApi.MCPGetGenericInterface(self.IMCPPWR_Version, pointer(self.api))
            if err != MCPError.NoError:
                raise RuntimeError('Cannot get MCPAliceHubApi: {0}'.format(MCPError._fields[err])) 
        self.handle = handle   

    def get_PWR_id(self):
        p_id = c_uint32()
        err = self.api.contents.GetPWRId(byref(p_id), self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('GetPWRId failed with error {0}'.format(MCPError._fields[err])) 
        return p_id.value
    
    def get_PWR_status(self):
        p_status = c_int32()
        err = self.api.contents.GetPWRStatus(byref(p_status), self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('GetPWRStatus failed with error {0}'.format(MCPError._fields[err])) 
        return p_status.value
    
    def get_PWR_position(self):
        p_x = c_float()
        p_y = c_float()
        p_z = c_float()
        err = self.api.contents.GetPWRPosition(byref(p_x), byref(p_y), byref(p_z), self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('GetPWRPosition failed with error {0}'.format(MCPError._fields[err]))
        return (p_x.value, p_y.value, p_z.value)
    
    def get_PWR_quaternion(self):
        p_x = c_float()
        p_y = c_float()
        p_z = c_float()
        p_w = c_float()
        err = self.api.contents.GetPWRQuaternion(byref(p_x), byref(p_y), byref(p_z), byref(p_w), self.handle)
        if err != MCPError.NoError:
             raise RuntimeError('GetPWRQuaternion failed with error {0}'.format(MCPError._fields[err]))
        return (p_x.value, p_y.value, p_z.value, p_w.value)

MCPMarkerHandle = c_uint64
MCPAliceBusHandle = c_uint64
class MCPAliceHub(object):
    IMCPAliceHub_Version = c_char_p(b"PROC_TABLE:IMCPAliceHub_001")

    class MCPAliceHubApi(Structure):
        _fields_ = [
            ('GetSensorModuleList', CFUNCTYPE(c_int32, POINTER(MCPSensorModuleHandle), POINTER(c_int32))),
            ('GetSensorModuleTimestamp', CFUNCTYPE(c_int32, POINTER(c_uint64))),
            ('GetMarkerList', CFUNCTYPE(c_int32, POINTER(MCPMarkerHandle), POINTER(c_int32))),
            ('GetMarkerTimestamp', CFUNCTYPE(c_int32, POINTER(c_uint64))),
            ('GetRigidBodyList', CFUNCTYPE(c_int32, POINTER(MCPRigidBodyHandle), POINTER(c_int32))),
            ('GetRigidBodyTimestamp', CFUNCTYPE(c_int32, POINTER(c_uint64))),
            ('GetPWRList', CFUNCTYPE(c_int32, POINTER(MCPPWRHandle), POINTER(c_int32))),
            ('GetPWRTimestamp', CFUNCTYPE(c_int32, POINTER(c_uint64)))
        ]

    api = POINTER(MCPAliceHubApi)()

    def __init__(self):
        if not self.api:
            err = MocapApi.MCPGetGenericInterface(self.IMCPAliceHub_Version, pointer(self.api))
            if err != MCPError.NoError:
                raise RuntimeError('Cannot get MCPAliceHubApi: {0}'.format(MCPError._fields[err])) 
        self.handle = MCPAliceBusHandle()   

    def get_sensor_module_list(self, count=None):
        if count is None:
            count = c_int32()
            err = self.api.contents.GetSensorModuleList(None, byref(count))
            if err != MCPError.NoError:
               raise RuntimeError('Get sensor module list failed with error {0}'.format(MCPError._fields.get(err, "Unknown Error")))
            return None, count.value
        else:
            if isinstance(count, int):
                count = c_int32(count)
            handles = (MCPSensorModuleHandle * count.value)()
            err = self.api.contents.GetSensorModuleList(handles, byref(count))
            if err != MCPError.NoError:
                raise RuntimeError('Get sensor module list failed with error {0}'.format(MCPError._fields.get(err, "Unknown Error")))
            return handles, count.value

    def get_sensor_module_timestamp(self):
        pTimestamp = c_uint64()
        err = self.api.contents.GetSensorModuleTimestamp(byref(pTimestamp))
        if err != MCPError.NoError:
            raise RuntimeError('Get sensor module timestamp failed with error {0}'.format(MCPError._fields[err]))
        return pTimestamp.value

    def get_marker_list(self, count=None):
        if count is None:
            count = c_int32()
            err = self.api.contents.GetMarkerList(None, byref(count))
            if err != MCPError.NoError:
               raise RuntimeError('Get marker list failed with error {0}'.format(MCPError._fields.get(err, "Unknown Error")))
            return None, count.value
        else:
            if isinstance(count, int):
                count = c_int32(count)
            handles = (MCPMarkerHandle * count.value)()
            err = self.api.contents.GetMarkerList(handles, byref(count))
            if err != MCPError.NoError:
                raise RuntimeError('Get marker list failed with error {0}'.format(MCPError._fields.get(err, "Unknown Error")))
            return handles, count.value

    def get_marker_timestamp(self):
        pTimestamp = c_uint64()
        err = self.api.contents.GetMarkerTimestamp(byref(pTimestamp))
        if err != MCPError.NoError:
            raise RuntimeError('Get PWR timestamp failed with error {0}'.format(MCPError._fields[err]))
        return pTimestamp.value

    def get_rigid_body_list(self, count=None):
        if count is None:
            count = c_int32()
            err = self.api.contents.GetRigidBodyList(None, byref(count))
            if err != MCPError.NoError:
               raise RuntimeError('Get rigid body list failed with error {0}'.format(MCPError._fields.get(err, "Unknown Error")))
            return None, count.value
        else:
            if isinstance(count, int):
                count = c_int32(count)
            handles = (MCPRigidBodyHandle * count.value)()
            err = self.api.contents.GetRigidBodyList(handles, byref(count))
            if err != MCPError.NoError:
                raise RuntimeError('Get rigid body list failed with error {0}'.format(MCPError._fields.get(err, "Unknown Error")))
            return handles, count.value
    
    def get_rigid_body_timestamp(self):
        pTimestamp = c_uint64()
        err = self.api.contents.GetRigidBodyTimestamp(byref(pTimestamp))
        if err != MCPError.NoError:
            raise RuntimeError('Get rigid body timestamp failed with error {0}'.format(MCPError._fields[err]))
        return pTimestamp.value

    def get_PWR_list(self, count=None):
        if count is None:
            count = c_int32()
            err = self.api.contents.GetPWRList(None, byref(count))
            if err != MCPError.NoError:
               raise RuntimeError('Get PWR list failed with error {0}'.format(MCPError._fields.get(err, "Unknown Error")))
            return None, count.value
        else:
            if isinstance(count, int):
                count = c_int32(count)
            handles = (MCPPWRHandle * count.value)()
            err = self.api.contents.GetPWRList(handles, byref(count))
            if err != MCPError.NoError:
                raise RuntimeError('Get PWR list failed with error {0}'.format(MCPError._fields.get(err, "Unknown Error")))
            return handles, count.value

    def get_PWR_timestamp(self):
        pTimestamp = c_uint64()
        err = self.api.contents.GetPWRTimestamp(byref(pTimestamp))
        if err != MCPError.NoError:
            raise RuntimeError('Get PWR timestamp failed with error {0}'.format(MCPError._fields[err]))
        return pTimestamp.value


MCPSensorModuleHandle = c_uint64
MCPSystemHandle = c_uint64
class MCPSystem(object):
    IMCPSystem_Version = c_char_p(b"PROC_TABLE:IMCPSystem_001")

    class MCPSystemApi(Structure):
        _fields_ = [
            ('GetMasterVersion', CFUNCTYPE(c_int32, POINTER(c_char_p), MCPSystemHandle)),
            ('GetMasterSerialNumber', CFUNCTYPE(c_int32, POINTER(c_char_p), MCPSystemHandle)),
            ('GetSensorModules', CFUNCTYPE(c_int32, MCPSensorModuleHandle, c_int32, POINTER(MCPSystemHandle))),
        ]

    api = POINTER(MCPSystemApi)()

    def __init__(self, system_handle):
        if not self.api:
            self.api = POINTER(self.MCPSystemApi)()
            err = MocapApi.MCPGetGenericInterface(self.IMCPSystem_Version, pointer(self.api))
            if err != MCPError.NoError:
                raise RuntimeError('Cannot get MCPSystemApi: {0}'.format(MCPError._fields[err]))
        self.handle = system_handle

    def get_master_version(self):
        pstr = c_char_p()
        err = self.api.contents.GetMasterVersion(pointer(pstr), self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('Can not get master version: {0}'.format(MCPError._fields[err]))
        return str(pstr.value, encoding='utf8')

    def get_master_serial_number(self):
        pstr = c_char_p()
        err = self.api.contents.GetMasterSerialNumber(pointer(pstr), self.handle)
        if err != MCPError.NoError:
            raise RuntimeError('Can not get master serial number: {0}'.format(MCPError._fields[err]))
        return str(pstr.value, encoding='utf8')

    def get_sensor_modules(self, sensor_module_type):
        pstr = c_char_p()
        err = self.api.contents.GetSensorModules(sensor_module_type, pointer(pstr), pointer(self.handle))
        if err != MCPError.NoError:
            raise RuntimeError('Can not get sensor modules: {0}'.format(MCPError._fields[err]))
        return str(pstr.value, encoding='utf8')

MCPEventNotify = namedtuple('EMCPNotify', [
  'Notify_RecordStarted',
  'Notify_RecordStoped',
  'Notify_RecordFinished',
  'Notify_SystemUpdated',
])(0, 1, 2, 3)

class MCPEventNotifyData(Structure):
  _fields_ = [
    ('_notify', c_int32),
    ('_notifyHandle', MCPSystemHandle)
  ]

MCPTrackerHandle = c_uint64

class MCPEventData(Union):
  _fields_ = [
    ('reserved', MCPEventDataReserved),
    ('avatar_handle', MCPAvatarHandle),
    ('error', c_int32),
    ('sensorModuleData', c_uint64),
    ('tracker_handle', MCPTrackerHandle),
    ('commandRespond', MCPEventCommandRespond),
    ('marker_handle', MCPMarkerHandle),
    ('pwrData', c_uint64),
    ('notifyData', MCPEventNotifyData)
  ]

MCPEventType = namedtuple('EMCPEventType', [
  'InvalidEvent',
  'AvatarUpdated',
  'RigidBodyUpdated',
  'Error',
  'SensorModulesUpdated',
  'TrackerUpdated',
  "CommandReply",
  "Notify",
  "AliceIMUUpdated",
  "AliceRigidbodyUpdated",
  "AliceTrackerUpdated",
  "AliceMarkerUpdated",
])(0, 256, 512, 768, 1024, 1280,1536, 1792, 4096,4097,4098,4099)

class MCPEvent(Structure):
  _fields_ = [
    ("size", c_uint32),
    ("event_type", c_int32),
    ('timestamp', c_double),
    ("event_data", MCPEventData)
  ]

MCPBvhRotation = namedtuple('EMCPBvhRotation', [
  'XYZ',
  'XZY',
  'YXZ',
  'YZX',
  'ZXY',
  'ZYX'
])(0, 1, 2, 3, 4, 5)

MCPBvhData = namedtuple('EMCPBvhData', [
  'String',
  'BinaryWithOldFrameHeader',
  'Binary',
  'BinaryLegacyHumanHierarchy'
])(0, 1, 2, 6)

MCPBvhDisplacement = namedtuple('EMCPBvhDisplacement', [
  'Disable',
  'Enable'
])(0, 1)

class MCPTracker(object):
    IMCPTracker_Version = c_char_p(b"PROC_TABLE:IMCPTracker_001")

    class MCPTrackerApi(Structure):
        _fields_ = [
            ('SendMessageData', CFUNCTYPE(c_int32, POINTER(c_char_p), c_int32, MCPTrackerHandle)),
            ('GetTrackerRotation', CFUNCTYPE(c_int32, POINTER(c_float),POINTER(c_float),POINTER(c_float),POINTER(c_float), c_char_p, MCPTrackerHandle)),
            ('GetTrackerPosition', CFUNCTYPE(c_int32, POINTER(c_float),POINTER(c_float),POINTER(c_float), c_char_p, MCPTrackerHandle)),
            ('GetTrackerPosition', CFUNCTYPE(c_int32, POINTER(c_float),POINTER(c_float),POINTER(c_float), c_char_p, MCPTrackerHandle)),
            ('GetTrackerEulerAng', CFUNCTYPE(c_int32, POINTER(c_float),POINTER(c_float),POINTER(c_float), c_char_p, MCPTrackerHandle)),
            ('GetDeviceCount', CFUNCTYPE(c_int32, POINTER(c_uint32), MCPTrackerHandle)),
            ('GetDeviceName', CFUNCTYPE(c_int32, c_int32, c_char_p, MCPTrackerHandle)),
        ]

    api = POINTER(MCPTrackerApi)()

    def __init__(self, tracker_handle):
      if not self.api:
        err = MocapApi.MCPGetGenericInterface(self.IMCPTracker_Version, pointer(self.api))
        print(self.api)
        if err != MCPError.NoError:
          raise RuntimeError('Can not get MCPTracker interface: {0}'.format(MCPError._fields[err]))
      self.handle = tracker_handle

    def get_device_name(self, serialNum):
      name = c_char_p()
      err = self.api.contents.GetDeviceName(c_int32(serialNum), name, self.handle)
      if err != MCPError.NoError:
        raise RuntimeError('Can not get device name: {0}'.format(MCPError._fields[err]))
      return name.value
    
    def get_device_count(self): 
      devCount = c_uint32(0)
      err = self.api.contents.GetDeviceCount(pointer(devCount), self.handle)
      if err != MCPError.NoError:
        raise RuntimeError('Can not get device count: {0}'.format(MCPError._fields[err]))
      return devCount.value


    def get_tracker_rotataion(self):
      x = c_float()
      y = c_float()
      z = c_float()
      w = c_float()
      deviceName = c_char_p()
      err = self.api.contents.GetTrackerRotation(pointer(x),pointer(y),pointer(z),pointer(w), deviceName,self.handle)
      if err != MCPError.NoError:
        raise RuntimeError('Can not get tracker rotation:{}'.format(MCPError._fields[err]))
      return x.value, y.value, z.value, w.value, deviceName.value
    
    def get_tracker_position(self):
      x = c_float()
      y = c_float()
      z = c_float()
      deviceName = create_string_buffer(256)  # 创建一个足够大的缓冲区
      err = self.api.contents.GetTrackerPosition(pointer(x), pointer(y), pointer(z), deviceName, self.handle)
      if err != MCPError.NoError:
        raise RuntimeError('Can not get tracker position:{}'.format(MCPError._fields[err]))
      return x.value, y.value, z.value, deviceName.value.decode('utf-8')   

    def send_message_data(self):
      message = c_char_p()
      len = c_int32()
      err = self.api.contents.SendMessageData(pointer(message), len, self.handle)
      if err != MCPError.NoError:
        raise RuntimeError('Can not Send Message Data: {0}'.format(MCPError._fields[err]))  
      return  message.value, len.value
    
    

class MCPMarker(object):
    IMCPMarker_Version = c_char_p(b"PROC_TABLE:IMCPMarker_001")

    class MCPMarkerApi(Structure):
        _fields_ = [
            ('GetMarkerPosition', CFUNCTYPE(c_int32, POINTER(c_float),POINTER(c_float),POINTER(c_float), MCPMarkerHandle)),
        ]

    api = POINTER(MCPMarkerApi)()

    def __init__(self, marker_handle):
      if not self.api:
        err = MocapApi.MCPGetGenericInterface(self.IMCPMarker_Version, pointer(self.api))
        if err != MCPError.NoError:
          raise RuntimeError('Can not get MCPMarker interface: {0}'.format(MCPError._fields[err]))
      self.handle = marker_handle

    def get_marker_position(self):
      marker_x = c_float()
      marker_y = c_float()
      marker_z = c_float()
      err = self.api.contents.GetMarkerPosition(pointer(marker_x), pointer(marker_y), pointer(marker_z), self.handle)
      if err != MCPError.NoError:
        raise RuntimeError('Can not get marker position: {0}'.format(MCPError._fields[err]))  
      return  marker_x.value, marker_y.value, marker_z.value 

MCPSettingsHandle = c_uint64

class MCPSettings(object):
  IMCPSettingsApi_Version = c_char_p(b'PROC_TABLE:IMCPSettings_001')

  class MCPSettingsApi(Structure):
    _fields_ = [
      ('CreateSettings', CFUNCTYPE(c_int32, POINTER(MCPSettingsHandle))),
      ('DestroySettings', CFUNCTYPE(c_int32, MCPSettingsHandle)),
      ('SetSettingsUDP', CFUNCTYPE(c_int32, c_uint16, MCPSettingsHandle)),
      ('SetSettingsTCP', CFUNCTYPE(c_int32, c_char_p, c_uint16, MCPSettingsHandle)),
      ('SetSettingsBvhRotation', CFUNCTYPE(c_int32, c_int32, MCPSettingsHandle)),
      ('SetSettingsBvhTransformation', CFUNCTYPE(c_int32, c_int32, MCPSettingsHandle)),
      ('SetSettingsBvhData', CFUNCTYPE(c_int32, c_int32, MCPSettingsHandle)),
      ('SetSettingsCalcData', CFUNCTYPE(c_int32, MCPSettingsHandle)),
      ("SetSettingsUDPServer", CFUNCTYPE(c_int32, c_char_p, c_uint16, MCPSettingsHandle)),
      ("SetSettingsUDPEx", CFUNCTYPE(c_int32, c_char_p, c_uint16, MCPSettingsHandle))
    ]

  api = POINTER(MCPSettingsApi)()

  def __init__(self):
    if not self.api:
      err = MocapApi.MCPGetGenericInterface(self.IMCPSettingsApi_Version, pointer(self.api))
      if err != MCPError.NoError:
        raise RuntimeError('Can not get MCPSettings interface: {0}'.format(MCPError._fields[err]))
    self.handle = MCPSettingsHandle()
    err = self.api.contents.CreateSettings(pointer(self.handle))
    if err != MCPError.NoError:
      raise RuntimeError('Can not create settings: {0}'.format(MCPError._fields[err]))

  def __del__(self):
    err = self.api.contents.DestroySettings(self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not destroy settings: {0}'.format(MCPError._fields[err]))

  def set_udp(self, local_port):
    err = self.api.contents.SetSettingsUDP(c_uint16(local_port), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set udp port of {0}: {1}'.format(local_port, MCPError._fields[err]))

  def set_tcp(self, ip, port):
    err = self.api.contents.SetSettingsTCP(c_char_p(bytes(ip, encoding='utf8')), c_uint16(port), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not settings tcp addr of {0}:{1}: {2}'.format(ip, port, MCPError._fields[err]))

  def set_bvh_rotation(self, bvh_rotation):
    err = self.api.contents.SetSettingsBvhRotation(c_int32(bvh_rotation), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set bvh rotation: {0}'.format(MCPError._fields[err]))

  def set_bvh_displacement(self, bvh_transformation):
    err = self.api.contents.SetSettingsBvhDisplacement(c_int32(bvh_transformation), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set bvh displacement: {0}'.format(MCPError._fields[err]))
  def set_bvh_transformation(self, bvh_transformation):
    err = self.api.contents.SetSettingsBvhTransformation(c_int32(bvh_transformation), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set bvh Transformation: {0}'.format(MCPError._fields[err]))

  def set_bvh_data(self, bvh_data):
    err = self.api.contents.SetSettingsBvhData(c_int32(bvh_data), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set bvh data: {0}'.format(MCPError._fields[err]))

  def set_calc_data(self):
    err = self.api.contents.SetSettingsCalcData(self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set calc data: {0}'.format(MCPError._fields[err]))
  def SetSettingsUDPServer(self, server_ip, port) -> None:
          err = self.api.contents.SetSettingsUDPServer(
              c_char_p(bytes(server_ip, encoding='utf8')),
              c_uint16(port),
              self.handle
          )
          if err != 0:
              raise RuntimeError(f"UDPServer config failed with code {err}")
  def SetSettingsUDPEx(self, local_ip, port) -> None:
        err = self.api.contents.SetSettingsUDPEx(
            c_char_p(bytes(local_ip, encoding='utf8')),
            c_uint16(port),
            self.handle
        )
        if err != 0:
            raise RuntimeError(f"UDPEx config failed with code {err}")
        
MCPUpVector = namedtuple('EMCPUpVector', [
  'XAxis',
  'YAxis',
  'ZAxis'
])(1, 2, 3)

MCPFrontVector = namedtuple('EMCPFrontVector', [
  'ParityEven',
  'ParityOdd'
])(1, 2)

MCPCoordSystem = namedtuple('EMCPCoordSystem', [
  'RightHanded',
  'LeftHanded'
])(0, 1)

MCPRotatingDirection = namedtuple('EMCPRotatingDirection', [
  'Clockwise',
  'CounterClockwise'
])(0, 1)

MCPPreDefinedRenderSettings = namedtuple('EMCPPreDefinedRenderSettings', [
  'Default',
  'UnrealEngine',
  'Unity3D',
  'Count'
])(0, 1, 2, 3)

MCPUnit = namedtuple('EMCPUnit', [
  'Centimeter',
  'Meter'
])(0, 1)

MCPRenderSettingsHandle = c_uint64


class MCPRenderSettings(object):
  IMCPRenderSettingsApi_Version = c_char_p(b'PROC_TABLE:IMCPRenderSettings_001')

  class MCPRenderSettingsApi(Structure):
    _fields_ = [
      ('CreateRenderSettings', CFUNCTYPE(c_int32, POINTER(MCPRenderSettingsHandle))),
      ('GetPreDefRenderSettings', CFUNCTYPE(c_int32, c_int32, POINTER(MCPRenderSettingsHandle))),
      ('SetUpVector', CFUNCTYPE(c_int32, c_int32, c_int32, MCPRenderSettingsHandle)),
      ('GetUpVector', CFUNCTYPE(c_int32, POINTER(c_int32), POINTER(c_int32), MCPRenderSettingsHandle)),
      ('SetFrontVector', CFUNCTYPE(c_int32, c_int32, c_int32, MCPRenderSettingsHandle)),
      ('GetFrontVector', CFUNCTYPE(c_int32, POINTER(c_int32), POINTER(c_int32), MCPRenderSettingsHandle)),
      ('SetCoordSystem', CFUNCTYPE(c_int32, c_int32, MCPRenderSettingsHandle)),
      ('GetCoordSystem', CFUNCTYPE(c_int32, POINTER(c_int32), MCPRenderSettingsHandle)),
      ('SetRotatingDirection', CFUNCTYPE(c_int32, c_int32, MCPRenderSettingsHandle)),
      ('GetRotationDirection', CFUNCTYPE(c_int32, POINTER(c_int32), MCPRenderSettingsHandle)),
      ('SetUnit', CFUNCTYPE(c_int32, c_int32, MCPRenderSettingsHandle)),
      ('GetUnit', CFUNCTYPE(c_int32, POINTER(c_int32), MCPRenderSettingsHandle)),
      ('DestroyRenderSettings', CFUNCTYPE(c_int32, MCPRenderSettingsHandle))
    ]

  api = POINTER(MCPRenderSettingsApi)()

  def __init__(self, pre_def=None):
    if not self.api:
      err = MocapApi.MCPGetGenericInterface(self.IMCPRenderSettingsApi_Version, pointer(self.api))
      if err != MCPError.NoError:
        raise RuntimeError('Can not get MCPRenderSettings interface: {0}'.format(MCPError._fields[err]))
    self.pre_def = pre_def
    self.handle = MCPRenderSettingsHandle()
    if self.pre_def == None:
      err = self.api.contents.CreateRenderSettings(pointer(self.handle))
      if err != MCPError.NoError:
        raise RuntimeError('Can not create render settings: {0}'.format(MCPError._fields[err]))
    else:
      err = self.api.contents.GetPreDefRenderSettings(c_int32(pre_def), pointer(self.handle))
      if err != MCPError.NoError:
        raise RuntimeError('Can not get render settings: {0}'.format(MCPError._fields[err]))

  def __del__(self):
    if self.pre_def == None:
      err = self.api.contents.DestroyRenderSettings(self.handle)
      if err != MCPError.NoError:
        raise RuntimeError('Can not destroy render settings: {0}'.format(MCPError._fields[err]))

  def set_up_vector(self, up_vector, sign):
    err = self.api.contents.SetUpVector(c_int32(up_vector), c_int32(sign), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set up vector: {0}'.format(MCPError._fields[err]))

  def get_up_vector(self):
    up_vector = c_int32()
    sign = c_int32()
    err = self.api.contents.GetUpVector(pointer(up_vector), pointer(sign), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get up vector: {0}'.format(MCPError._fields[err]))
    return up_vector.value, sign.value

  def set_front_vector(self, front_vector, sign):
    err = self.api.contents.SetFrontVector(c_int32(front_vector), c_int32(sign), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set front vector: {0}'.format(MCPError._fields[err]))

  def get_front_vector(self):
    front_vector = c_int32()
    sign = c_int32()
    err = self.api.contents.GetFrontVector(pointer(front_vector), pointer(sign), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get front vector: {0}'.format(MCPError._fields[err]))
    return front_vector.value, sign.value

  def set_coord_system(self, coord_sys):
    err = self.api.contents.SetCoordSystem(c_int32(coord_sys), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set coord system: {0}'.format(MCPError._fields[err]))

  def get_coord_system(self):
    coord_sys = c_int32()
    err = self.api.contents.GetCoordSystem(pointer(coord_sys), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get coord system: {0}'.format(MCPError._fields[err]))
    return coord_sys.value

  def set_rotating_direction(self, rotating_direction):
    err = self.api.contents.SetRotatingDirection(c_int32(rotating_direction), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set rotating direction: {0}'.format(MCPError._fields[err]))

  def get_rotating_direction(self):
    rotating_direction = c_int32()
    err = self.api.contents.GetRotatingDirection(pointer(rotating_direction), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get rotating direction: {0}'.format(MCPError._fields[err]))
    return rotating_direction.value

  def set_unit(self, unit):
    err = self.api.contents.SetUnit(c_int32(unit), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set unit: {0}'.format(MCPError._fields[err]))

  def get_unit(self):
    unit = c_int32()
    err = self.api.contents.GetUnit(pointer(unit), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get unit: {0}'.format(MCPError._fields[err]))
    return unit.value

MCPRobotJointType=namedtuple('EMCPRobotJointType',
    ['JointLeftShoulder_Pitch',
        'JointLeftShoulder_Roll',
        'JointLeftShoulder_Yaw',
        'JointLeftElbow',
        'JointLeftWrist_Yaw',
        'JointLeftWrist_Roll',
        'JointLeftWrist_Pitch',
        'JointRightShoulder_Pitch',
        'JointRightShoulder_Roll',
        'JointRightShoulder_Yaw',
        'JointRightElbow',
        'JointRightWrist_Yaw',
        'JointRightWrist_Roll',
        'JointRightWrist_Pitch',
        'JointLeftHip_Yaw',
        'JointLeftHip_Pitch',
        'JointLeftHip_Roll',
        'JointLeftKnee',
        'JointLeftAnkle_Pitch',
        'JointLeftAnkle_Roll',
        'JointRightHip_Yaw',
        'JointRightHip_Pitch',
        'JointRightHip_Roll',
        'JointRightKnee',
        'JointRightAnkle_Pitch',
        'JointRightAnkle_Roll',
        'JointSpine_Yaw',
        'JointSpine_Roll',
        'JointSpine_Pitch',
        'JointHead_Yaw',
        'JointHead_Roll',
        'JointHead_Pitch',
        'JointCount'])._make([x for x in range(0,33)])

MCPRobotHandle = c_uint64

class MCPRobot(object):
  IMCPRobot_Version = c_char_p(b"PROC_TABLE:IMCPRobot_001")
  class MCPRobotApi(Structure):
    _fields_=[
      ('CreateRobot', CFUNCTYPE(c_int32,c_char_p,POINTER(MCPRobotHandle))),
      ('SetRobotFPS', CFUNCTYPE(c_int32, c_int32, MCPRobotHandle)),
      ('UpdateRobot', CFUNCTYPE(c_int32, MCPAvatarHandle, MCPRobotHandle)),
      ('GetRobotRawJointAngle', CFUNCTYPE(c_int32, c_int32, POINTER(c_float), MCPRobotHandle)),
      ('GetRobotRetargetJointAngle', CFUNCTYPE(c_int32, c_char_p, POINTER(c_float), MCPRobotHandle)),
      ('GetRobotRosFrameJson', CFUNCTYPE(c_int32, POINTER(c_char_p), c_bool, MCPRobotHandle)),
      ('GetRobotRootRotation', CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPRobotHandle)),
      ('GetRobotRootPosition', CFUNCTYPE(c_int32, POINTER(c_float), POINTER(c_float), POINTER(c_float), MCPRobotHandle)),
      ('DestroyRobot', CFUNCTYPE(c_int32, MCPRobotHandle)),
      ('RunRobotStep', CFUNCTYPE(c_int32, MCPRobotHandle))
       ]
  api = POINTER(MCPRobotApi)()
  def __init__(self, json_robot_desc):
    if not self.api:
      err = MocapApi.MCPGetGenericInterface(self.IMCPRobot_Version, pointer(self.api))
      if err != MCPError.NoError:
        raise  RuntimeError('Can not get MCPRobot interface: {}'.format(MCPError._fields[err]))
    self.handle = MCPSettingsHandle()
    err = self.api.contents.CreateRobot(c_char_p(bytes(json_robot_desc, encoding='utf8')), pointer(self.handle))
    if err != MCPError.NoError:
      raise  RuntimeError('Can not create robot: {0}'.format(MCPError._fields[err]))
  def __del__(self):
    err = self.api.contents.DestroyRobot(self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not destroy robot : {0}'.format(MCPError._fields[err]))

  def set_robot_fps(self,fps):
    err = self.api.contents.SetRobotFPS(c_uint32(fps),self.handle)
    if self != MCPError.NoError:
      raise RuntimeError('Can not set robot fps: {0}'.format(MCPError._fields[err]))
  def update_robot(self,avatar):
    err = self.api.contents.UpdateRobot(avatar.handle, self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not update robot: {0}'.format(MCPError._fields[err]))
  def get_robot_raw_joint_angle(self,robotJointType):
    val = c_float()
    err = self.api.contents.GetRobotRawJointAngle(c_int32(robotJointType),pointer(val), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get robot raw joint angle: {0}'.format(MCPError._fields[err]))
    return val.value
  def get_robot_retarget_joint_angle(self,jointName):
    val = c_float()
    err = self.api.contents.GetRobotRetargetJointAngle(c_char_p(bytes(jointName,encoding='utf8')), pointer(val),self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get robot retarget joint angle: {0}'.format(MCPError._fields[err]))
    return val.value
  def get_robot_ros_frame_json(self):
    val = c_float()
    json_str = c_char_p()
    err = self.api.contents.GetRobotRosFrameJson(pointer(json_str), pointer(val), self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get robot ros frame json: {0}'.format(MCPError._fields[err]))
    return str(json_str.value, encoding='utf8'),val.value
  def get_robot_root_rotataion(self):
    x = c_float()
    y = c_float()
    z = c_float()
    w = c_float()
    err = self.api.contents.GetRobotRootRotation(pointer(x),pointer(y),pointer(z),pointer(w),self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get robot robot rotation:{}'.format(MCPError._fields[err]))
    return x.value, y.value, z.value, w.value
  def get_robot_root_position(self):
    x = c_float()
    y = c_float()
    z = c_float()
    err = self.api.contents.GetRobotRootPosition(pointer(x), pointer(y), pointer(z),self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get robot root position:{}'.format(MCPError._fields[err]))
    return x.value, y.value, z.value
  def run_robot_step(self):
    err = self.api.contents.RunRobotStep(self.handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not run robot step:{}'.format(MCPError._fields[err]))

EMCPCommand = namedtuple('EMCPCommand', [
  'CommandStartCapture',
  'CommandStopCapture',
  'CommandZeroPosition',
  'CommandCalibrateMotion',
  'CommandStartRecored',
  'CommandStopRecored',
  'CommandResumeOriginalPosture',
  'CommandClearZeroMotionDrift',
  "CommandResumeOriginalHandsPosture"
])(0, 1, 2, 3,4,5,6,7,8)

MCPApplicationHandle = c_uint64
MCPTrackerHandle = c_uint64        
MCPEventHandleProc = c_uint64        
class MCPApplication(object):
  IMCPApplicationApi_Version = c_char_p(b'PROC_TABLE:IMCPApplication_002')

  class MCPApplicationApi(Structure):
    _fields_ = [
      ('CreateApplication', CFUNCTYPE(c_int32, POINTER(MCPApplicationHandle))),
      ('DestroyApplication', CFUNCTYPE(c_int32, MCPApplicationHandle)),
      ('SetApplicationSettings', CFUNCTYPE(c_int32, MCPSettingsHandle, MCPApplicationHandle)),
      ('SetApplicationRenderSettings', CFUNCTYPE(c_int32, MCPRenderSettingsHandle, MCPApplicationHandle)),
      ('OpenApplication', CFUNCTYPE(c_int32, MCPApplicationHandle)),
      ('EnableApplicationCacheEvents', CFUNCTYPE(c_int32, MCPApplicationHandle)),
      ('DisableApplicationCacheEvents', CFUNCTYPE(c_int32, MCPApplicationHandle)),
      ('ApplicationCacheEventsIsEnabled', CFUNCTYPE(c_int32, POINTER(c_bool), MCPApplicationHandle)),
      ('CloseApplication', CFUNCTYPE(c_int32, MCPApplicationHandle)),
      ('GetApplicationRigidBodies', CFUNCTYPE(c_int32, POINTER(c_uint64), POINTER(c_uint32), MCPApplicationHandle)),
      ('GetApplicationAvatars', CFUNCTYPE(c_int32, POINTER(c_uint64), POINTER(c_uint32), MCPApplicationHandle)),
      ('PollApplicationNextEvent', CFUNCTYPE(c_int32, POINTER(MCPEvent), POINTER(c_uint32), MCPApplicationHandle)),
      ('GetApplicationSensorModules', CFUNCTYPE(c_int32, MCPSensorModuleHandle, c_int32, MCPApplicationHandle)),
      ('GetApplicationTrackers', CFUNCTYPE(c_int32, MCPTrackerHandle, POINTER(c_uint32), MCPApplicationHandle)),
      ('QueuedServerCommand', CFUNCTYPE(c_int32, MCPCommandHandle, MCPApplicationHandle)),
      ('RegisterEventHandler', CFUNCTYPE(c_int32, MCPEventHandleProc,  POINTER(c_uint64), MCPApplicationHandle)),
      ('UnregisterEventHandler', CFUNCTYPE(c_int32, MCPEventHandleProc,  POINTER(c_uint64), MCPApplicationHandle)),
      ('GetAppplicationSystem', CFUNCTYPE(c_int32, MCPSystemHandle, MCPApplicationHandle)),
    ]

  api = POINTER(MCPApplicationApi)()

  def __init__(self):
    if not self.api:
      err = MocapApi.MCPGetGenericInterface(self.IMCPApplicationApi_Version, pointer(self.api))
      if err != MCPError.NoError:
        raise RuntimeError('Can not get IMCPApplication interface: {0}'.format(MCPError._fields[err]))
    self._handle = MCPApplicationHandle()
    err = self.api.contents.CreateApplication(pointer(self._handle))
    if err != MCPError.NoError:
      raise RuntimeError('Can not create application: {0}'.format(MCPError._fields[err]))
    self._is_opened = False

  def queue_command(self, cmd_type):
    cmd = MCPCommand()
    cmd.create_command(cmd_type)
    err = self.api.contents.QueuedServerCommand(cmd.handle, self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not queued server command: {0}'.format(MCPError._fields[err]))

  def queue_command_with_extra_long(self, cmd_type, extra_long,flag=0):
    print("Queue record command with export folder: {}".format(extra_long))
    cmd = MCPCommand()
    cmd.create_command(cmd_type)  # 先创建命令初始化句柄
    # 根据C++实现，使用CommandExtraLong_Extra0（值为0）作为commandType
    cmd.set_command_extra_long(extra_long)  # 0 is CommandExtraLong_Extra0
    err = self.api.contents.QueuedServerCommand(cmd.handle, self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not queued server command: {0}'.format(MCPError._fields[err]))    

  def command_stop_recored_with_extra_long(self, export_folder, auto_recalculate=False):
    print("Stop record command with export folder: {}".format(export_folder))
    cmd = MCPCommand()
    cmd.create_command(EMCPCommand.CommandStopRecored)  # 先创建命令初始化句柄
    
    # 根据C++实现，设置标志
    flags = EMCPRecoredFlag.RecoredFlag_AutoRecalculating|EMCPRecoredFlag.RecoredFlag_AutoExportingBvh
    # if auto_recalculate:
    #   flags |= EMCPRecoredFlag.RecoredFlag_AutoRecalculating
    # if export_folder:
    #   flags |= EMCPRecoredFlag.RecoredFlag_AutoExportingBvh
    
    # 如果flags不为0，设置CommandExtraLong_Extra0
    print("flags: {}".format(flags))
    if flags != 0:
      cmd.set_command_extra_long(flags, 0)  # CommandExtraLong_Extra0: 设置为标志值
    
    # 如果设置了导出路径，设置CommandExtraLong_Extra1
    if export_folder:
      cmd.set_command_extra_long(export_folder, 1)  # CommandExtraLong_Extra1: 设置为导出文件夹路径
    
    err = self.api.contents.QueuedServerCommand(cmd.handle, self._handle)
    
    if err != MCPError.NoError:
      raise RuntimeError('Can not queued server command: {0}'.format(MCPError._fields[err]))    

  def __del__(self):
    err = self.api.contents.DestroyApplication(self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not destroy application: {0}'.format(MCPError._fields[err]))

  def set_settings(self, settings):
    err = self.api.contents.SetApplicationSettings(settings.handle, self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set application settings: {0}'.format(MCPError._fields[err]))

  def set_render_settings(self, settings):
    err = self.api.contents.SetApplicationRenderSettings(settings.handle, self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not set application render settings: {0}'.format(MCPError._fields[err]))

  def open(self):
    err = self.api.contents.OpenApplication(self._handle)
    self._is_opened = (err == MCPError.NoError)
    return err == MCPError.NoError, MCPError._fields[err]

  def is_opened(self):
    return self._is_opened

  def enable_event_cache(self):
    err = self.api.contents.EnableApplicationCacheEvents(self._handle)
    return err == MCPError.NoError, MCPError._fields[err]

  def disable_event_cache(self):
    err = self.api.contents.DisableApplicationCacheEvents(self._handle)
    return err == MCPError.NoError, MCPError._fields[err]

  def is_event_cache_enabled(self):
    enable = c_bool()
    err = self.api.contents.ApplicationCacheEventsIsEnabled(pointer(enable), self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get application event cache settings: {0}'.format(MCPError._fields[err]))
    return enable.value

  def close(self):
    err = self.api.contents.CloseApplication(self._handle)
    self._is_opened = False
    return err == MCPError.NoError, MCPError._fields[err]

  def get_rigid_bodies(self):
    rigid_body_size = c_uint32()
    err = self.api.contents.GetApplicationRigidBodies(POINTER(MCPRigidBodyHandle)(), pointer(rigid_body_size),
                                                      self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get application rigid bodies: {0}'.format(MCPError._fields[err]))
    rigid_body_handles = (MCPRigidBodyHandle * rigid_body_size.value)()
    err = self.api.GetApplicationRigidBodies(rigid_body_handles, pointer(rigid_body_size), self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get application rigid bodies: {0}'.format(MCPError._fields[err]))
    return [MCPRigidBody(rigid_body_handles[i]) for i in range(rigid_body_size.value)]

  def get_avatars(self):
    avatar_count = c_uint32()
    err = self.api.contents.GetApplicationAvatars(POINTER(MCPAvatarHandle)(), pointer(avatar_count), self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get application avatars: {0}'.format(MCPError._fields[err]))
    avatar_handles = (MCPAvatarHandle * avatar_count.value)()
    err = self.api.contents.GetApplicationAvatars(avatar_handles, pointer(avatar_count), self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get application avatars: {0}'.format(MCPError._fields[err]))
    return [MCPAvatar(avatar_handles[i]) for i in range(avatar_count.value)]
  def get_trackers(self):
    tracker_count = c_uint32()
    err = self.api.contents.GetApplicationTrackers(POINTER(MCPTrackerHandle)(), pointer(tracker_count), self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get application trackers: {0}'.format(MCPError._fields[err]))
    tracker_handles = (MCPTrackerHandle * tracker_count.value)()
    err = self.api.contents.GetApplicationTrackers(tracker_handles, pointer(tracker_count), self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get application trackers: {0}'.format(MCPError._fields[err]))
    return [MCPTracker(tracker_handles[i]) for i in range(tracker_count.value)]

  def poll_next_event(self):
    evt_count = c_uint32(0)
    err = self.api.contents.PollApplicationNextEvent(POINTER(MCPEvent)(), pointer(evt_count), self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not poll application next event: {0}'.format(MCPError._fields[err]))
    if evt_count.value == 0:
      return []
    evt_array = (MCPEvent * evt_count.value)()
    for i in range(evt_count.value):
      evt_array[i].size = sizeof(MCPEvent)
    err = self.api.contents.PollApplicationNextEvent(evt_array, pointer(evt_count), self._handle)
    if err != MCPError.NoError:
      return []
    if evt_count.value == 0:
      return []
    return [evt_array[i] for i in range(evt_count.value)]
  
  def get_system(self):
    systemHandle =  MCPSystemHandle()
    err = self.api.contents.GetAppplicationSystem(pointer(systemHandle), self._handle)
    if err != MCPError.NoError:
      raise RuntimeError('Can not get application system: {0}'.format(MCPError._fields[err]))
    return systemHandle

def test_mocap_api():
  def print_joint(joint):
    print(joint.get_name())
    print(joint.get_local_rotation())
    print(joint.get_local_rotation_by_euler())
    print(joint.get_local_position())

    children = joint.get_children()
    for child in children:
      print_joint(child)

  app = MCPApplication()
  settings = MCPSettings()
  settings.set_udp(7012)
  app.set_settings(settings)
  app.open()
  # app.queue_command1(1)
  
  
  while True:
    evts = app.poll_next_event()
    for evt in evts:
      if evt.event_type == MCPEventType.AvatarUpdated:
        avatar = MCPAvatar(evt.event_data.avatar_handle)
        print(f"get_avatar_posture_ptp_time():{avatar.get_avatar_posture_time()}")
      elif evt.event_type == MCPEventType.RigidBodyUpdated:
        print('rigid body updated')
      elif evt.event_type == MCPEventType.AliceTrackerUpdated:
         _handle_tracker_data()    
      elif evt.event_type == MCPEventType.AliceMarkerUpdated:
        _handle_marker_data()
      else:
        print('unknow event')
    time.sleep(0.001)

def  _handle_marker_data():
    alicehub = MCPAliceHub()
    recv, count = alicehub.get_marker_list()
    if count > 0:
        recv, count1 = alicehub.get_marker_list(count)
        for i in range(count):
          marker_handle = recv[i]
          marker = MCPMarker(marker_handle)
          position = marker.get_marker_position()
          print('position:', position)


def _handle_tracker_data():
    alicehub = MCPAliceHub()
    recv, count = alicehub.get_PWR_list()
    if count > 0:
        recv, count1 = alicehub.get_PWR_list(count)
        timestamp = alicehub.get_PWR_timestamp()
        for i in range(count):
            PWRHandle = recv[i]
            MCPPWRH = MCPPWR(PWRHandle)
            id = MCPPWRH.get_PWR_id()
            status = MCPPWRH.get_PWR_status()
            position = MCPPWRH.get_PWR_position()
            quaternion = MCPPWRH.get_PWR_quaternion()
            print('timestamp',timestamp,'id:', id, 'status:', status, 'position:', position, 'quaternion:', quaternion)
if __name__ == '__main__':
  test_mocap_api()