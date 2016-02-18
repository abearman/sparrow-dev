print "Start simulator (SITL)"
from dronekit_sitl import SITL
sitl = SITL()
sitl.download('solo', '1.2.0', verbose=True)
sitl_args = ['-I0', '--model', 'quad', '--home=-35.363261,149.165230,584,353']
sitl.launch(sitl_args, await_ready=True, restart=True)

# Import DroneKit-Python
from dronekit import connect, VehicleMode, LocationGlobal, LocationGlobalRelative
from pymavlink import mavutil
import time
import math

# Connect to the Vehicle.
print "Connecting to vehicle on: 'tcp:127.0.0.1:5760'"
vehicle = connect('tcp:127.0.0.1:5760', wait_ready=True)

# Get some vehicle attributes (state)
print "Get some vehicle attribute values:"
print " GPS: %s" % vehicle.gps_0
print " Battery: %s" % vehicle.battery
print " Last Heartbeat: %s" % vehicle.last_heartbeat
print " Is Armable?: %s" % vehicle.is_armable
print " System status: %s" % vehicle.system_status.state
print " Mode: %s" % vehicle.mode.name	 # settable


def hover(last_altitude):
	print "Old throttle: ", vehicle.parameters['RCMAP_THROTTLE']
	print "Old altitude: ", last_altitude
	new_altitude = vehicle.location.global_relative_frame.alt
	print "New altitude: ", new_altitude

	if new_altitude < last_altitude:
		if vehicle.parameters['RCMAP_THROTTLE'] < 8:
			vehicle.parameters['RCMAP_THROTTLE'] += 1
	elif new_altitude > last_altitude:
		if vehicle.parameters['RCMAP_THROTTLE'] > 0:
			vehicle.parameters['RCMAP_THROTTLE'] -= 1

	print "New throttle: ", vehicle.parameters['RCMAP_THROTTLE']
	print ""


def arm_and_takeoff(aTargetAltitude):
	"""
	Arms vehicle and fly to aTargetAltitude.
	"""

	print "Basic pre-arm checks"
	# Don't try to arm until autopilot is ready
	while not vehicle.is_armable:
		print " Waiting for vehicle to initialise..."
		time.sleep(1)

	print "Arming motors"
	vehicle.mode	= VehicleMode("ACRO")
	vehicle.armed	= True

	# Confirm vehicle armed before attempting to take off
	while not vehicle.armed:
		print " Waiting for arming..."
		time.sleep(1)

	print "Local location (before takeoffe: ", vehicle.location.local_frame

	print "Taking off!"
	#vehicle.simple_takeoff(aTargetAltitude)
	print "Throttle: ", vehicle.parameters['RCMAP_THROTTLE']
	vehicle.parameters['RCMAP_THROTTLE'] = 4.0
	print "Throttle: ", vehicle.parameters['RCMAP_THROTTLE']

	# Wait until the vehicle reaches a safe height before processing the goto (otherwise the command
	#  after Vehicle.simple_takeoff will execute immediately).
	while True:
		print " Altitude: ", vehicle.location.global_relative_frame.alt
		print "Throttle: ", vehicle.parameters['RCMAP_THROTTLE']
		#Break and return from function just below target altitude.
		if vehicle.location.global_relative_frame.alt>=aTargetAltitude*0.95:
			print "Reached target altitude"
			old_altitude = vehicle.location.global_relative_frame.alt
			time.sleep(0.1)
			hover(old_altitude)
			#break
		else:
			time.sleep(0.1)


arm_and_takeoff(10)


def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
	"""
	Move vehicle in direction based on specified velocity vectors.
	"""
	msg = vehicle.message_factory.set_position_target_local_ned_encode(
		0,		 # time_boot_ms (not used)
		0, 0,	 # target system, target component
		mavutil.mavlink.MAV_FRAME_LOCAL_NED, # frame
		0b0000111111000111, # type_mask (only speeds enabled)
		0, 0, 0, # x, y, z positions (not used)
		velocity_x, velocity_y, velocity_z, # x, y, z velocity in m/s
		0, 0, 0, # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
		0, 0)	 # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)


	# send command to vehicle on 100 Hz cycle
	vehicle.send_mavlink(msg)
	time.sleep(duration)

# move in a square 
print "Local location (after takeoff, starting point): ", vehicle.location.local_frame

send_ned_velocity(1, 0, 0, 0.5) # first move in x direction at 1 m/s for 5 s
print "Local location (point 1): ", vehicle.location.local_frame
print "Altitude: ", vehicle.location.global_relative_frame.alt

send_ned_velocity(0, 1, 0, 0.5)
print "Local location (point 2): ", vehicle.location.local_frame
print "Altitude: ", vehicle.location.global_relative_frame.alt

send_ned_velocity(-1, 0, 0, 0.5)
print "Local location (point 3): ", vehicle.location.local_frame
print "Altitude: ", vehicle.location.global_relative_frame.alt

send_ned_velocity(0, -1, 0, 0.5)
print "Local location (point 4): ", vehicle.location.local_frame
print "Altitude: ", vehicle.location.global_relative_frame.alt

# land at current location
time.sleep(1)
print "Altitude: ", vehicle.location.global_relative_frame.alt
vehicle.mode = VehicleMode("LAND")
time.sleep(100)
print "Local location (after landing): ", vehicle.location.local_frame

# Close vehicle object before exiting script
vehicle.close()

# Shut down simulator
sitl.stop()
print("Completed")
