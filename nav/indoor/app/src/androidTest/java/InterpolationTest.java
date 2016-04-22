import com.projecttango.experiments.quickstartjava.Path;
import com.projecttango.experiments.quickstartjava.PoseData;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

/**
 * Created by Pavitra on 4/13/16.
 */
public class InterpolationTest {

    @Test
     public void moveInXDirection() {
        double[] translation1 = {0.0, 0.0, 0.0};
        double[] rotation1 = {0.0, 0.0, 0.0, 0.0};
        PoseData pt1 = new PoseData(translation1, rotation1);

        double[] translation2 = {5.0, 0.0, 0.0};
        double[] rotation2 = {0.0, 0.0, 0.0, 0.0};
        PoseData pt2 = new PoseData(translation2, rotation2);

        Path path = new Path();
        path.addPose(pt1);
        path.addPose(pt2);

        Path interpolatedPath = path.interpolate(1.0);

        assertEquals(6, interpolatedPath.getPoseList().size(), 0);

        assertEquals(0.0, interpolatedPath.getPoseList().get(0).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(0).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(0).getTranslation()[2], 0.0);

        assertEquals(1.0, interpolatedPath.getPoseList().get(1).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(1).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(1).getTranslation()[2], 0.0);

        assertEquals(2.0, interpolatedPath.getPoseList().get(2).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(2).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(2).getTranslation()[2], 0.0);

        assertEquals(3.0, interpolatedPath.getPoseList().get(3).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(3).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(3).getTranslation()[2], 0.0);

        assertEquals(4.0, interpolatedPath.getPoseList().get(4).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(4).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(4).getTranslation()[2], 0.0);

        assertEquals(5.0, interpolatedPath.getPoseList().get(5).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(5).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(5).getTranslation()[2], 0.0);

    }

    @Test
    public void test3Points() {
        double[] translation1 = {0.0, 0.0, 0.0};
        double[] rotation1 = {0.0, 0.0, 0.0, 0.0};
        PoseData pt1 = new PoseData(translation1, rotation1);

        double[] translation2 = {5.0, 0.0, 0.0};
        double[] rotation2 = {0.0, 0.0, 0.0, 0.0};
        PoseData pt2 = new PoseData(translation2, rotation2);

        double[] translation3 = {5.0, 5.0, 0.0};
        double[] rotation3 = {0.0, 0.0, 0.0, 0.0};
        PoseData pt3 = new PoseData(translation3, rotation3);

        Path path = new Path();
        path.addPose(pt1);
        path.addPose(pt2);
        path.addPose(pt3);

        Path interpolatedPath = path.interpolate(1.0);

        assertEquals(11, interpolatedPath.getPoseList().size(), 0);

        assertEquals(0.0, interpolatedPath.getPoseList().get(0).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(0).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(0).getTranslation()[2], 0.0);

        assertEquals(1.0, interpolatedPath.getPoseList().get(1).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(1).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(1).getTranslation()[2], 0.0);

        assertEquals(2.0, interpolatedPath.getPoseList().get(2).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(2).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(2).getTranslation()[2], 0.0);

        assertEquals(3.0, interpolatedPath.getPoseList().get(3).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(3).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(3).getTranslation()[2], 0.0);

        assertEquals(4.0, interpolatedPath.getPoseList().get(4).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(4).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(4).getTranslation()[2], 0.0);

        assertEquals(5.0, interpolatedPath.getPoseList().get(5).getTranslation()[0], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(5).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(5).getTranslation()[2], 0.0);

        assertEquals(5.0, interpolatedPath.getPoseList().get(6).getTranslation()[0], 0.0);
        assertEquals(1.0, interpolatedPath.getPoseList().get(6).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(6).getTranslation()[2], 0.0);

        assertEquals(5.0, interpolatedPath.getPoseList().get(7).getTranslation()[0], 0.0);
        assertEquals(2.0, interpolatedPath.getPoseList().get(7).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(7).getTranslation()[2], 0.0);

        assertEquals(5.0, interpolatedPath.getPoseList().get(8).getTranslation()[0], 0.0);
        assertEquals(3.0, interpolatedPath.getPoseList().get(8).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(8).getTranslation()[2], 0.0);

        assertEquals(5.0, interpolatedPath.getPoseList().get(9).getTranslation()[0], 0.0);
        assertEquals(4.0, interpolatedPath.getPoseList().get(9).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(9).getTranslation()[2], 0.0);

        assertEquals(5.0, interpolatedPath.getPoseList().get(10).getTranslation()[0], 0.0);
        assertEquals(5.0, interpolatedPath.getPoseList().get(10).getTranslation()[1], 0.0);
        assertEquals(0.0, interpolatedPath.getPoseList().get(10).getTranslation()[2], 0.0);

    }

}
