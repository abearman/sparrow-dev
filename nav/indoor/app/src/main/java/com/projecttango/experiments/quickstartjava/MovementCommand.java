/**
 * Date: 03/06/2016
 * Author: Pavitra Rengarajan
 */

package com.projecttango.experiments.quickstartjava;

public class MovementCommand {

    // command encapsulates velocity_x, velocity_y, velocity_z, duration
    // these parameters can be used by send_ned_velocity
    private double velocity_x;
    private double velocity_y;
    private double velocity_z;
    private double duration;

    // TODO: incorporate orientation/quaternion rotations?

    public MovementCommand(double velocity_x, double velocity_y, double velocity_z, double duration) {
        this.velocity_x = velocity_x;
        this.velocity_y = velocity_y;
        this.velocity_z = velocity_z;
        this.duration = duration;
    }

    // Getters
    public double getVelocityX(){ return this.velocity_x; }
    public double getVelocityY(){ return this.velocity_y; }
    public double getVelocityZ(){ return this.velocity_z; }
    public double getDuration(){ return this.duration; }

    public String toString(){
        return "velocity_x: " + this.velocity_x + " velocity_y: " + this.velocity_y +
                " velocity_z: " + this.velocity_z + " duration: " + this.duration;
    }


}
