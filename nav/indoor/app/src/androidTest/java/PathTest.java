import com.projecttango.experiments.quickstartjava.PoseData;
import com.projecttango.experiments.quickstartjava.Path;

/**
 * Created by Pavitra on 4/9/16.
 */
import org.junit.Test;
import java.util.regex.Pattern;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

public class PathTest {

    @Test
    public void checkPathConstruction() {

        double[] translation1 = {0.0, 0.0, 0.0};
        double[] rotation1 = {0.0, 0.0, 0.0, 0.0};
        PoseData pt1 = new PoseData(translation1, rotation1);

        double[] translation2 = {3.0, 3.0, 3.0};
        double[] rotation2 = {0.0, 0.0, 0.0, 0.0};
        PoseData pt2 = new PoseData(translation2, rotation2);

        Path path = new Path();
        path.addPose(pt1);
        path.addPose(pt2);

        assertEquals(2, path.getPoseList().size(), 0.0);

    }
}