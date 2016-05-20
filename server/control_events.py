# Events and associated callbacks for server-side sockets

from flask import session
from flask.ext.socketio import emit, join_room, leave_room
import json
from threading import Thread

import mission_state
from mission_state import gps_init
import navigation
from server_state import socketio

from dronekit import connect, VehicleMode, LocationGlobalRelative, Command, LocationGlobal
import time
import sys
from pymavlink import mavutil
import eventlet
eventlet.monkey_patch()

import math

# TODO: namespace
CONTROL_NAMESPACE = "/"

@socketio.on('connect', namespace=CONTROL_NAMESPACE)
def on_connect():
		print "[socket][control][connect]: Connection received"
		eventlet.spawn(listen_for_location_change, 
			[mission_state.vehicle.location.global_relative_frame, mission_state.vehicle.attitude.yaw])


# Updates the location of the drone on a 1 Hz cycle
# Arguments are a tuple
def listen_for_location_change(vehicle_location_params):
		vehicle_location = vehicle_location_params[0]
		vehicle_yaw = vehicle_location_params[1]
		while True:
			current_lat = mission_state.vehicle.location.global_relative_frame.lat
			current_lon = mission_state.vehicle.location.global_relative_frame.lon
			current_alt = mission_state.vehicle.location.global_relative_frame.alt
			current_yaw = mission_state.vehicle.attitude.yaw
			if (vehicle_location.lat != current_lat) or (vehicle_location.lon != current_lon) or (vehicle_location.alt != current_alt) or (vehicle_yaw != current_yaw):
						loc = {
							'lat': current_lat,
							'lon': current_lon,
							'alt': current_alt,
							'yaw': current_yaw
						}
						json_loc = json.dumps(loc)
						print "[socket][control][gps_pos]: ", str(json_loc)
						socketio.emit("gps_pos_ack", json_loc, broadcast=True)
			eventlet.sleep(1)

#@socketio.on('gps_pos_tango') # , namespace=CONTROL_NAMESPACE)
#def gpsChangeTango(json):
#	print "[socket][control][gps_pos]: " + str(json)
#	emit("gps_pos_ack", json, broadcast=True)

@socketio.on('gps_pos') # , namespace=CONTROL_NAMESPACE)
def gpsChange(json):
		print "GOT TO GPS_POS FUNC"
		loc = json
		global gps_init
		if gps_init == False:
			navigation.getOrigin(json)  # sets mission_state.origin_ecef in navigation.py
			gps_init = True
			print "[socket][control][gps_pos]: " + str(json)
			emit("gps_pos_ack", json, broadcast=True)

STEP = 10
RADIAL_OFFSETS = [(1, 0), (1, 1), (-1, 1), (-1, -1), (2, -1), (2, 2), (-2, 2), (-2, -2), (3, -2), (3, 3)]
LINE_OFFSETS = [(0, 1), (-4, 1), (-4, 2), (0, 2), (0, 3), (-4, 3), (-4, 4), (0, 4), (0, 5), (-4, 5)]
SECTOR_OFFSETS = [(4, 0), (3, -1), (1, 1), (2, 2), (2, -2), (1, -1), (3, 1)]

def get_location_metres(original_location, dNorth, dEast):
    """
    Returns a LocationGlobal object containing the latitude/longitude `dNorth` and `dEast` metres from the 
    specified `original_location`. The returned Location has the same `alt` value
    as `original_location`.

    The function is useful when you want to move the vehicle around specifying locations relative to 
    the current vehicle position.
    The algorithm is relatively accurate over small distances (10m within 1km) except close to the poles.
    For more information see:
    http://gis.stackexchange.com/questions/2951/algorithm-for-offsetting-a-latitude-longitude-by-some-amount-of-meters
    """
    earth_radius=6378137.0 #Radius of "spherical" earth
    #Coordinate offsets in radians
    dLat = dNorth/earth_radius
    dLon = dEast/(earth_radius*math.cos(math.pi*original_location.lat/180))

    #New position in decimal degrees
    newlat = original_location.lat + (dLat * 180/math.pi)
    newlon = original_location.lon + (dLon * 180/math.pi)
    return LocationGlobal(newlat, newlon,original_location.alt)


