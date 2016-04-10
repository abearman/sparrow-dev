/*
 * Date: 03/03/2016
 * Author: Andrew Lim
 */

package com.projecttango.experiments.quickstartjava;

import java.util.List;
import java.util.ArrayList;

public class Path {
    private List<PoseData> poseList;

    public Path() {
	    poseList = new ArrayList<PoseData>();
    }

    public void addPose(PoseData pose) {
        poseList.add(pose);
    }

    public List<PoseData> getPoseList() {
        return poseList;
    }
};