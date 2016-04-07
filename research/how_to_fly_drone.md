# How to fly the 3DR Solo
This article explains how to fly the 3DR Solo from the controller. It also explains how channel overrides work.

![Controller](http://www.halo-robotics.com/wp-content/uploads/2015/12/3dr-solo-controller.png)

### Table of Contents
1. [Flight Modes](#flight-modes)
2. [Guided Takeoff](#guided-takeoff)
3. [Manual Takeoff](#manual-takeoff)
4. [Landing](#landing)
5. [Stick Controls](#stick-controls)
6. [Panic Buttons](#panic-buttons)
7. [Channel Overrides](#channel-overrides)

## Flight Modes
You can set the flight mode either through the remote controller, or through a DroneKit script.

#### Setting the flight mode through the remote controller
To set the flight mode through the remote controller, from the app home screen, tap Settings and then choose Advanced Settings. Toggle the Enable Advanced Flight Modes option to gain access to Solo’s advanced modes. To access an advanced flight mode, assign one of the five available advanced modes to the controller’s A or B button in the Solo settings section.

#### Setting the flight mode through DroneKit

```
vehicle.mode = VehicleMode(<arducopter_mode>)
```

See the list of flight modes below. When setting a flight mode in DroneKit, use the ***ArduCopter*** name for the mode, not the Solo name.

### Flight modes that require GPS:
Flight modes that use GPS-positioning data require an active GPS lock prior to takeoff. 

#### GUIDED (GUIDED in ArduCopter)
Guided mode is a capability of Copter to dynamically guide the copter to a target location wirelessly using a telemetry radio module and ground station application. This page provides instructions for using guided mode.

#### Fly:Manual (ALT_HOLD in ArduCopter) 
Fly:Manual mode is a version of standard flight without GPS lock. In Fly:Manual, the throttle stick controls altitude the same way as standard flight (Fly mode). However, Fly:Manual includes no GPS positioning so that, when you release the right stick, Solo will not hold its position; it will drift according to wind conditions and existing momentum. When flying in Fly:Manual, make constant adjustment to the right stick to control Solo’s position and use the left stick to maintain Solo’s orientation.



### Flight modes that do not require GPS:



## Guided Takeoff
Once Solo and the controller are both powered on, and a GPS connection has been established (**NOTE: the takeoff button requires a GPS connection**), the controller will prompt you to hold the Fly button to start Solo’s motors. Hold Fly to activate the motors.

<img src="https://3dr.com/wp-content/uploads/2015/05/fly_screen-300x233.png" width="300" />

With the propellers now spinning, Solo is ready for takeoff. Hold Fly once again, and Solo will ascend to 10 ft. (3 m) and hover in place.

<img src="https://3dr.com/wp-content/uploads/2015/05/start_takeoff-600x444.png" width="300" />


## Manual Takeoff
You should only be doing this from a DroneKit script, and we do this using a PID control loop to override the remote control (RC) channels. See the [Channel Overrides](#channel-overrides) section for more information. 


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


## Panic Buttons
If you experience a problem during one of your flights, the following procedures can help you avoid trouble. Note: we have discovered that these buttons can behave unpredictably during controller channel overrides. 

#### Pause Button
The Pause button freezes Solo mid-air like an emergency air brake. This can be useful to stop Solo from hitting an obstacle or to re-orient Solo for navigation. Pressing Pause during Return Home or Land will also freeze Solo during these maneuvers.

<img src="https://3dr.com/wp-content/uploads/2015/05/conrtoller_pause.jpg" width="300" />

#### Return to Home
The Return Home button allows you to bring your flight to an end by recalling Solo to its launch location and landing. Keep in mind that Return Home requires enough battery to get back to the launch point, so if you need to end your flight immediately then land manually or hold the Fly button. Return Home requires GPS lock prior to takeoff. DO NOT use this button during non-GPS modes.

<img src="https://3dr.com/wp-content/uploads/2015/05/fly_home-600x262.jpg" width="300" />

Solo will not avoid obstacles during its journey home, so make sure the return path is clear before activating Return Home.

#### Land
If you need to immediately, hold the Fly button to land Solo at its current position.

<img src="https://3dr.com/wp-content/uploads/2015/05/fly_button.jpg" width="300" />

#### Motor Shutoff
In the event that Solo’s motors do not stop after landing or for an emergency in-flight kill switch, Solo includes an emergency motor shutoff procedure. To shut off the motors at any time, either in flight or on the ground, hold the A, B, and Pause buttons at the same time. An initial screen will appear on the controller to confirm the shutoff command.

<img src="https://3dr.com/wp-content/uploads/2015/05/shut_off-600x444.png" width="300" />

Continue to hold A, B, and Pause buttons to activate motor shutoff. If activated during flight, Solo will crash, so use this procedure only in case of emergency.


 
## Channel Overrides
 

