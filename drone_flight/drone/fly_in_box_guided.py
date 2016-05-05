from dronekit import connect, VehicleMode, LocationGlobalRelative
import time
import sys
from pymavlink import mavutil
from dronekit_sitl import SITL
import math

# Connect to UDP endpoint (and wait for default attributes to accumulate)
target = sys.argv[1] if len(sys.argv) >= 2 else 'udpin:0.0.0.0:14550'

#sitl = SITL()
#sitl.download('solo', '1.2.0', verbose=True)
#sitl_args = ['-I0', '--model', 'quad', '--home=-35.363261,149.165230,584,353']
#sitl_args = ['-I0', '--model', 'quad', '--home=37.430020,-122.173302,28,353']
#sitl.launch(sitl_args, await_ready=True, restart=True)
#target = "tcp:127.0.0.1:5760"

print 'Connecting to ' + target + '...'
vehicle = connect(target, wait_ready=True)

# Get all vehicle attributes (state)
print "Vehicle state:"
print " Global Location: %s" % vehicle.location.global_frame
print " Global Location (relative altitude): %s" % vehicle.location.global_relative_frame
print " Local Location: %s" % vehicle.location.local_frame
print " Attitude: %s" % vehicle.attitude
print " Velocity: %s" % vehicle.velocity
print " Battery: %s" % vehicle.battery
print " Last Heartbeat: %s" % vehicle.last_heartbeat
print " Heading: %s" % vehicle.heading
print " Groundspeed: %s" % vehicle.groundspeed
print " Airspeed: %s" % vehicle.airspeed
print " Mode: %s" % vehicle.mode.name
print " Is Armable?: %s" % vehicle.is_armable
print " Armed: %s" % vehicle.armed

def arm_and_takeoff(aTargetAltitude):
	"""
	Arms vehicle and fly to aTargetAltitude.
	"""

	#print "Sending mission start cmd"
	#vehicle._master.mav.command_long_send(0, 0, mavutil.mavlink.MAV_CMD_MISSION_START,
   #                                              0, 0, 0, 0, 0, 0, 0, 0)

	#print "Sending Tango mavlink message"
	#vehicle._master.mav.command_long_send(0, 0, mavutil.mavlink.MAV_CMD_NAV_SEND_TANGO_GPS,
	#																								0, 0, 0, 0, 0, 37.4, 149, 20)

	#print "Sending new mavlink message"
	#msg = vehicle.message_factory.command_long_encode(
	#	0, 0, # target_system, target_component 
	#	mavutil.mavlink.MAV_CMD_NAV_SEND_TANGO_GPS,
	#	0, # confirmation
	#	0, 0, 0, 0, # param 1-4 not used
	#	37.4, 149, 20)
	#vehicle.send_mavlink(msg)

	# Copter should arm in GUIDED mode
	vehicle.mode = VehicleMode("GUIDED")
	while not vehicle.mode == VehicleMode("GUIDED"):
		time.sleep(1)

	print "Basic pre-arm checks"
	# Don't try to arm until autopilot is ready
	while not vehicle.is_armable:
		print " Waiting for vehicle to initialise..."
		time.sleep(1)

	# Copter should arm in GUIDED mode
	vehicle.mode = VehicleMode("GUIDED")
	while not vehicle.mode == VehicleMode("GUIDED"):
		time.sleep(1)
		#print "Flight mode: ", vehicle.mode
	print "Final flight mode: ", vehicle.mode
	
	print "Arming motors"

	#print "Skipping GPS"
	#vehicle.parameters['ARMING_CHECK'] = -9

	vehicle.armed		= True

	# Confirm vehicle armed before attempting to take off
	while not vehicle.armed:
		print " Waiting for arming..."
		time.sleep(0.1)
	print "Armed!"

	#print "Local location (before takeoff): ", vehicle.location.local_frame
	#print "Taking off!"
	#vehicle.simple_takeoff(aTargetAltitude)


	#print "Sending new mavlink message"
	#msg = vehicle.message_factory.command_long_encode(
	# 0, 0, # target_system, target_component 
	# mavutil.mavlink.MAV_CMD_NAV_SEND_TANGO_GPS,
	# 0, # confirmation
	# 0, 0, 0, 0, # param 1-4 not used
	# 37.4, 149, 20)
	#vehicle.send_mavlink(msg)

	print "Sending Tango mavlink message"
	vehicle._master.mav.command_long_send(0, 0, mavutil.mavlink.MAV_CMD_NAV_SEND_TANGO_GPS,
																									0, 0, 0, 0, 0, 37.4, 149, 20)

	print "Sending takeoff mavlink message"
	vehicle._master.mav.command_long_send(0, 0, mavutil.mavlink.MAV_CMD_NAV_TAKEOFF,
																									0, 0, 0, 0, 0, 0, 0, 10)

	while True:
		print vehicle.location.global_relative_frame
		time.sleep(0.5)	
	#time.sleep(10)
	exit()

	print "Local location (before takeoff): ", vehicle.location.local_frame
	print "Taking off!"
	vehicle.simple_takeoff(aTargetAltitude)

	while vehicle.location.global_relative_frame.alt < aTargetAltitude:
		print " Altitude: ", vehicle.location.global_relative_frame.alt
		#Break and return from function just below target altitude.
		if vehicle.location.global_relative_frame.alt>=aTargetAltitude*0.95:
			print "Reached target altitude"
			break
		time.sleep(1)

