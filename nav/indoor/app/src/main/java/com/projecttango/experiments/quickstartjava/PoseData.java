/*
 * Date: 03/03/2016
 * Author: Andrew Lim
 */

package com.projecttango.experiments.quickstartjava;

import com.google.atap.tangoservice.TangoPoseData;

public class PoseData {
    private double[] translation;
    private double[] rotation;

    public PoseData(TangoPoseData tangoPoseData) {
	this(tangoPoseData.translation,
	     tangoPoseData.rotation);
    };

    public PoseData(double[] translation,
		            double[] rotation) {
	this.translation = translation.clone();
	this.rotation = rotation.clone();
    }

};