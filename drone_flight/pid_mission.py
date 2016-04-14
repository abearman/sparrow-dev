# Import DroneKit-Python
from dronekit import connect, VehicleMode, LocationGlobal, LocationGlobalRelative, LocationLocal, Command
from pymavlink import mavutil
import time
import math
import inspect
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pylab
from dronekit_sitl import SITL
import cv2
import sys

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

# Mapping from the direction we want to travel (NORTH, EAST, or DOWN), to the joystick we'll need to control (PITCH, ROLL, or THROTTLE)
sticks_mapping = {NORTH: PITCH, EAST: ROLL, DOWN: THROTTLE}

# Mapping from the direction we want to travel (NORTH, EAST, or DOWN), to the RC channel we'll need to control ('1', '2', or '3')
channels_mapping = {NORTH: '2', EAST: '1', DOWN: '3'}

DELTA_T = 0.05	# seconds (20 Hz)

vehicle = None
ax = None
ax = None
do_graph_3d_position = False 
do_graph_2d_position = False
use_tango_location = False

simulator_hyperparams = {
	"K_p": {PITCH: -15.0,
						 ROLL: 17.0,
						THROTTLE: 20.0}, 

	"K_i": {PITCH: 0.0,
						 ROLL: 0.0,
						 THROTTLE: 0.1},

	"K_d": {PITCH: -1.0,
						 ROLL: 7.0,
						 THROTTLE: 3.6},

	"bias": {PITCH: 1536.0,
							 ROLL: 1537.0,
							 THROTTLE: 1500.0}
}

# Drone without Tango
K_p = {PITCH: -15.0,
					ROLL: 17.0,
					THROTTLE: 15.0} 

K_i = {PITCH: 0.0,
					ROLL: 0.0,
					THROTTLE: 0.0}

K_d = {PITCH: -1.0,
					ROLL: 7.0,
					THROTTLE: 0.0}

bias = {PITCH: 1536.0,
					 ROLL: 1537.0,
					 THROTTLE: 1404.0}

def emergency_land():
	"""Emergency land function that clears all RC channel overrides, lands the drone, and exits."""
	global vehicle
	print "Emergency land, clearing RC channels"
	# Clear all overrides by setting an empty dictionary
	vehicle.channels.overrides = {}
	print "Landing"
	vehicle.mode = VehicleMode("LAND")
	while (vehicle.location.global_relative_frame.alt) > 0.0:
		print vehicle.location.global_relative_frame.alt	
		time.sleep(1)
	exit()


def connect_to_server():
	sys.path.append('../server')
	print "Connecting to server ..."
	from drone_utility import get_tango_location
	print "Connected to server"


def connect_to_vehicle(is_simulator=True):
	"""Connects to either the real drone or the simulator.
		Args:
			is_simulator (bool): If true, connect to the simulator. Otherwise, connect to the real drone.
	"""
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
	"""Arms the vehicle in the selected mode.
		Args:
			mode (String): The desired flight mode for the vehicle.
	"""
	global vehicle
	vehicle.mode = VehicleMode(mode)
	print "Flight mode: ", vehicle.mode

	# Lower throttle before takeoff is required in STABILIZE mode
	if mode == "STABILIZE":
		vehicle.channels.overrides['3'] = 1000
		#vehicle.parameters['ARMING_CHECK'] = -5	# Skip compass

	while not vehicle.is_armable:
		print " Waiting for vehicle to initialise..."
		time.sleep(1)

	vehicle.armed = True

	while not vehicle.armed:
		print " Waiting for arming..."
		time.sleep(1)
	print "Armed!"


def init_3d_graph():
	"""Initializes a 3D plot for the drone position (North, East, Down)."""
	global ax
	plt.ion()
	plt.show()
	fig = plt.figure()
	ax = fig.add_subplot(111, projection='3d')
	ax.set_xlabel('east')
	ax.set_ylabel('north')
	ax.set_zlabel('down')


def init_2d_graph(ylabel):
	"""Initializes a 2D plot for the drone's position (north, east, or down) over time."""
	global ln
	ln, = plt.plot([])
	plt.ion()
	plt.show()
	plt.xlabel('time (seconds)')
	plt.ylabel(ylabel + ' (meters)')


