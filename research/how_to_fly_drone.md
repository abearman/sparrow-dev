# How to fly the 3DR Solo
This article explains how to fly the 3DR Solo from the controller. It also explains how channel overrides work.

![Controller](http://www.halo-robotics.com/wp-content/uploads/2015/12/3dr-solo-controller.png)

### Table of Contents
1. [Flight Modes](#flight-modes)
2. [Guided Takeoff](#guided-takeoff)
3. [Manual Takeoff](#manual-takeoff)
4. [Landing](#landing)
5. [Stick Controls](#stick-controls)
6. [Emergency Commands](#emergency-commands)
7. [Channel Overrides](#channel-overrides)

## Flight Modes


## Guided Takeoff
Once Solo and the controller are both powered on, and a GPS connection has been established (**NOTE: the takeoff button requires a GPS connection**), the controller will prompt you to hold the Fly button to start Solo’s motors. Hold Fly to activate the motors.

<img src="https://3dr.com/wp-content/uploads/2015/05/fly_screen-300x233.png" width="300" />

With the propellers now spinning, Solo is ready for takeoff. Hold Fly once again, and Solo will ascend to 10 ft. (3 m) and hover in place.

<img src="https://3dr.com/wp-content/uploads/2015/05/start_takeoff-600x444.png" width="300" />


## Manual Takeoff
You should only be doing this from a drone-kit script, and we do this using a PID control loop to override the remote control (RC) channels. See the [Channel Overrides](#channel-overrides) section for more information. 


## Landing
When you decide to end your flight, simply hold the Fly button to land Solo at its current location. Landing will work without GPS.

<img src="https://3dr.com/wp-content/uploads/2015/05/land_start-300x222.png" width="300" />

Note: When commanded to land, Solo will land at the current location, wherever it is. Always make sure there is a clear path to a safe landing point directly below Solo before landing.


## Stick Controls
The controller’s two joysticks allow you to navigate Solo in flight.
 
#### Left Joystick
The left stick controls Solo’s altitude and rotation.

![left joystick](https://3dr.com/wp-content/uploads/2015/05/left_stick-300x221.png)

Move vertically to control Solo’s altitude and acceleration.               

<img src="https://3dr.com/wp-content/uploads/2015/05/left_motions-1024x835.png" width="400" />


Move horizontally to rotate Solo and control orientation.

<img src="https://3dr.com/wp-content/uploads/2015/05/left_yaw-1024x695.png" width="400" />

#### Right Joystick
Use the right stick to fly Solo forward, back, left, and right. These movements are relative to Solo’s current orientation, so always maintain awareness of Solo’s forward-facing direction before using right-stick controls.

![right joystick](https://3dr.com/wp-content/uploads/2015/05/right_stick-300x208.png)

Move vertically to control pitch.

<img src="https://3dr.com/wp-content/uploads/2015/05/right_pitch-1024x615.png" width="400" />


Move the right stick horizontally to control roll.

<img src="https://3dr.com/wp-content/uploads/2015/05/right_roll-1024x550.png" width="400" />


## Emergency Commands


## Channel Overrides
 