@socketio.on('sar_path')
def flySARPath(data):
	print "[socket][control][sar_path]: " + str(data)

	vehicle = mission_state.vehicle
	lat = data['lat']
	lon = data['lon']
	altitude = data['altitude']
	path_type = data['sar_type']
	
	cmds = vehicle.commands
	cmds.clear()

	#Add MAV_CMD_NAV_TAKEOFF command. This is ignored if the vehicle is already in the air.
	cmds.add(Command( 0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, 0, 0, 0, 0, 4))

	waypoint_list = None
	if path_type == 'line':
		waypoint_list = LINE_OFFSETS
	elif path_type == 'sector':
		waypoint_list = SECTOR_OFFSETS
	elif path_type == 'radial':
		waypoint_list = RADIAL_OFFSETS

	for wp in waypoint_list:
		
		cmds.clear()

		point = get_location_metres(vehicle.location.global_frame, wp[1]	* STEP, wp[0]	* STEP)
		cmds.add(Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_WAYPOINT, 0, 0, 0, 0, 0, 0, point.lat, point.lon, altitude))
		cmds.add(Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_WAYPOINT, 0, 0, 0, 0, 0, 0, point.lat, point.lon, altitude))

		cmds.upload()

		vehicle.commands.next = 0
		vehicle.mode = VehicleMode("AUTO")
		while True:

			current_lat = mission_state.vehicle.location.global_relative_frame.lat
			current_lon = mission_state.vehicle.location.global_relative_frame.lon
			current_alt = mission_state.vehicle.location.global_relative_frame.alt
			loc = {'lat': current_lat, 'lon': current_lon, 'alt': current_alt}
			json_loc = json.dumps(loc)
			
			#print "[socket][control][sar_path_gps_pos]: ", str(json_loc)
			#socketio.emit("gps_pos_ack", json_loc, broadcast=True)

			nextwaypoint = vehicle.commands.next
			if path_type == 'line':
				if nextwaypoint == 2:
					break;
			if path_type == 'sector':
				if nextwaypoint == 2:
					break;
			if path_type == 'radial':
				if nextwaypoint == 2:
					break;

		vehicle.mode = VehicleMode("GUIDED")

"""
	
	# Call dronekit gps waypoint flight command with list of waypoints
	for wp in waypoint_list:
		print "GOING TO WAYPOINT, ", wp
		wp_lat = wp[0]
		wp_lon = wp[1]
		wp_alt = wp[2] 
		waypoint_location = LocationGlobalRelative(wp_lat, wp_lon, wp_alt)
	  	vehicle.simple_goto(waypoint_location)
	  	print "vehicle.location.global_relative_frame.lat: ", vehicle.location.global_relative_frame.lat
	  	print "lat: ", wp_lat
	  	print "diff: ", abs(vehicle.location.global_relative_frame.lat - wp_lat)
	  	print "vehicle.location.global_relative_frame.lon: ", vehicle.location.global_relative_frame.lon
	  	print "lon: ", wp_lon
	  	print "diff: ", abs(vehicle.location.global_relative_frame.lon - wp_lon)
	  	counter = 0
		while ((abs(vehicle.location.global_relative_frame.lat - wp_lat) > 0.00001) or
					 (abs(vehicle.location.global_relative_frame.lon - wp_lon) > 0.00001)): 
			time.sleep(0.1)
			counter = counter + 1
		print "OUT OF WHILE LOOP, counter = ", counter
	"""



@socketio.on('altitude_abs_cmd') # , namespace=CONTROL_NAMESPACE)
def altitudeAbsChange(json):
		print "[socket][control][altitude]: " + str(json)
		target_alt = float(json['altitude'])
		change_altitude_global(target_alt)

@socketio.on('altitude_cmd')
def altitudeChange(json):
		vehicle = mission_state.vehicle
		print "[socket][control][altitude]: " + str(json)
		dalt = float(json['dalt'])
		curr_alt = vehicle.location.global_relative_frame.alt
		if (curr_alt + dalt < 2):
			change_altitude_global(2)
 		elif (curr_alt + dalt > 10):
 			change_altitude_global(10)
 		else:
 			change_altitude_global(curr_alt + dalt)	

@socketio.on('rotation_cmd') # , namespace=CONTROL_NAMESPACE)
def rotationChange(json):
		print "[socket][control][rotation]: " + str(json)
		heading = float(json['heading'])
		condition_yaw(heading)

@socketio.on('waypoint_cmd')
def waypointCommand(json):
	vehicle = mission_state.vehicle
	# vehicle.airspeed = 4
	print "airspeed: ", vehicle.airspeed
	print "[socket][control][waypoint]: " + str(json)
	lat = float(json['lat'])
	lon = float(json['lon'])
	if 'alt' in json:
		alt = float(json['alt'])
	else:
		alt = vehicle.location.global_relative_frame.alt
	waypoint_location = LocationGlobalRelative(lat, lon, alt)
	vehicle.simple_goto(waypoint_location, airspeed = 1.0)

