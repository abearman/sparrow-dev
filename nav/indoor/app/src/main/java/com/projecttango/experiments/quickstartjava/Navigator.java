/*
 * Date: 03/03/2016
 * Author: Andrew Lim
 */

package com.projecttango.experiments.quickstartjava;

import com.google.atap.tangoservice.TangoPoseData;

public class Navigator {
    private Path path;
    private FlightController flightController;
    private DroneConfig;

    public Navigator(FlightController flightController) {
	this.flightController = flightController;
    }

    public Navigator(FlightController flightController,
		     Path path) {
	this.flightController(flightController);
	this.path = path;
    }
    
    
};