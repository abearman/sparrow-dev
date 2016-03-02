print "Start simulator (SITL)"
from dronekit_sitl import SITL
sitl = SITL()
sitl.download('solo', '1.2.0', verbose=True)
sitl_args = ['-I0', '--model', 'quad', '--home=-35.363261,149.165230,584,353']
sitl.launch(sitl_args, await_ready=True, restart=True)

# Import DroneKit-Python
from dronekit import connect, VehicleMode, LocationGlobal, LocationGlobalRelative, Command
from pymavlink import mavutil
import time
import math
import inspect

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

#print dir(vehicle)
#print ""
#print vehicle.parameters.keys()
#print vehicle._mode_mapping

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
	vehicle.mode	= VehicleMode("GUIDED")
	vehicle.armed	= True

	# vehicle climbs automatically to PILOT_TKOFF_ALT alt when taking off in ALT_HOLD mode
	#vehicle.parameters['PILOT_TKOFF_ALT'] = 5

	# Confirm vehicle armed before attempting to take off
	while not vehicle.armed:
		print " Waiting for arming..."
		time.sleep(1)

	

	#print "Taking off!"
	#print "Current throttle: ", vehicle.parameters['RCMAP_THROTTLE']
	#vehicle.parameters['RCMAP_THROTTLE'] = 8.0 
	#print "Current throttle: ", vehicle.parameters['RCMAP_THROTTLE']

	#vehicle.simple_takeoff(aTargetAltitude)
	
	vehicle.mode = VehicleMode("ALT_HOLD")
	
	print dir(vehicle.message_factory)

	cmds = vehicle.commands
	cmds.download()
	cmds.wait_ready()
	cmds.clear()	
	arm_cmd = Command(0, 0, 0,
                        mavutil.mavlink.MAV_FRAME_BODY_NED,
                        mavutil.mavlink.MAV_CMD_COMPONENT_ARM_DISARM,
                        0,  # current (set to 0) 
												0,	# autocontinue (set to 0)
												1,  # param 1 
												0, 0, 0,  # params 2-4
												0, 0, 0)  # x, y, z
	takeoff_cmd = Command(0, 0, 0,  
												mavutil.mavlink.MAV_FRAME_BODY_NED,
												mavutil.mavlink.MAV_CMD_NAV_TAKEOFF,
  											1, 0, 0, 0, 0, 0, 0, 0, 20.0)	
	#cmds.add(arm_cmd)
	cmds.add(takeoff_cmd)
	cmds.upload()

	while True:
		print " Altitude: ", vehicle.location.global_relative_frame.alt
		#Break and return from function just below target altitude.
		if vehicle.location.global_relative_frame.alt>=aTargetAltitude*0.95:
			print "Reached target altitude"
			break
		time.sleep(1)

	#print inspect.getargspec(vehicle.message_factory.altitude_encode)
	# args=['self', 'time_usec', 'altitude_monotonic', 'altitude_amsl', 'altitude_local', 'altitude_relative', 'altitude_terrain', 'bottom_clearance']

	#alt_msg = vehicle.message_factory.altitude_send(5.0, -20, -20, -20.0, 20, -20, -20)
	#vehicle.send_mavlink(alt_msg)

	vehicle.message_factory.set_position_target_local_ned_send(
	  0,			# time_boot_ms (not used)
	  0, 0,	# target system, target component
	  mavutil.mavlink.MAV_FRAME_BODY_NED, # frame
	  0b0000111111000111, # type_mask (only speeds enabled)
	  0, 0, 0, # x, y, z positions (not used)
	  0, 0, -1, # x, y, z velocity in m/s
	  0, 0, 0, # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
	  0, 0)	# yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)
	#vehicle.send_mavlink(takeoff_msg)

	time.sleep(5)	
	print "Altitude: ", vehicle.location.global_relative_frame.alt
	time.sleep(5)
	print "Altitude: ", vehicle.location.global_relative_frame.alt




arm_and_takeoff(10)
#vehicle.mode = VehicleMode("ALT_HOLD")
time.sleep(5)



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
#print "Local location (after takeoff, starting point): ", vehicle.location.local_frame

#send_ned_velocity(1, 0, 0, 0.5) # first move in x direction at 1 m/s for 5 s
#print "Local location (point 1): ", vehicle.location.local_frame
#print "Altitude: ", vehicle.location.global_relative_frame.alt

#send_ned_velocity(0, 1, 0, 0.5)
#print "Local location (point 2): ", vehicle.location.local_frame
#print "Altitude: ", vehicle.location.global_relative_frame.alt

#send_ned_velocity(-1, 0, 0, 0.5)
#print "Local location (point 3): ", vehicle.location.local_frame
#print "Altitude: ", vehicle.location.global_relative_frame.alt

#send_ned_velocity(0, -1, 0, 0.5)
#print "Local location (point 4): ", vehicle.location.local_frame
#print "Altitude: ", vehicle.location.global_relative_frame.alt

# land at current location
time.sleep(10)
print "Altitude: ", vehicle.location.global_relative_frame.alt
vehicle.mode = VehicleMode("LAND")
print "Local location (after landing): ", vehicle.location.local_frame

# Close vehicle object before exiting script
vehicle.close()

# Shut down simulator
sitl.stop()
print("Completed")