@socketio.on('lateral_cmd') #, namespace=CONTROL_NAMESPACE)
def lateralChangeDiscrete(json):
	print "[socket][control][lateral]: " + str(json)
	direction = json['direction']
	# args are x_vel, y_vel, z_vel, duration
	if direction == "left":
		send_ned_velocity(-1, 0, 0, 1)
	elif direction == "right":
		send_ned_velocity(1, 0, 0, 1)
	elif direction == "forward":
		send_ned_velocity(0, -1, 0, 1)
	elif direction == "back":
		send_ned_velocity(0, 1, 0, 1)
	elif direction == "stop":
		send_ned_velocity(0, 0, 0, 1)


def lateralChangeJoystick(json):
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


def condition_yaw(heading, relative=True):
		vehicle = mission_state.vehicle

		direction = 1
		if heading < 0:
			direction = -1

		send_global_velocity(0, 0, 0, 1)
		if relative:
				is_relative=1 #yaw relative to direction of travel
		else:
				is_relative=0 #yaw is an absolute angle
		# create the CONDITION_YAW command using command_long_encode()
		msg = vehicle.message_factory.command_long_encode(
				0, 0,		 # target system, target component
				mavutil.mavlink.MAV_CMD_CONDITION_YAW, #command
				0, #confirmation
				abs(heading),		# param 1, yaw in degrees. Makes this a positive value
				0,					# param 2, yaw speed deg/s
				direction,					# param 3, direction -1 ccw, 1 cw
				is_relative, # param 4, relative offset 1, absolute angle 0
				0, 0, 0)		# param 5 ~ 7 not used
		# send command to vehicle
		vehicle.send_mavlink(msg)

		yaw_before = vehicle.attitude.yaw
		target_yaw = 0
		if relative:
			desired_yaw_change = math.radians(heading)
			target_yaw = yaw_before + desired_yaw_change
		else:
			target_yaw = math.radians(heading)

		#while abs(vehicle.attitude.yaw - target_yaw) > 0.01:
		#	pass


def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
		"""
		Move vehicle in direction based on specified velocity vectors.
		"""
		vehicle = mission_state.vehicle
		msg = vehicle.message_factory.set_position_target_local_ned_encode(
				0,			 # time_boot_ms (not used)
				0, 0,		 # target system, target component
				mavutil.mavlink.MAV_FRAME_BODY_NED, # frame
				0b0000111111000111, # type_mask (only speeds enabled)
				0, 0, 0, # x, y, z positions (not used)
				velocity_x, velocity_y, velocity_z, # x, y, z velocity in m/s
				0, 0, 0, # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
				0, 0)		 # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)


		# send command to vehicle on 1 Hz cycle
		for x in range(0, duration):
			vehicle.send_mavlink(msg)
			time.sleep(1)


def send_global_velocity(velocity_x, velocity_y, velocity_z, duration):
		"""
		Move vehicle in direction based on specified velocity vectors.
		"""
		vehicle = mission_state.vehicle
		msg = vehicle.message_factory.set_position_target_global_int_encode(
				0,			 # time_boot_ms (not used)
				0, 0,		 # target system, target component
				mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT_INT, # frame
				0b0000111111000111, # type_mask (only speeds enabled)
				0, # lat_int - X Position in WGS84 frame in 1e7 * meters
				0, # lon_int - Y Position in WGS84 frame in 1e7 * meters
				0, # alt - Altitude in meters in AMSL altitude(not WGS84 if absolute or relative)
				# altitude above terrain if GLOBAL_TERRAIN_ALT_INT
				velocity_x, # X velocity in NED frame in m/s
				velocity_y, # Y velocity in NED frame in m/s
				velocity_z, # Z velocity in NED frame in m/s
				0, 0, 0, # afx, afy, afz acceleration (not supported yet, ignored in GCS_Mavlink)
				0, 0)		 # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)

		# send command to vehicle on 1 Hz cycle
		for x in range(0, duration):
				vehicle.send_mavlink(msg)
				time.sleep(1)


def change_altitude_global(target_alt):
		vehicle = mission_state.vehicle
		target_location = LocationGlobalRelative(vehicle.location.global_relative_frame.lat,
																						 vehicle.location.global_relative_frame.lon,
																						 target_alt)
		vehicle.simple_goto(target_location)

		while abs(target_alt - vehicle.location.global_relative_frame.alt) > 0.1:
			time.sleep(0.5)

