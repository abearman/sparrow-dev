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
cmd2=Command( 0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, 0, 0, 0, 0, 10)
cmd3=Command( 0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_WAYPOINT, 0, 0, 0, 0, 0, 0, 10, 10, 10)
#cmds.add(cmd1)
cmds.add(cmd2)
cmds.add(cmd3)
print "Added first two commmands"
cmds.add(cmd0)
cmds.upload() # Send commands
print "Uploaded commands"


print "Basic pre-arm checks"
# Don't try to arm until autopilot is ready
while not vehicle.is_armable:
  print " Waiting for vehicle to initialise..."
  time.sleep(1)

print "Arming motors"
print "Mode: ", vehicle.mode.name
#vehicle.mode  = VehicleMode("GUIDED")
vehicle.armed = True

# Confirm vehicle armed before attempting to take off
while not vehicle.armed:
  print " Waiting for arming..."
  time.sleep(1)
print "Armed"


# Set the vehicle into auto mode
print dir(vehicle)
vehicle.mode = VehicleMode("AUTO")
print "Set to auto mode"

print "Mode: ", vehicle.mode

while True:
	print " Altitude: ", vehicle.location.global_relative_frame.alt
	print "Local location: ", vehicle.location.local_frame
	#Break and return from function just below target altitude.
	time.sleep(1)
