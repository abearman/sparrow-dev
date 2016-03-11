package com.projecttango.experiments.quickstartjava;

/**
 * Created by Pavitra on 3/10/16.
 */
public class CollisionDetector {

    // avg depth distance that is consider to be a collision in meters
    private double collisionDepthDistance;

    // create a new object with the given components
    public CollisionDetector(double collisionDepthDistance) {
        this.collisionDepthDistance = collisionDepthDistance;
    }

    // collisionDepthDistance accessor
    public double getCollisionDepthDistance() {
        return this.collisionDepthDistance;
    }

    // collisionDepthDistance mutator
    public void setCollisionDepthDistance(double collisionDepthDistance) {
        this.collisionDepthDistance = collisionDepthDistance;
    }

    // TODO: implement a real collision avoidance system
    public boolean goingToCollide(double averageDepth) {
        if (averageDepth <= collisionDepthDistance) {
            return true;
        } else {
            return false;
        }
    }

}
