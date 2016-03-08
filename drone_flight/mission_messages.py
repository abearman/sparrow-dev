from dronekit_sitl import SITL
sitl = SITL()
#sitl.download('solo', '1.2.0', verbose=True)
sitl.download('copter', '3.3', verbose=True)
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
print "Connected to vehicle"


# Get the set of commands from the vehicle
cmds = vehicle.commands
cmds.download()
cmds.wait_ready()
print "Downloaded commands"

# Create and add commands
cmd0=Command( 0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_MISSION_START, 0, 0, 0, 2, 0, 0, 0, 0, 0)
#cmd1=Command( 0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_COMPONENT_ARM_DISARM, 0, 0, 1, 0, 0, 0, 0, 0, 0)
cmd2=Command( 0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, 0, 0, 10, 10, 10)
cmd3=Command( 0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_WAYPOINT, 0, 0, 0, 0, 0, 0, 10, 10, 10)
#cmds.add(cmd1)
#cmds.add(cmd2)
#cmds.add(cmd3)
#cmds.add(cmd0)
#cmds.upload() # Send commands
#print "Uploaded commands"


print "Basic pre-arm checks"
# Don't try to arm until autopilot is ready
while not vehicle.is_armable:
  print " Waiting for vehicle to initialise..."
  time.sleep(1)

print "Arming motors"
print "Mode: ", vehicle.mode.name
vehicle.mode  = VehicleMode("GUIDED")
vehicle.armed = True

# Confirm vehicle armed before attempting to take off
while not vehicle.armed:
  print " Waiting for arming..."
  time.sleep(1)
print "Armed"

print "Taking off"


#print dir(vehicle)
vehicle.mode = VehicleMode("GUIDED")
#print "Set to auto mode"

vehicle.simple_takeoff(10)

def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
	msg = vehicle.message_factory.set_position_target_local_ned_encode(
        0,       # time_boot_ms (not used)
        0, 0,    # target system, target component
     	mavutil.mavlink.MAV_FRAME_BODY_OFFSET_NED, # frame
        0b0000111111000111, # type_mask (only speeds enabled)
        0, 0, 0, # x, y, z positions (not used)
        velocity_x, velocity_y, velocity_z, # x, y, z velocity in m/s
        0, 0, 0, # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
        0, 0)    # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)

	for x in range(0,duration):
		vehicle.send_mavlink(msg)
		print " Altitude: ", vehicle.location.global_relative_frame.alt
		time.sleep(1)

send_ned_velocity(0, 0, -2, 5)

while True:
	print " Altitude: ", vehicle.location.global_relative_frame.alt
	print "Local location: ", vehicle.location.local_frame
	#Break and return from function just below target altitude.
	time.sleep(1)
