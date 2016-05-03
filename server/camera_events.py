# Events and associated callbacks for server-side sockets

from flask import session
from flask.ext.socketio import emit, join_room, leave_room
from server_state import socketio
from mission_state import frame_counter

import base64

from PIL import Image
import sys
from struct import *
import array

import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np

CAMERA_NAMESPACE = "/camera"

@socketio.on('connect', namespace=CAMERA_NAMESPACE)
def on_connect():
    print "[socket][camera][connect]: Connection received"

@socketio.on("wide_angle_upload", namespace=CAMERA_NAMESPACE)
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
    emit("wide_angle_upload", namespace=CAMERA_NAMESPACE)

@socketio.on("frame") #, namespace=CAMERA_NAMESPACE)
def frame_upload(data):
    print "[camera][frame]: frame received"
    print "[camera][frame]: saving image"
    byteArray = bytearray(data['image'])    
    global frame_counter
    with open('frame_' + str(frame_counter) + '.rgba', 'w') as file_:
        file_.write(byteArray)
        frame_counter = frame_counter + 1
    print "[camera][frame]: image processing complete"

def view_yuv(data):
    width = 1280
    height = 720
 
    y = array.array('B')
    u = array.array('B')
    v = array.array('B')
 
    image_out = Image.new("RGB", (width, height))
    pix = image_out.load() 
 
    k = 0
    for i in range(0, height/2):
        for j in range(0, width/2):
            # u.append(ord(f_uv.read(1)));
            u.append(ord(data[width*height + k]))
            ++k
 
    for i in range(0, height/2):
        for j in range(0, width/2):
            # v.append(ord(f_uv.read(1)));
            v.append(ord(data[width*height + k]))
            ++k

    l = 0

    for i in range(0,height):
        for j in range(0, width):
            # y.append(ord(f_y.read(1)));                        
            y.append(ord(data[l]))
            ++l
            Y_val = y[(i*width)+j]
            U_val = u[((i/2)*(width/2))+(j/2)]
            V_val = v[((i/2)*(width/2))+(j/2)]
            B = 1.164 * (Y_val-16) + 2.018 * (U_val - 128)
            G = 1.164 * (Y_val-16) - 0.813 * (V_val - 128) - 0.391 * (U_val - 128)
            R = 1.164 * (Y_val-16) + 1.596*(V_val - 128)
            pix[j, i] = int(R), int(G), int(B)
    
    image_out.show()
