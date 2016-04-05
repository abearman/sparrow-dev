# Import DroneKit-Python
from dronekit import connect, VehicleMode, LocationGlobal, LocationGlobalRelative, LocationLocal, Command
from pymavlink import mavutil
import time
import math
import inspect
import numpy as np
import matplotlib.pyplot as plt
import pylab

# 1 = Roll
# 2 = Pitch
# 3 = Throttle
# 4 = Yaw

# LocalLocation: (North, East, Down). In meters. Down is negative
# Attitude: (Pitch, Yaw, Roll). In radians.

from dronekit_sitl import SITL
sitl = SITL()
sitl.download('solo', '1.2.0', verbose=True)
#sitl.download('copter', '3.3', verbose=True)
sitl_args = ['-I0', '--model', 'quad', '--home=-35.363261,149.165230,584,353']
sitl.launch(sitl_args, await_ready=True, restart=True)


# pwms are in microseconds. 1000 = 0%, 2000 = 100%
def send_servo_pwm(servo_num, pwm, duration):
	servo_pwms = [1700]*8
	servo_pwms[servo_num] = pwm		
	#msg = vehicle.message_factory.servo_output_raw_send(duration, 7, *servo_pwms)		
	msg = vehicle.message_factory.command_long_encode(
		0, 0,		 # target_system, target_component
		mavutil.mavlink.MAV_CMD_DO_SET_SERVO, #command
		0, #confirmation
		3,	# param 1, servo number
		1700,	# param 2, PWM
		0, 0, 0, 0, 0)	# params 3-7, not used
	# send command to vehicle
	vehicle.send_mavlink(msg)
	vehicle.flush()


# pwms are in microseconds. 1000 = 0%, 2000 = 100%
def send_channel_pwm(channel_num, pwm):
	channel_pwms = [0]*8
	channel_pwms[channel_num] = pwm
	msg = vehicle.message_factory.command_long_encode(
		0, 0,		 # target_system, target_component
		mavutil.mavlink.RC_CHANNELS_OVERRIDE, #command
		*channel_pwms)
	# send command to vehicle
	vehicle.send_mavlink(msg)
	vehicle.flush()


def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
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

def condition_yaw(heading, relative=False):
    if relative:
        is_relative=1 #yaw relative to direction of travel
    else:
        is_relative=0 #yaw is an absolute angle
    # create the CONDITION_YAW command using command_long_encode()
    msg = vehicle.message_factory.command_long_encode(
        0, 0,    # target system, target component
        mavutil.mavlink.MAV_CMD_CONDITION_YAW, #command
        0, #confirmation
        0,    # param 1, yaw in degrees (heading)
        10,          # param 2, yaw speed deg/s
        1,          # param 3, direction -1 ccw, 1 cw
        is_relative, # param 4, relative offset 1, absolute angle 0
        0, 0, 0)    # param 5 ~ 7 not used
    # send command to vehicle
    vehicle.send_mavlink(msg)


def channel_override():
	msg = vehicle.message_factory.rc_channels_override_encode(
		0, 0,  # Target system, target component0,
		1800,  # Channel 1
		1800,  # Channel 2
		1800,  # Channel 3,
		1800, 1800, 1800, 1800, 1800  # Channels 4-8 
	)	

def calculateDistance2D(location1, location2):
	dist = math.sqrt((location1.east - location2.east)**2 + (location1.north - location2.north)**2)
	return dist


# delta_T is in seconds
def controller_pid(error, delta_T):
	current_error = error[-1]
	previous_error = error[-2] if len(error) > 1 else 0
	bias = 1537
	K_p = 17
	K_i = 0 
	K_d = 7  
	P = K_p * current_error
	I = K_i * delta_T * sum(error)
	D = K_d * (current_error - previous_error) / delta_T
	return P + I + D + bias


# Connect to the Vehicle.
print "Connecting to vehicle on: 'tcp:127.0.0.1:5760'"
vehicle = connect('tcp:127.0.0.1:5760', wait_ready=True)
print "Connected to vehicle"

waypoints = [LocationLocal(10, 0, -10.0)]

while not vehicle.is_armable:
        print " Waiting for vehicle to initialise..."
        time.sleep(1)

vehicle.mode = VehicleMode("STABILIZE")
vehicle.armed = True

while not vehicle.armed:
	print " Waiting for arming..."
	time.sleep(1)
print "Armed!"

vehicle.channels.overrides['3'] = 1800


# Ascend
while (vehicle.location.local_frame.down is None) or ((-1 * vehicle.location.local_frame.down) < 20.0):
	time.sleep(1)
	print "Still ascending"
	print "Local location: ", vehicle.location.local_frame
vehicle.channels.overrides['3'] = 1500
print "Finished ascending"

def special_function(target_east=20.0, time_to_break=30.0):
	delta_T = 0.05	# seconds (20 Hz)
	error = []
	PV = []  # Array of past process variables (i.e., altitudes)

	ln, = plt.plot(error)
	plt.ion()
	plt.show()
	plt.xlabel('time (seconds)')
	plt.ylabel('current displacement east (meters)')

	tme = 0
	while True:
		east_actual = vehicle.location.local_frame.east 
		PV.append(east_actual)
		print "east actual:", east_actual 
		current_error = target_east - east_actual 
		error.append(current_error)
		
		u_t = controller_pid(error, delta_T) 
		vehicle.channels.overrides['1'] = max(0.01, u_t) 
		print "u(t): ", u_t
		print "Roll: ", vehicle.channels.overrides['1']
		print "Current loc: ", vehicle.location.local_frame	

		x = np.linspace(0, delta_T *len(error), len(error))
		ln.set_xdata(x)
		ln.set_ydata(PV)
		plt.scatter(x, PV)
		plt.draw()	
		plt.pause(delta_T)
		tme += 1

		#if tme > time_to_break:
		#	break
	
special_function(target_east=20.0, time_to_break=100)
#special_function(target_alt=50.0, time_to_break=100)
#special_function(target_alt=30.0, time_to_break=100)

#for wp in waypoints:
#	distance = float('inf') 
#	# Update loop
#	while distance > 0.0:
#		print "Distance: ", calculateDistance2D(vehicle.location.local_frame, wp)	
#		print "Current loc: ", vehicle.location.local_frame
#		vehicle.channels.overrides['2'] = 1400
#		time.sleep(0.5)


while True:
	print "Local location: ", vehicle.location.local_frame
	time.sleep(1)
