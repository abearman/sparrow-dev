/*
 * Date: 03/10/2016
 * Author: Pavitra Rengarajan
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

    public List<PoseData> interpolateBetween(PoseData p1, PoseData p2, double intervalDistance) {
        List<PoseData> points = new ArrayList<PoseData>();

        double actualDist = Utils.getDist(p1.getTranslation(), p2.getTranslation());
        double desiredDist = intervalDistance;

        if (actualDist > desiredDist) {
            double factor = desiredDist / actualDist;

            double[] newTranslation = p1.getTranslation();
            double[] startTranslation = p1.getTranslation();
            double[] endTranslation = p2.getTranslation();

            double diffX = endTranslation[0] - startTranslation[0];
            double diffY = endTranslation[1] - startTranslation[1];
            double diffZ = endTranslation[2] - startTranslation[2];

            while (Utils.getDist(newTranslation, p2.getTranslation()) >= intervalDistance) {

                PoseData newPoint = new PoseData(newTranslation, p1.getRotation());

                newTranslation[0] = newTranslation[0] + factor*diffX;
                newTranslation[1] = newTranslation[1] + factor*diffY;
                newTranslation[2] = newTranslation[2] + factor*diffZ;



                points.add(newPoint);
            }
        } else {
            points.add(p1);
            points.add(p2);
        }

        return points;
    }

    public Path poseListToPath(List<PoseData> poseList) {
        Path path = new Path();
        for (int i = 0; i < poseList.size(); i++) {
            path.addPose(poseList.get(i));
        }

        for (int i = 0; i < path.getPoseList().size(); i++){
            PoseData pose = path.getPoseList().get(i);
            double[] poseTranslation = pose.getTranslation();
        }

        return path;
    }

    public Path interpolate(double intervalDistance) {
        List<PoseData> interpolatedList = new ArrayList<PoseData>();

        List<PoseData> originalPoseList = getPoseList();

        for (int i = 0; i < originalPoseList.size()-1; i++) {
            List<PoseData> intermediateList = interpolateBetween(originalPoseList.get(i), originalPoseList.get(i+1), intervalDistance);
            interpolatedList.addAll(intermediateList);
        }

        // add last point
        PoseData lastPose = originalPoseList.get(originalPoseList.size()-1);
        interpolatedList.add(lastPose);

        Path returnPath = poseListToPath(interpolatedList);
        return returnPath;
    }

};