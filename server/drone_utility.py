from socketIO_client import SocketIO, BaseNamespace
from server_state import HOST

# import logging
# logging.getLogger('requests').setLevel(logging.WARNING)
# logging.basicConfig(level=logging.DEBUG)

pose = None
path = None

class PoseNamespace(BaseNamespace):
    _connected = True

def on_pose_current_ack_response(*args):
    global pose
    pose = args

def on_path_current_ack_response(*args):
    global path
    path = args
        
socketIO = SocketIO(HOST, 5000, PoseNamespace)
pose_namespace = socketIO.define(PoseNamespace, '/pose')

def get_tango_location():
    pose_namespace.on('pose_current_ack', on_pose_current_ack_response)
    socketIO.wait(seconds=0.1)
    return pose

def get_path():
    pose_namespace.on('path_current_ack', on_path_current_ack_response)
    socketIO.wait(seconds=0.1)
    return path
