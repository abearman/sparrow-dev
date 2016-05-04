import auth

import mission_state

from flask import Blueprint, Flask, request
from flask.ext.socketio import emit, join_room, leave_room
import json
import thread
from threading import Thread

from dronekit import connect, VehicleMode
import time
import sys
from pymavlink import mavutil


def drone_init():
		print "Start simulator (SITL)"
		from dronekit_sitl import SITL
		sitl = SITL()
		sitl.download('copter', '3.3', verbose=True)
		#sitl_args = ['-I0', '--model', 'quad', '--home=-35.363261,149.165230,584,353']
		sitl_args = ['-I0', '--model', 'quad']
		sitl.launch(sitl_args, await_ready=True, restart=True)
		

		# Import DroneKit-Python
		from dronekit import connect, VehicleMode

		# Connect to the Vehicle.
		# simulator
		target = 'tcp:127.0.0.1:5760' 
		# drone
		#target = 'udpin:0.0.0.0:14550'
		print "Connecting to vehicle on: " + target
		mission_state.vehicle = connect(target, wait_ready=True)	
		print "Connected to vehicle..."
		# mission_state.vehicle = connect('tcp:127.0.0.1:5760', wait_ready=True)

		print "App1: ", app_param


control_api = Blueprint("control_api", __name__)


def listen_for_location_change(vehicle_location_param, app_param):
		vehicle_location = vehicle_location_param
		while True:
			current_lat = mission_state.vehicle.location.global_relative_frame.lat
			current_lon = mission_state.vehicle.location.global_relative_frame.lon
			current_alt = mission_state.vehicle.location.global_relative_frame.alt	
			if (vehicle_location.lat != current_lat) or (vehicle_location.lon != current_lon) or (vehicle_location.alt != current_alt):
						loc = {'lat': current_lat,
           				 'lon': current_lon,
            			 'alt': current_alt}
						json_loc = json.dumps(loc)
						print "[socket][control][gps_pos]: ", mission_state.vehicle.location.global_relative_frame	
						vehicle_location = mission_state.vehicle.location.global_relative_frame
						print "App2: ", app_param
						with app_param.app_context():
							emit("gps_pos_ack", json_loc, broadcast=True)



def arm_and_takeoff(aTargetAltitude):
		"""
		Arms vehicle and fly to aTargetAltitude.
		"""
		while not mission_state.vehicle.is_armable:
				print "[control]: Waiting for vehicle to initialise..."
				time.sleep(1)

		print "[control]: Arming motors"
		# Copter should arm in GUIDED mode
		mission_state.vehicle.mode = VehicleMode("GUIDED")
		print "Setting mode to GUIDED"
		while not mission_state.vehicle.mode == VehicleMode("GUIDED"):
			time.sleep(1)
			print "mode: ", mission_state.vehicle.mode
		print "Final mode: ", mission_state.vehicle.mode

		mission_state.vehicle.armed		= True

		# Confirm vehicle armed before attempting to take off
		while not mission_state.vehicle.armed:
				print "[control]: Waiting for arming..."
				time.sleep(1)

		print "[control]: Taking off"
		mission_state.vehicle.simple_takeoff(aTargetAltitude)


@control_api.route("/control/take_off", methods = ["POST"])
def take_off():
		print "[/control/take_off]: launching drone"
		arm_and_takeoff(2)
		print "[/control/take_off]: takeoff complete"
		return json.dumps({'success':True}), 200, {'ContentType':'application/json'} 

@control_api.route("/control/land", methods = ["POST"])
def land():
		print "[/control/land]: landing drone"
		mission_state.vehicle.mode = VehicleMode("LAND")

		while True:
				print " Altitude: ", mission_state.vehicle.location.global_relative_frame.alt
				if mission_state.vehicle.location.global_relative_frame.alt <= 0.5:
						print "Landed on ground"
						break
				time.sleep(1)

		return json.dumps({'success':True}), 200, {'ContentType':'application/json'} 