arm_and_takeoff(2)
print "Finished takeoff"

def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
		"""
		Move vehicle in direction based on specified velocity vectors.
		"""
		msg = vehicle.message_factory.set_position_target_local_ned_encode(
				0,			 # time_boot_ms (not used)
				0, 0,		 # target system, target component
				mavutil.mavlink.MAV_FRAME_LOCAL_NED, # frame
				0b0000111111000111, # type_mask (only speeds enabled)
				0, 0, 0, # x, y, z positions (not used)
				velocity_x, velocity_y, velocity_z, # x, y, z velocity in m/s
				0, 0, 0, # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
				0, 0)		 # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)


		# send command to vehicle on 1 Hz cycle
		for x in range(0,duration):
				vehicle.send_mavlink(msg)
				print vehicle.location.local_frame
				print vehicle.attitude
				print "Channels: ", vehicle.channels
				time.sleep(1)


def condition_yaw(heading, relative=False):
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

		while abs(vehicle.attitude.yaw - target_yaw) > 0.1:
			pass
	
		print "Target yaw: ", target_yaw
		print "Final yaw: ", vehicle.attitude


def send_global_velocity(velocity_x, velocity_y, velocity_z, duration):
		"""
		Move vehicle in direction based on specified velocity vectors.
		"""
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
		for x in range(0,duration):
				vehicle.send_mavlink(msg)
				time.sleep(1)


def goto_position_target_global_int(aLocation):
		"""
		Send SET_POSITION_TARGET_GLOBAL_INT command to request the vehicle fly to a specified location.
		"""
		msg = vehicle.message_factory.set_position_target_global_int_encode(
				0,			 # time_boot_ms (not used)
				0, 0,		 # target system, target component
				mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT_INT, # frame
				0b0000111111111000, # type_mask (only speeds enabled)
				aLocation.lat*1e7, # lat_int - X Position in WGS84 frame in 1e7 * meters
				aLocation.lon*1e7, # lon_int - Y Position in WGS84 frame in 1e7 * meters
				aLocation.alt, # alt - Altitude in meters in AMSL altitude, not WGS84 if absolute or relative, above terrain if GLOBAL_TERRAIN_ALT_INT
				0, # X velocity in NED frame in m/s
				0, # Y velocity in NED frame in m/s
				0, # Z velocity in NED frame in m/s
				0, 0, 0, # afx, afy, afz acceleration (not supported yet, ignored in GCS_Mavlink)
				0, 0)		 # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)

		# send command to vehicle
		vehicle.send_mavlink(msg)


def change_altitude_global(target_alt):
	print vehicle.location.global_relative_frame
	current_lat = vehicle.location.global_relative_frame.lat
	current_lon = vehicle.location.global_relative_frame.lon 
	current_alt = vehicle.location.global_relative_frame.alt
	target_location = LocationGlobalRelative(current_lat, current_lon, target_alt)
	vehicle.simple_goto(target_location)

	while abs(target_alt - vehicle.location.global_relative_frame.alt) > 0.01:
		pass
	print vehicle.location.global_relative_frame


#change_altitude_global(10)
#condition_yaw(90, relative=True)

# move in a square
#print "Local location (after takeoff, starting point): ", vehicle.location.local_frame

#for i in range(0, 0):
#	send_ned_velocity(0, -1, 0, 3)
#	print "Finished first command"
#	send_ned_velocity(1, 0, 0, 3)
#	print "Finished second command"
#	send_ned_velocity(0, 1, 0, 3)
#	print "Finished third command"
#	send_ned_velocity(-1, 0, 0, 3)
#	print "Finished fourth command"

# land at current location)
#vehicle.mode = VehicleMode("LAND")
#print "Local location (after landing): ", vehicle.location.local_frame

#time.sleep(10)

while True:
	condition_yaw(45, relative=True)


while True:
	print vehicle.channels
	time.sleep(1)

vehicle.close()
#sitl.stop()
print "Done."
