## SITL ArduCopter Simulator 
How to use SITL (Software In The Loop) on Mac, using Vagrant.
This document is a summary of the information we've collected on how to run a Copter simulator through the terminal. For more information, see the links below.

#### Relevant links:
* [Setting up SITL on Linux](http://ardupilot.org/dev/docs/setting-up-sitl-on-linux.html)
* [Setting up SITL using Vagrant](http://ardupilot.org/dev/docs/setting-up-sitl-using-vagrant.html)

### Overview
The SITL simulator allows you to run Plane, Copter, or Rover without any hardware. We will be focusing on the 3DR Solo simulator.
 
![SITL](http://ardupilot.org/dev/_images/SITL_Linux.jpg)

SITL runs natively on Linux and Windows. To run SITL on Mac, we need to set up SITL in a virtual machine environment using [Vagrant](https://www.vagrantup.com/), and connect it to a Ground Control Station running on the host computer. Vagrant is a tool for automating setting up and configuring development environments running in virtual machines.  

Setting up SITL with Vagrant is much easier and faster than [manually](http://ardupilot.org/dev/docs/setting-up-sitl-on-windows.html#setting-up-sitl-on-windows) setting up a virtual machine to run SITL on Mac OSX or Windows (or Linux).

### Set up Vagrant and the virtual machine
1. [Download and install VirtualBox](https://www.virtualbox.org/wiki/Downloads)
2. [Download and install Vagrant](https://www.vagrantup.com/downloads.html) for your platform (Windows, OSX, and Linux are supported).
3. The ArduPilot Github repository is already cloned in the main directory of the **dev** branch (don't need to do anything for this step).
4. Start a vagrant instance
	* Open a command prompt and navigate to the directory ``sparrow-dev/ardupilot/ardupilot-solo/Tools/vagrant/``
	* Run the command: 

	```
	vagrant up
	```

Note: the first time you run the ``vagrant up`` command it will take some time to complete. 
