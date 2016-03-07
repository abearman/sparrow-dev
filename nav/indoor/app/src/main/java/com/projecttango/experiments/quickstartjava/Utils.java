/**
 * Date: 03/06/2016
 * Author: Pavitra Rengarajan
 */

package com.projecttango.experiments.quickstartjava;

public class Utils {

    // find distance between two points
    public static double getDist(double[] vec1, double[] vec2) {
        double[] diffVec = diff(vec1, vec2);
        double dist = 0;
        for (int i = 0; i < diffVec.length; i++) {
            dist += diffVec[i]*diffVec[i];
        }
        dist = Math.sqrt(dist);
        return dist;
    }


    // performs subtraction: vec2 - vec1
    public static double[] diff(double[] vec1, double[] vec2) {
        double[] diffVec = new double[vec1.length];
        for (int i = 0; i < vec1.length; i++) {
            diffVec[i] = vec2[i] - vec1[i];
        }
        return diffVec;
    }

    // performs normalization of vec
    public static double[] normalize(double[] vec, double desiredVecLength) {
        double[] normVec = new double[vec.length];
        int len = 0;
        for (int i = 0; i < vec.length; i++) {
            len += vec[i] * vec[i];
        }
        double normFactor = desiredVecLength/len;
        for (int i = 0; i < vec.length; i++) {
            normVec[i] = vec[i]*normFactor;
        }
        return normVec;
    }

}
