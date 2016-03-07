/*
 * Date: 03/03/2016
 * Author: Andrew Lim
 */

package com.projecttango.experiments.quickstartjava;

import com.google.atap.tangoservice.TangoPoseData;

import java.util.ArrayList;

public class Navigator {
    private Path path;
    private FlightController flightController;
    private DroneConfig droneConfig;
    private ArrayList<MovementCommand> mvmtCommands;

    public Navigator(FlightController flightController) {
	    this.flightController = flightController;
    }

    public Navigator(FlightController flightController,
		     Path path) {
	    this(flightController);
	    this.path = path;
    }

    // Getters
    public ArrayList<MovementCommand> getMvmtCommand() { return this.mvmtCommands; }

    // Issue commands to go from p1 to p2
    // Currently performs this using just one command, according to DroneConfig's speed setting
    public MovementCommand issueCommands(PoseData p1, PoseData p2) {

        ArrayList<MovementCommand> commands = new ArrayList<MovementCommand>();

        // only consider translation component for now
        // TODO: also consider rotation
        double[] vec1 = p1.getTranslation();
        double[] vec2 = p2.getTranslation();
        double[] diffTranslation = Utils.diff(vec1, vec2);

        // normalize xyz-vector according to desired velocity (as per DroneConfig)
        double[] normalizedVelocityVec = Utils.normalize(diffTranslation, droneConfig.getSpeed());
        double duration = Utils.getDist(vec1, vec2) / droneConfig.getSpeed();

        // create movement commands
        double velocity_x = normalizedVelocityVec[0];
        double velocity_y = normalizedVelocityVec[1];
        double velocity_z = normalizedVelocityVec[2];

        // find the MovementCommand and add to the navigator's running list of commands
        MovementCommand cmd = new MovementCommand(velocity_x, velocity_y, velocity_z, duration);
        this.mvmtCommands.add(cmd);

        return cmd;
    }
    
};