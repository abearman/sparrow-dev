# Events and associated callbacks for server-side sockets

from flask import session
from flask.ext.socketio import emit, join_room, leave_room
from .. import socketio

@socketio.on("pose_update", namespace="/pose")
def pose_update(json):
    # JSON arg should have timestamp and pose string
    print "[socket][pose_update]: " + str(json)
    emit("pose_update_ack", json, namespace="/pose")
