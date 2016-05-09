# System Design

The system is comprised of three major components: the drone, server, and iOS client. We consider the Tango as an accessory to the system; it would only be considered a requirement for indoor flight, but for outdoor SAR flights, it isn't necessary. For outdoor flights, the Tango provides the video feed for the client. This, video feed, however could be attained from another source such as a GoPro camera. As such, our system is not tightly coupled to the Tango device.

### Drone:

The drone receives commands from the server that dictate what actions it takes. There is no critical logic on board the drone. This decision was made largely due to the limitations of the 3DR's SDK; the SDK provides a clean way of sending movement commands, but does not necessarily provide support for integrating external inputs. Since we wanted to build our system with the flexibility for future external sensor platforms, we opted to include much of the core mission logic on the server. A more robust/performant approach would probably have attempted to integrate external sources directly with the drone's board via serial ports. Our lack of familiarity with hardware integration, however, impacted our decision not to pursue this avenue.

Much of our recent work, however, has included looking at potentially modifying the drone's firmware to incorporate a new flight mode that is based off relative based GPS transformations. Using coordinate transforms and a reference GPS location, it is possible to use relative offsets to estimate GPS position changes from the original reference location. These transforms allow us to "spoof" GPS coordinates and rely on many of the existing routines in the ArduPilot system without having actual connections to GPS satellites.

### Server

The server handles much of the navigational logic and mission decision making. It is responsible for handling on-the-fly commands from the client but also for carrying out pre-defined missions. These missions include a variety of standard SAR paths that we have found to be commonly used by military, park service, and disaster relief operators. The server handles all the data communication between the system components. For example, it passes on user commands from the iOS client as flight commands to the drone. It all takes video feed input and forwards it along to the iOS client.

### iOS client
The iOS client is really the only aspect of the system that an operator would have to interact with. The UI has been designed with the SAR application in mind. In particular, we have incorporated certain SAR specific features such as pre-defined search paths, area covered mapping, and pin dropping to mark locations of interest.

### Tango
The Tango serves as our current video source. To gather video from the Tango, we must interpose into the rendering pipeline to snatch pixel buffers to send over to the server. This is not ideal and not the most performant option. The Tango does not provide the highest quality video but has been chosen since it allows us to have the flexibility to fly in potentially GPS denied applications. For example, the Tango gives us the ability to understand an environment without GPS reliability and even allows us to perform 3D mapping of a new space.

