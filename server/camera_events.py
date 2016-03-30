# Events and associated callbacks for server-side sockets

from flask import session
from flask.ext.socketio import emit, join_room, leave_room
from .. import socketio

import base64

@socketio.on("wide_angle_upload", namespace="/camera")
def wide_angle_upload(json):
    # JSON arg should have a timestamp and base64 encoded string
    # for image data
    timestamp = json["timestamp"]
    print "[camera][wide_angle_upload]: " + timestamp
    encoded_image = json["image64"]
    decoded_image = base64.b64decode(encoded_image)
    # Save image
    # TODO: update upload path for saving
    filename = timestamp + "png"
    with open(filename, 'wb') as f:
        f.write(decoded_image)
    emit("wide_angle_upload", namespace="/pose")
