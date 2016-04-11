/*
 * Date: 03/05/2015
 * Author: Pavitra Rengarajan
 */

package com.projecttango.experiments.quickstartjava;

public class DroneConfig {
    private double speed;
    private double intervalDistance;

    // create a new object with the given components
    public DroneConfig(double speed, double intervalDistance) {
        this.speed = speed;
        this.intervalDistance = intervalDistance;
    }

    // speed accessor
    public double getSpeed() {
        return this.speed;
    }

    // speed mutator
    public void setSpeed(double speed) {
        this.speed = speed;
    }

    // speed accessor
    public double getIntervalDistance() {
        return this.intervalDistance;
    }

    // speed mutator
    public void setIntervalDistance(double intervalDistance) {
        this.intervalDistance = intervalDistance;
    }

}