def takeoff(target_alt, loiter=False):
	"""Takes off the drone using a PID controller.
		Args:
			target_alt (double): The desired altitude (in meters) for takeoff  
	"""
	print "Taking off!"
	move_one_direction(target_alt, DOWN, loiter)

def land():
	"""Lands the drone using a PID controller."""
	print "Landing the drone"
	move_one_direction(0.0, DOWN)


def move_one_direction(target_displacement, direction, loiter=False):
	"""Uses a PID controller to move the drone in ONE direction. Useful for taking off, landing, and parameter tuning.
		Args:
			target_displacment (double):
			direction (String): Represents the direction in which displacement is to occur. One of a list of constants (NORTH, EAST or DOWN)
			loiter (boolean): Whether or not to remain at the target displacement location indefinitely, or move onto a new waypoint.
	"""
	global vehicle

	if do_graph_2d_position:
		init_2d_graph("displacement " + direction)

	error = {NORTH: [], EAST: [], DOWN: []}  # Arrays of past errors for each process variable
	PV = {NORTH: [], EAST: [], DOWN: []}	# Arrays of past values for each process variable (i.e., altitudes)

	displacement_actual = 0.0
	if direction == NORTH:
		displacement_actual = vehicle.location.local_frame.north
	elif direction == EAST:
		displacement_actual = vehicle.location.local_frame.east
	elif direction == DOWN:
		displacement_actual = vehicle.location.global_relative_frame.alt	

	while loiter or (abs(target_displacement - displacement_actual) > 0.1):
		imgfile = cv2.imread("img.jpg")
		cv2.imshow("Img", imgfile)
		key = cv2.waitKey(1) & 0xFF

		if direction == NORTH:
			displacement_actual = vehicle.location.local_frame.north
		elif direction == EAST:
			displacement_actual = vehicle.location.local_frame.east
		elif direction == DOWN:
			displacement_actual = vehicle.location.global_relative_frame.alt		

		PV[NORTH].append(vehicle.location.local_frame.north)
		PV[EAST].append(vehicle.location.local_frame.east)
		PV[DOWN].append(vehicle.location.global_relative_frame.alt)		

		error[direction].append(target_displacement - displacement_actual)
		u_t = controller_pid(error[direction], sticks_mapping[direction]) 
		print "u_t: ", u_t, " direction: ", direction 
		vehicle.channels.overrides[channels_mapping[direction]] = u_t

		if do_graph_3d_position or do_graph_2d_position:
			if do_graph_3d_position:
				global ax
				ax.scatter(PV[EAST], PV[NORTH], PV[DOWN])
			if do_graph_2d_position:
				global ln
				x = np.linspace(0, DELTA_T*len(error[direction]), len(error[direction]))
				ln.set_xdata(x)
				ln.set_ydata(PV[direction])
				plt.scatter(x, PV[direction])
			plt.draw()
			plt.pause(DELTA_T)

		time.sleep(DELTA_T)
		print "Current loc: ", vehicle.location.local_frame
		print "Global loc: ", vehicle.location.global_relative_frame

		# if the 'q' key is pressed, stop the loop
		if key == ord("q"):
			print "Pressed q"
			emergency_land()
	cv2.destroyAllWindows()

