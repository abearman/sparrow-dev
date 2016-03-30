# SITL ArduCopter Simulator 

#### Table of Contents
1. [Setting up SITL using Vagrant](#setting-up-sitl-using-vagrant)
2. [Copter SITL and MAVProxy Tutorial](#copter-sitl-and-mavproxy-tutorial)

## Setting up SITL using Vagrant
How to use SITL (Software In The Loop) on Mac, using Vagrant.
This document is a summary of the information we've collected on how to run a Copter simulator through the terminal. For more information, see the links below.
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

	```bash
	vagrant up
	```

> Note: The first time you run the <code>vagrant up</code> command it will take some time to complete.	

### Start running SITL
Enter the following in your vagrant shell to run the 3DR Solo simulator. This will first build the code (if it has not previously been built) and then run the simulator:

```bash
vagrant ssh -c "sim_vehicle.sh -j 2"
```

Once the simulation is running, you will start getting information about the vehicle state. For example:

```
GPS lock at 0 meters
APM: PreArm: RC not calibrated
APM: Copter V3.3-dev (999710d0)
APM: Frame: QUAD
APM: PreArm: RC not calibrated
```

### Run MAVProxy in your main OS
You can now connect to the running simulator from your main OS. Just connect to UDP port 14550. The **MAVProxy** command is:

```bash
mavproxy.py --master=127.0.0.1:14550
```

### Shutting down the simulator
When you are done with the simulator:
* Press **ctrl-d** in the Vagrant SSH window to exit **MAVProxy**.
* Suspend the running VM by entering the following in the command prompt:

```bash
vagrant suspend
```

### Restarting the simulator
When you need the simulator again, you can resume the VM and restart the simulator as shown below. This only takes a few seconds.

```bash
vagrant up
vagrant ssh -c "sim_vehicle.sh -j 2"
``` 

## Copter SITL and MAVProxy Tutorial

