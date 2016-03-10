# Sparrow Development

Welcome to the dev branch of the sparrow-dev repository! This repository tracks our development progress. We're excited by the progress we've made and are looking forward to making more progress over the next several weeks!  

### Onboarding

There are four main components to our development: application, tango, drone, and server. The application deals with everything user-facing, from having the user input a drone path on a 3D map to visualizing the drone path in real-time as it follows the path. The Google Tango device is used for the bulk of the navigation logic, collision avoidance, motion tracking, 3D mapping, and spatial understanding. The drone is jerryrigged to the tango and serves as a vehicle to autonomously move around a space. We have built a server to handle communication between the different components; most significantly, the tango would send movement commands to the drone based on its vision algorithms and computations. The tango, drone, and server will all run on the drone controller's wifi. Here is a graphic depicting this design:



### Directory Structure

Here is an overview of the directory structure to get oriented:

**barcode** and **zbar** - These directories contain code pertaining to barcode scanning. We are exploring the advantages and disadvantages of both barcode scanning and RFID reading in order to do inventory counting and missing item retrieval.

**droneMonitor** and **sparrowWeb** - These directories contain code pertaining to the front-end application. The user would input a path for the drone to travel on using the web app; this information would then be sent to the back-end to make the drone move this user-specified path.

**drone_flight** - This directory contains scripts that allow the drone to fly. We have experimented with flying the drone with GPS as well as without GPS.

**nav** - This directory contains code pertaining to navigation: Tango computation, 3D mapping, spatial recognition, collision avoidance, etc.

**server** - This directory contains server code that provides endpoints to facilitate the sending of data from the tango to the drone. The server would act as the main hub for communication between the various hardware devices at play; all devices would connect to the 3DR solo's wifi to communicate.

### Workflow Protocol

We are using Git issues to track our progress. Each issue has 2 labels: one for priority level (P0 - Critical, P1 - Important, P2 - Medium, P3 - Low), and one for category (Application, Drone, Tango, Server). We will keep track of a running list of issues and work on issues in order of priority. Everyone may create tasks as we engage in more discussions and continue in our development throughout the next few weeks, and assign the tasks to themselves or others appropriately.
