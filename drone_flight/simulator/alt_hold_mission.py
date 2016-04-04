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



def calculateDistance2D(location1, location2):
	dist = math.sqrt((location1.east - location2.east)**2 + (location1.north - location2.north)**2)
	return dist


# delta_T is in seconds
def controller_pid(e, delta_T):
	K_p = 0.05
	K_i = 0
	K_d = 0
	p = K_p * e[len(e) - 1]
	i = K_i * delta_T * sum(e)
	d = K_d * (e[len(e)-1] - e[len(e)-2]) / delta_T
	return p + i + d


# Connect to the Vehicle.
print "Connecting to vehicle on: 'tcp:127.0.0.1:5760'"
vehicle = connect('tcp:127.0.0.1:5760', wait_ready=True)
print "Connected to vehicle"

waypoints = [LocationLocal(10, 0, -10.0)]

while not vehicle.is_armable:
        print " Waiting for vehicle to initialise..."
        time.sleep(1)

vehicle.mode = VehicleMode("GUIDED")
vehicle.armed = True
while not vehicle.armed:
	print " Waiting for arming..."
	time.sleep(1)

#vehicle.channels.overrides['3'] = 1800
vehicle.simple_takeoff(10)

# Ascend
while (vehicle.location.local_frame.down is None) or ((-1 * vehicle.location.local_frame.down) < 10.0):
	time.sleep(1)
	print "Still ascending"
	print "Local location: ", vehicle.location.local_frame
#vehicle.channels.overrides['3'] = 1500
print "Finished ascending"

vehicle.mode = VehicleMode("ALT_HOLD")
vehicle.groundspeed = 7.5

#cmd = Command(0,0,0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT,
#				  mavutil.mavlink.MAV_CMD_DO_CHANGE_SPEED,
#				  0, 0, # Not supported 
#				  1, # Param 1: Ground speed
#				  1, # Param 2: Speed (m/s)
#				  -1, # Param 3: Throttle
#				  0, 0, 0, 0)
#cmds = vehicle.commands
#cmds.clear()
#cmds.add(cmd)
#cmds.upload()
#print "Sent command"					

while True:
	print vehicle.location.global_relative_frame
	time.sleep(1)

k = 1500
K_inv = np.matrix([[k, k, k, k], [-k, k, -k, k], [-k, -k, k, k], [k, -k, -k, k]]) 
delta_T = 0.05	# seconds
e = []

ln, = plt.plot(e)
plt.ion()
plt.show()
plt.xlabel('time (seconds)')
plt.ylabel('altitude error (meters)')

#while True:
#	H_T = 20.0	# target height
#	H_M = -1 * vehicle.location.local_frame.down	# measured height
#	e_H = H_T - H_M
#	e.append(e_H)
#	F = controller_pid(e, delta_T) 
#	print "F: ", F
#	vehicle.channels.overrides['3'] += F
#	print "Thrust: ", vehicle.channels.overrides['3']
#	print "Current loc: ", vehicle.location.local_frame	

#	x = np.linspace(0, delta_T *len(e), len(e))
#	ln.set_xdata(x)
#	ln.set_ydata(e)
#	plt.scatter(x, e)
#	plt.draw()	
#	plt.pause(delta_T)

#	second_product = np.matrix([[0], [0], [0], [F]])
#	ws = np.matrix.dot(K_inv, second_product)		
#	print "ws: ", ws
#	#time.sleep(delta_T)


#for wp in waypoints:
#	distance = float('inf') 
#	# Update loop
#	while distance > 0.0:
#		print "Distance: ", calculateDistance2D(vehicle.location.local_frame, wp)	
#		print "Current loc: ", vehicle.location.local_frame
#		vehicle.channels.overrides['2'] = 1400
#		time.sleep(0.5)


#while True:
	#print "Local location: ", vehicle.location.local_frame
	#print "Down: ", vehicle.location.local_frame.down
#	time.sleep(1)
