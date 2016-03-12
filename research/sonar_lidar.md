## Sonar/Lidar

###Sonar Overview

Sonar uses sound propagation to navigate and detect objects. "Passive sonar" refers to listening to the sound made by objects, while "active sonar" emits pulses of sounds and listens for echoes. Sonar is often used as a means of acoustic location in air as well as for robot navigation. To understand sonar, it is helpful to think of the mental model of a bat; you can "see" based on the sound echoing back. The farther the distance, the longer it takes for the sound to come back.

###Specs

Sonar sensors are very cheap, often around $25 each. The types of sensors we would buy would probably be 6 grams (max 30-50) grams each, so with 4 sensors this means 24 grams total (max 120-200 grams total if we get the heaviest ones). The payload capacity of a 3DR solo is 450 grams beyond the gimbal and GoPro, so we should be well under the payload capacity. Several sensors have automatic calibration to compensate for changes in temperature, voltage, humidity, and noise. They often also have real-time auto or power-up calibration, and are powered with a battery. MaxSonar sensors specifically state that they can be used on UAVs.

###Sonar-Robot Integration

The microcontroller tells sonar to go, and the sonar emits a mostly inaudible sound (ultrasonic range finder sensor often emits a 40 kHz sound wave), time passes, then detects the return echo. The voltage signal is sent to the microcontroller, which by keeping track of the time that passes, can calculate the distance of the objects detected.

###Calculations of Distance vs. Time

The speed of sound in air is approximately 343 meters per second. The relationship between distance and time is: speed of sound[m/s] * time passed [s] = distance from object [m].

###eBumper4 by Panoptes

The eBumper4 is installed on a drone and helps avoid obstacles during flight. It uses 4 sonar sensors, on the left right, above, and front. When it senses an obstacle in the field of view, it "bumps" off the object, keeping the drone at a safe predetermined distance. The eBumper4 is self-centered, automatically centering the drone when flying between two objects. It is specifically used for indoor collision protection, with its sensors having a 40 degree field of view and 15 feet detection range. It weighs 130 grams and results in a reduction of 2-3 minutes of flight time. Currently, this technology only works for the Phantom DJI and Iris+ 3DR.

###ArduCopter Analog Sonar Support

The Copter supports MaxSonar line of sonar sensors for “Terrain Following” while in Loiter or Alt Hold modes to copter will attempt to maintain a constant distance from the ground. The sonar sensor should be connected to the appropriate pins, and the code allows you to display current distance sensed by the sonar (the numbers will change if you aim the sonar at different targets).

###DroneKit Compass/Magnetometer Support

There is a "heading" attribute in the "droneKit.Vehicle" class which provides the current heading in degrees from 0..360, where North = 0. In the ArduPilot source code, the compass information is used for performing yaw and drift correction. As long as the compass is initially calibrated properly (by turning the copter a bit), you should be able to use the compass to understand the drone's current position and be able to self-correct given a predetermined map.
