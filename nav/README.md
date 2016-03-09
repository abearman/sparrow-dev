# Sparrow Dev: Navigation

This README is meant to provide additional information on the navigation code contained in this folder.

**DroneConfig.java** - Used for configuration of drone settings, such as desired drone speed as it navigates through a warehouse.

**FlightController.java** - Encapsulation around ardupilot, used to interface the Java navigation code with the C++ ardupilot code.

**MainActivity.java** - The main entry point into the Android tablet application which gathers the coordinates/movement commands and sends them to the server.

**MovementCommand.java** - Encapsulates the normalized velocity vector and duration, which are the movement commands that we would send to the drone. 

**Navigator.java** - Handles Tango data manipulation, navigation, and collision avoidance. Also contains logic for transformation from basic xyz-coordinates to movement commands, based on transformations between the drone's current location and its nearest path point.

**Path.java** - A list of PoseData points, which is entered by the user for the drone to fly along.

**PoseData.java** - The most atomic element of the navigation code, a wrapper around the Tango PoseData API such that we only deal with translation and rotation components.

**Quaternion.java** - The Tango API represents its rotation information using quaternions. This class contains quaternion transformations to be able to transform from one orientation to another, and utility functions.

**SocketConnector.java** - Used to interface the Android application with the server via sockets.

**Utils.java** - Miscellaneous utility functions that are used by the navigator.
