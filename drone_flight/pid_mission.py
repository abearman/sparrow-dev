# Import DroneKit-Python
from dronekit import connect, VehicleMode, LocationGlobal, LocationGlobalRelative, LocationLocal, Command
from pymavlink import mavutil
import time
import math
import inspect
import numpy as np
import matplotlib.pyplot as plt
import pylab
from dronekit_sitl import SITL
import signal
import cv2

# 1 = Roll
# 2 = Pitch
# 3 = Throttle
# 4 = Yaw

# LocalLocation: (North, East, Down). In meters. Down is negative
# Attitude: (Pitch, Yaw, Roll). In radians.

PITCH = "pitch"
ROLL = "roll"
THROTTLE = "throttle"
NORTH = "north"
EAST = "east"
DOWN = "down"
DELTA_T = 0.05	# seconds (20 Hz)

vehicle = None

K_p = {PITCH: -15.0,
			 ROLL: 17.0,
			 THROTTLE: 15.0} 

K_i = {PITCH: 0.0,
			 ROLL: 0.0,
			 THROTTLE: 0.1}

K_d = {PITCH: -1.0,
			 ROLL: 7.0,
			 THROTTLE: 3.6}

bias = {PITCH: 1536.0,
				ROLL: 1537.0,
				THROTTLE: 1400.0}



def handler():
	global vehicle
	print "Emergency land, clearing RC channels"
	if '1' in vehicle.channels.overrides:
		vehicle.channels.overrides['1'] = 0
	if '2' in vehicle.channels.overrides:
		vehicle.channels.overrides['2'] = 0
	if '3' in vehicle.channels.overrides:	
		vehicle.channels.overrides['3'] = 0
	if '4' in vehicle.channels.overrides:
		vehicle.channels.overrides['4'] = 0
	print "Landing"
	vehicle.mode = VehicleMode("LAND")
	while (-1*vehicle.location.local_frame.down) > 0.0:
		print vehicle.location.local_frame 
		time.sleep(1)
	exit()


def connect_to_vehicle(is_simulator=True):
	global vehicle
	target = None

	if is_simulator:
		sitl = SITL()
		sitl.download('solo', '1.2.0', verbose=True)
		sitl_args = ['-I0', '--model', 'quad', '--home=-35.363261,149.165230,584,353']
		sitl.launch(sitl_args, await_ready=True, restart=True)
		target = "tcp:127.0.0.1:5760"
	else:
		target = "udpin:0.0.0.0:14550"
	
	print "Connecting to vehicle on: ", target, "..."
	vehicle = connect(target, wait_ready=True)
	print "Connected to vehicle"


def arm_vehicle(mode):
	global vehicle
	while not vehicle.is_armable:
		print " Waiting for vehicle to initialise..."
		time.sleep(1)

	vehicle.mode = VehicleMode(mode)
	vehicle.armed = True

	while not vehicle.armed:
		print " Waiting for arming..."
		time.sleep(1)
	print "Armed!"


def takeoff(target_down):
	global vehicle
	print "Taking off!"
	error = []
	PV = []
	down_actual = -1 * vehicle.location.local_frame.down
	
	while abs(down_actual - target_down) > 0.1:
		imgfile = cv2.imread("img.jpg")
		cv2.imshow("Img", imgfile)
		key = cv2.waitKey(1) & 0xFF
					
		down_actual = -1 * vehicle.location.local_frame.down
		PV.append(down_actual)
		error.append(target_down - down_actual)
		u_down = controller_pid(error, THROTTLE)
		print "u down: ", u_down
		vehicle.channels.overrides['3'] = u_down
		time.sleep(DELTA_T)
		print "Current loc: ", vehicle.location.local_frame

		# if the 'q' key is pressed, stop the loop
		if key == ord("q"):
			print "Pressed q"
			handler()
	cv2.destroyAllWindows()


# targets measured in meters
def adjust_channels(target_north, target_east, target_down):
	global vehicle
	error = {NORTH: [], EAST: [], DOWN: []}  # Arrays of past errors for each process variable
	PV = {NORTH: [], EAST: [], DOWN: []}	# Arrays of past values for each process variable (i.e., altitudes)

	north_actual = vehicle.location.local_frame.north
	east_actual = vehicle.location.local_frame.east
	down_actual = -1 * vehicle.location.local_frame.down

	ln, = plt.plot(PV[DOWN])
	plt.ion()
	plt.show()
	plt.xlabel('time (seconds)')
	plt.ylabel('current altitude (meters)')

	while (abs(north_actual - target_north) > 0.1) or (abs(east_actual - target_east) > 0.1) or (abs(down_actual - target_down) > 0.1):
		imgfile = cv2.imread("img.jpg")
		cv2.imshow("Img", imgfile)
		key = cv2.waitKey(1) & 0xFF
					
		north_actual = vehicle.location.local_frame.north
		east_actual = vehicle.location.local_frame.east 
		down_actual = -1 * vehicle.location.local_frame.down

		PV[NORTH].append(north_actual)
		PV[EAST].append(east_actual)
		PV[DOWN].append(down_actual)

		error[NORTH].append(target_north - north_actual)
		error[EAST].append(target_east - east_actual)
		error[DOWN].append(target_down - down_actual)

		u_north = controller_pid(error[NORTH], PITCH)	
		u_east = controller_pid(error[EAST], ROLL)
		u_down = controller_pid(error[DOWN], THROTTLE)
		print "u down: ", u_down

		vehicle.channels.overrides['1'] = max(0.01, u_east)	# roll
		vehicle.channels.overrides['2'] = max(0.01, u_north)  # pitch
		vehicle.channels.overrides['3'] = max(0.01, u_down)  # throttle

		x = np.linspace(0, DELTA_T*len(error[DOWN]), len(error[DOWN]))
		ln.set_xdata(x)
		ln.set_ydata(PV[DOWN])
		plt.scatter(x, PV[DOWN])
		plt.draw()		 
		plt.pause(DELTA_T)

		time.sleep(DELTA_T)
		print "Current loc: ", vehicle.location.local_frame	

		# if the 'q' key is pressed, stop the loop
		if key == ord("q"):
			print "Pressed q"
			handler()
	cv2.destroyAllWindows()

def controller_pid(error, channel):
	"""Determines the next control variable (roll, pitch, or throttle) using a PID controller.
		Args:
				error ([double]): An array of all past errors.
				channel (String): The channel we want to control (e.g. roll)
		Yields:
			double: The next control variable, in pwm
	"""
	current_error = error[-1]
	previous_error = error[-2] if len(error) > 1 else 0
	P = K_p[channel] * current_error
	I = K_i[channel] * DELTA_T * sum(error)
	D = K_d[channel] * (current_error - previous_error) / DELTA_T
	return P + I + D + bias[channel]



def main():
	waypoints = [(0.0, 0.0, 20.0), (0.0, 10.0, 15.0)]
	connect_to_vehicle(is_simulator=True)
	arm_vehicle("STABILIZE")
	takeoff(10.0)

	for wp in waypoints:
		print "Switching waypoint"
		adjust_channels(*wp)	
	

if __name__ == "__main__": main()
