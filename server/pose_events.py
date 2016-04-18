# Events and associated callbacks for server-side sockets

from flask import session
from flask.ext.socketio import emit, join_room, leave_room
from server_state import socketio

from mission_state import path, pose

POSE_NAMESPACE = "/pose"

@socketio.on('connect', namespace=POSE_NAMESPACE)
def on_connect():
    print "[socket][pose][connect]: Connection received"

@socketio.on("pose_update", namespace=POSE_NAMESPACE)
def pose_update(json):
    # JSON arg should have Tango position object
    global pose
    pose = json
    # print "[socket][pose_update]: " + str(json)
    emit("pose_update_ack", namespace=POSE_NAMESPACE)    
    print "[socket][pose_current_ack]: " + str(pose)
    emit('pose_current_ack', pose, namespace=POSE_NAMESPACE, broadcast=True)

@socketio.on("path_config", namespace=POSE_NAMESPACE)
def path_config(json):
    global path
    path = json
    print "[socket][path_config]: " + str(json)
    emit("path_config_ack", namespace=POSE_NAMESPACE)
    emit("path_current_ack", path, namespace=POSE_NAMESPACE, broadcast=True)

@socketio.on("pose_current", namespace=POSE_NAMESPACE)
def pose_current(json):
    print "[socket][pose_current]: " + str(pose)
    emit("pose_current_ack", pose, namespace=POSE_NAMESPACE)
