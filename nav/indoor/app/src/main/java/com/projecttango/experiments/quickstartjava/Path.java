/*
 * Date: 03/03/2016
 * Author: Andrew Lim
 */

package com.projecttango.experiments.quickstartjava;

import java.util.ArrayList;

public class Path {
    private List<PoseData> poseList;

    public Path() {
	poseList = new ArrayList<PoseData>();
    }

    void addPose(PoseData pose) {

    }



    public Navigator(FlightController flightController) {
	this.flightController = flightController;
    }

    public Navigator(FlightController flightController,
		     Path path) {
	this.flightController(flightController);
	this.path = path;
    }

    

    
    
};