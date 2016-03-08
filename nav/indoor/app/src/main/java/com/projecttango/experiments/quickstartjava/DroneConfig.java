/*
 * Date: 03/05/2015
 * Author: Pavitra Rengarajan
 */

package com.projecttango.experiments.quickstartjava;

public class DroneConfig {
    private double speed; 

    // create a new object with the given components
    public DroneConfig(double speed) {
        this.speed = speed;
    }

    // speed accessor
    public double getSpeed() {
        return this.speed;
    }

    // speed mutator
    public void setSpeed(double speed) {
        this.speed = speed;
    }

}
