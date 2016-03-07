/*
 * Date: 03/05/2015
 * Author: Pavitra Rengarajan
 */

package com.projecttango.experiments.quickstartjava;

public class Quaternion {
    private final double x, y, z, w; 

    // constructor
    public Quaternion(double x, double y, double z, double w) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.w = w;
    }

    // return the quaternion norm
    public double norm() {
        return Math.sqrt(x*x + y*y +z*z + w*w);
    }

    // return the quaternion conjugate
    public Quaternion conjugate() {
        return new Quaternion(-x, -y, -z, w);
    }

    // return a new Quaternion whose value is (this + b)
    public Quaternion plus(Quaternion q) {
        return new Quaternion(this.x+q.x, this.y+q.y, this.z+q.z, this.w+q.w);
    }


    // return a new Quaternion whose value is (this * b)
    public Quaternion times(Quaternion q) {
        double newX = this.w*q.x + this.x*q.w + this.y*q.z - this.z*q.y;
        double newY = this.w*q.y - this.x*q.z + this.y*q.w + this.z*q.x;
        double newZ = this.w*q.z + this.x*q.y - this.y*q.x + this.z*q.w;
        double newW = this.w*q.w - this.x*q.x - this.y*q.y - this.z*q.z;
        return new Quaternion(newX, newY, newZ, newW);
    }

    // return a new Quaternion whose value is the inverse of this
    public Quaternion inverse() {
        double d = w*w + x*x + y*y + z*z;
        return new Quaternion(w/d, -x/d, -y/d, -z/d);
    }

    // return a / b
    // we use the definition a * b^-1 (as opposed to b^-1 a)
    public Quaternion divides(Quaternion q) {
        return this.times(q.inverse());
    }

    // transformation from current quaternion position to new one: q2*q1^(-1)
    // definition: inverse of q1 * q2 - inverse of q1 will rotate object back 
    // to original frame, transform by q2 to rotate to new orientation  
    public Quaternion transformTo(Quaternion q) {
        return q.times(this.inverse());
    }

}
