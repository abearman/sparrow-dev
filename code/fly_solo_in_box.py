from dronekit import connect, VehicleMode
import time
import sys
from pymavlink import mavutil

# Connect to UDP endpoint (and wait for default attributes to accumulate)
target = sys.argv[1] if len(sys.argv) >= 2 else 'udpin:0.0.0.0:14550'
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

	#vehicle.parameters['THR_MIN']=140
	#print "Throttle min: %s" % vehicle.parameters['THR_MIN']

	#vehicle.channel_override = { "3" : 50}
#	vehicle.flush()

	#print "Basic pre-arm checks"
	# Don't try to arm until autopilot is ready
	while not vehicle.is_armable:
		print " Waiting for vehicle to initialise..."
		time.sleep(1)
	
	print "Arming motors"
    # Copter should arm in GUIDED mode
	vehicle.mode    = VehicleMode("GUIDED")
	vehicle.armed   = True

	# Confirm vehicle armed before attempting to take off
	while not vehicle.armed:
		print " Waiting for arming..."
		time.sleep(1)

	print "Local location (before takeoff): ", vehicle.location.local_frame
	print "Taking off!"
	vehicle.simple_takeoff(aTargetAltitude)

	while True:
		print " Altitude: ", vehicle.location.global_relative_frame.alt
		#Break and return from function just below target altitude.
		if vehicle.location.global_relative_frame.alt>=aTargetAltitude*0.95:
			print "Reached target altitude"
			break
		time.sleep(1)

arm_and_takeoff(3)

def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
    """
    Move vehicle in direction based on specified velocity vectors.
    """
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

# move in a square
print "Local location (after takeoff, starting point): ", vehicle.location.local_frame

send_ned_velocity(0, -1, 0, 5)
#send_ned_velocity(0, 1, 0, 10)
time.sleep(100)


# land at current location)
vehicle.mode = VehicleMode("LAND")
print "Local location (after landing): ", vehicle.location.local_frame


vehicle.close()
sitl.stop()
print "Done."