def move_to_waypoint(target_north, target_east, target_down):
	"""Uses a PID controller to navigate the drone to a provided waypoint.
		Args:
			target_north (double): The desired relative north position (in meters) for the drone
			target_east (double): The desired relative east position (in meters) for the drone
			target_down (double): The desired relative down position (in meters) for drone
	"""
	global vehicle
	global ax
	
	error = {NORTH: [], EAST: [], DOWN: []}  # Arrays of past errors for each process variable
	PV = {NORTH: [], EAST: [], DOWN: []}	# Arrays of past values for each process variable (i.e., altitudes)

	north_actual = vehicle.location.local_frame.north
	east_actual = vehicle.location.local_frame.east
	#down_actual = -1 * vehicle.location.local_frame.down
	down_actual = vehicle.location.global_relative_frame.alt 

	while (abs(north_actual - target_north) > 0.1) or (abs(east_actual - target_east) > 0.1) or (abs(down_actual - target_down) > 0.1):
		imgfile = cv2.imread("img.jpg")
		cv2.imshow("Img", imgfile)
		key = cv2.waitKey(1) & 0xFF
					
		north_actual = vehicle.location.local_frame.north
		east_actual = vehicle.location.local_frame.east 
		#down_actual = -1 * vehicle.location.local_frame.down
		down_actual = vehicle.location.global_relative_frame.alt 

		PV[NORTH].append(north_actual)
		PV[EAST].append(east_actual)
		PV[DOWN].append(down_actual)

		error[NORTH].append(target_north - north_actual)
		error[EAST].append(target_east - east_actual)
		error[DOWN].append(target_down - down_actual)

		u_north = controller_pid(error[NORTH], sticks_mapping[NORTH])  # Corresponds to PITCH 
		u_east = controller_pid(error[EAST], sticks_mapping[EAST])
		u_down = controller_pid(error[DOWN], sticks_mapping[DOWN])
		print "u down: ", u_down

		vehicle.channels.overrides[channels_mapping[NORTH]] =  u_north	# pitch 
		vehicle.channels.overrides[channels_mapping[EAST]] = u_east  # roll
		vehicle.channels.overrides[channels_mapping[DOWN]] = u_down  # throttle

		if do_graph_3d_position or do_graph_2d_position:
			if do_graph_3d_position:
				global ax
				ax.scatter(PV[EAST], PV[NORTH], PV[DOWN])
			if do_graph_2d_position:
				global ln
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
			emergency_land()
	cv2.destroyAllWindows()


def controller_pid(error, channel):
	"""Determines the next control variable (e.g., roll, pitch, throttle) using a PID controller.
		Args:
			error ([double]): An array of all past errors.
			channel (String): The channel we want to control (e.g. roll). One of a list of constants (PITCH, ROLL, or THROTTLE)
		Yields:
			double: The next control variable, in pwm
	"""
	current_error = error[-1]
	previous_error = error[-2] if len(error) > 1 else 0

	print "K_p: ", K_p[channel]
	print "Error (target - actual): ", current_error

	P = K_p[channel] * current_error
	I = K_i[channel] * DELTA_T * sum(error)
	D = K_d[channel] * (current_error - previous_error) / DELTA_T

	print "P: ", P
	print "K_i: ", K_i[channel]
	print "K_d: ", K_d[channel]
	print "bias: ", bias[channel]

	return P + I + D + bias[channel]



def main():
	"""Main entry point for the PID mission script.

		To emergency land the drone and stop the script, click on the image that appears and press the "q" key. 

		Variables:
			use_simulator (bool): Whether or not to run the program on the simulator.
			do_graph_3d_position (bool): Whether or not to 3D plot the drone's position.
			do_graph_2d_position (bool): Whether or not to plot the drone's position along one direction, over time.
			use_tango_location (bool): Whether or not to use the Tango's localization coordinates (as opposed to those provided by the drone).
			mode (String): The flight mode for the vehicle (i.e., GUIDED, STABILIZE, ALT_HOLD)
			waypoints ([(double, double, double)]): A list of (North, East, Down) waypoints the drone should travel to, measured by displacements (in meters) from the origin point.
			takeoff_height (double): How high (in meters) the drone should take off to.
	"""
	global vehicle
	global do_graph_3d_position
	global do_graph_2d_position
	global use_tango_location

	try:
		use_simulator = True 
		do_graph_3d_position = False 
		do_graph_2d_position = False 
		use_tango_location = False 
		mode = "STABILIZE"
		takeoff_height = 8.0
		#waypoints = [(3.0, 0.0, 3.0)]
		waypoints = []

		if use_tango_location: connect_to_server()
		connect_to_vehicle(is_simulator=use_simulator)
		arm_vehicle(mode)
		if do_graph_3d_position: init_3d_graph()
		
		takeoff(takeoff_height, loiter=True)
		#move_one_direction(3.0, NORTH, loiter=True)

		for wp in waypoints:
			print "Switching waypoint"
			move_to_waypoint(*wp)	
		
		land()
	
	except:
		print "Closing vehicle before terminating"
		vehicle.close()
	finally:
		print "Closing vehicle before terminating"
		vehicle.close()


if __name__ == "__main__": main()
