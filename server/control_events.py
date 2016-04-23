# Events and associated callbacks for server-side sockets

from flask import session
from flask.ext.socketio import emit, join_room, leave_room
from server_state import socketio

import mission_state

from dronekit import connect, VehicleMode
import time
import sys
from pymavlink import mavutil

import math

# TODO: namespace
# CONTROL_NAMESPACE = "/control"

@socketio.on('connect') # , namespace=CONTROL_NAMESPACE)
def on_connect():
    print "[socket][control][connect]: Connection received"
    

@socketio.on('altitude_cmd') # , namespace=CONTROL_NAMESPACE)
def altitudeChange(json):
    print "[socket][control][altitude]: " + str(json)

@socketio.on('rotation_cmd') # , namespace=CONTROL_NAMESPACE)
def rotationChange(json):
    print "[socket][control][rotation]: " + str(json)
    heading = float(json['heading'])
    condition_yaw(heading)

@socketio.on('lateral_cmd') #, namespace=CONTROL_NAMESPACE)
def lateralChange(json):
    print "[socket][control][lateral]: " + str(json)
    x_offset = json['x_offset']
    y_offset = json['y_offset']

    if x_offset == 0 and y_offset == 0:
        return

    magnitude = math.sqrt(x_offset ** 2 + y_offset ** 2)

    x_norm = x_offset / magnitude
    y_norm = y_offset / magnitude

    speed = 5

    x_vel = speed * x_norm
    y_vel = speed * y_norm

    duration = json['duration']
    send_ned_velocity(x_vel, y_vel, 0, duration)


def condition_yaw(heading, relative=False):
    vehicle = mission_state.vehicle
    if relative:
        is_relative=1 #yaw relative to direction of travel
    else:
        is_relative=0 #yaw is an absolute angle
    # create the CONDITION_YAW command using command_long_encode()
    msg = vehicle.message_factory.command_long_encode(
        0, 0,    # target system, target component
        mavutil.mavlink.MAV_CMD_CONDITION_YAW, #command
        0, #confirmation
        heading,    # param 1, yaw in degrees
        0,          # param 2, yaw speed deg/s
        1,          # param 3, direction -1 ccw, 1 cw
        is_relative, # param 4, relative offset 1, absolute angle 0
        0, 0, 0)    # param 5 ~ 7 not used
    # send command to vehicle
    vehicle.send_mavlink(msg)

def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
    """
    Move vehicle in direction based on specified velocity vectors.
    """
    vehicle = mission_state.vehicle
    msg = vehicle.message_factory.set_position_target_local_ned_encode(
        0,       # time_boot_ms (not used)
        0, 0,    # target system, target component
        mavutil.mavlink.MAV_FRAME_LOCAL_NED, # frame
        0b0000111111000111, # type_mask (only speeds enabled)
        0, 0, 0, # x, y, z positions (not used)
        velocity_x, velocity_y, velocity_z, # x, y, z velocity in m/s
        0, 0, 0, # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
        0, 0)    # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)


    # send command to vehicle on 1 Hz cycle
    for x in range(0,duration):
        vehicle.send_mavlink(msg)
        time.sleep(1)
