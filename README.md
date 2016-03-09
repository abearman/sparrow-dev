# Sparrow Development

This repository tracks our development progress. Here is an overview of the directory structure to get oriented:

**barcode** and **zbar** - These directories contain code pertaining to barcode scanning. We are exploring the advantages and disadvantages of both barcode scanning and RFID reading in order to do inventory counting and missing item retrieval.

**droneMonitor** and **sparrowWeb** - These directories contain code pertaining to the front-end application. The user would input a path for the drone to travel on using the web app; this information would then be sent to the back-end to make the drone move this user-specified path.

**drone_flight** - This directory contains scripts that allow the drone to fly. We have experimented with flying the drone with GPS as well as without GPS.

**nav** - This directory contains code pertaining to navigation: Tango computation, 3D mapping, spatial recognition, collision avoidance, etc.

**server** - This directory contains server code that provides endpoints to facilitate the sending of data from the tango to the drone. The server would act as the main hub for communication between the various hardware devices at play; all devices would connect to the 3DR solo's wifi to communicate.
