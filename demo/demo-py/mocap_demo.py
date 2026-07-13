from MocapApi.mocap_api import *

def get_event_type_name(event_type_value):
    """
    Convert event type value to corresponding enum name
    
    Args:
        event_type_value: The numeric event type value
        
    Returns:
        str: The corresponding event type name
    """
    event_type_map = {
        MCPEventType.InvalidEvent: 'InvalidEvent',
        MCPEventType.AvatarUpdated: 'AvatarUpdated',
        MCPEventType.TrackerUpdated: 'TrackerUpdated',
        MCPEventType.AliceIMUUpdated: 'AliceIMUUpdated',
        MCPEventType.AliceRigidbodyUpdated: 'AliceRigidbodyUpdated',  
        MCPEventType.AliceTrackerUpdated: 'AliceTrackerUpdated',
        MCPEventType.AliceMarkerUpdated: 'AliceMarkerUpdated',
    }
    return event_type_map.get(event_type_value, f'Unknown({event_type_value})')


class MocapAxisDemo:
    """
    Mocap Axis Studio Demo class for demonstrating how to get Axis Studio data through Mocap API
    """
    
    def __init__(self):
        """
        Initialize Mocap Axis Studio Demo instance
        """
        self.app = None
        self.running = False
        self.prev_posture_time_ms = None
    
    def start(self, udp_port=7012):
        """
        Start Mocap application and handle event loop
        
        Args:
            udp_port: UDP port number, default is 7012
        """
        # Initialize Mocap application
        self.app = MCPApplication()
        settings = MCPSettings()
        settings.set_udp(udp_port)
        settings.set_bvh_rotation(MCPBvhRotation.XYZ)
        self.app.set_settings(settings)
        self.app.open()
        print(f"Mocap application initialized, UDP port: {udp_port}")
        
        self.running = True
        try:
            while self.running:
                evts = self.app.poll_next_event()
                for evt in evts:
                    if evt.event_type == MCPEventType.AvatarUpdated: # avatar bvh(人体BVH)
                        self._handle_avatar_data(evt)
                    else:
                        print('Other events:', get_event_type_name(evt.event_type))
        except KeyboardInterrupt:
            print("Program interrupted by user")
        finally:
            self.stop()

   

    def _handle_avatar_data(self, evt):
        """
        Handle avatar data
        """
        avatar = MCPAvatar(evt.event_data.avatar_handle)
        joints = avatar.get_joints()  # Get all joint data
        hour, minute, second, millisecond = avatar.get_avatar_posture_time()
        current_time_ms = ((hour * 3600 + minute * 60 + second) * 1000 + millisecond)
        if self.prev_posture_time_ms is not None:
            delta_ms = current_time_ms - self.prev_posture_time_ms
            print(f"=====Frame interval: {delta_ms} ms")
        self.prev_posture_time_ms = current_time_ms
        for joint in joints:
            link_name = joint.get_name()  # Get joint name
            
            
            position = joint.get_local_position()  # Get joint position
            rotation = joint.get_local_rotation()  # Get joint rotation
            
            # print(f"avatar data : joint: {link_name}, position: {position}, rotation: {rotation}")

            grounding_state = joint.get_grounding_state()  # Get joint grounding state
            if grounding_state == 0:
                print(f"avatar data : joint: {link_name}, grounding_state: {grounding_state}")
                groundable_points = joint.get_groundable_points()  # Get joint groundable points
                print(f"avatar data : joint: {link_name}, groundable_points: {groundable_points}")



    def stop(self):
        """
        Close Mocap application
        """
        self.running = False
        if self.app:
            self.app.close()
            print("Mocap application closed")

if __name__ == "__main__":
    # Create and run demo instance
    demo = MocapAxisDemo()
    print("Starting Mocap Axis Studio demo...") 
    demo.start()