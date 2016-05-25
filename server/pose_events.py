# Events and associated callbacks for server-side sockets

from flask import session
from flask.ext.socketio import emit, join_room, leave_room
from server_state import socketio

import mission_state
from mission_state import path, pose 
from pymavlink import mavutil
import json

import navigation

POSE_NAMESPACE = "/pose"
counter = 0

@socketio.on('connect', namespace=POSE_NAMESPACE)
def on_connect():
		print "[socket][pose][connect]: Connection received"

@socketio.on("pose_update", namespace=POSE_NAMESPACE)
def pose_update(tango_json):
		# JSON arg should have Tango position object
		global pose
		global counter
		pose = tango_json
		
		(lat, lng, alt) = navigation.getLLAFromNED(tango_json)
		xvel = tango_json["x_vel"]
		yvel = tango_json["y_vel"]
		zvel = tango_json["z_vel"]
		timestamp = tango_json["timestamp"]
		accuracy = tango_json["accuracy"]  # Horizontal accuracy
	
		counter += 1
		if counter % 10 == 0:	
			print "Tango location: [socket][pose][pose_update]: " + str(lat) + ", " + str(lng) + ", " + str(alt)
			print "Tango velocity: [socket][pose][pose_update]: " + str(xvel) + ", " + str(yvel) + ", " + str(zvel)
			#print "Tango timestamp: " + str(timestamp)	
			#print "Drone location: " + str(mission_state.vehicle.location.global_relative_frame)
			#print "Drone velocity: " + str(mission_state.vehicle.velocity)	
	
			mission_state.vehicle._master.mav.command_long_send(
					0, 0,	# target system, target component 
					mavutil.mavlink.MAV_CMD_NAV_SEND_TANGO_GPS,	# command 
					0,	# confirmation
					float(lat), float(lng), float(alt), float(xvel), float(yvel), float(zvel), float(timestamp))		# params 1-7
	
			#print "GPS location: " + str(mission_state.vehicle.location.global_relative_frame)
			#error = abs(mission_state.vehicle.location.global_relative_frame.lat - lat) + abs(mission_state.vehicle.location.global_relative_frame.lon - lng)	
			#print "error: ", error

		# To be sent to the iOS code: only need the calculated lat and lon from the Tango
		ios_info = {"lat": lat, "lon": lng}
		json_info = json.dumps(ios_info)
		print "[socket][pose][pose_update]: ", str(json_info)
		socketio.emit("gps_pos_ack", json_info, broadcast=True)

		emit("pose_update_ack", namespace=POSE_NAMESPACE)		 
		emit('pose_current_ack', pose, namespace=POSE_NAMESPACE, broadcast=True)

@socketio.on("path_config", namespace=POSE_NAMESPACE)
def path_config(tango_json):
		global path
		path = tango_json
		print "[socket][path_config]: " + str(tango_json)
		emit("path_config_ack", namespace=POSE_NAMESPACE)
		emit("path_current_ack", path, namespace=POSE_NAMESPACE, broadcast=True)

@socketio.on("pose_current", namespace=POSE_NAMESPACE)
def pose_current(tango_json):
		# print "[socket][pose_current]: " + str(pose)
		emit("pose_current_ack", pose, namespace=POSE_NAMESPACE)

@socketio.on("path_current", namespace=POSE_NAMESPACE)
def path_current(tango_json):
		print "[socket][path_current]: " + str(path)
		emit("path_current_ack", path, namespace=POSE_NAMESPACE)

@socketio.on("path_for_interpolation",namespace=POSE_NAMESPACE)
def path_for_interpolation(tango_json):
		print "[socket][path_for_interpolation]: " + str(path)
		#emit("path_for_interpolation_ack", path, namespace=POSE_NAMESPACE)
