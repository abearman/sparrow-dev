import com.projecttango.experiments.quickstartjava.PoseData;

/**
 * Created by Pavitra on 4/9/16.
 */
import org.junit.Test;
import java.util.regex.Pattern;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

public class PoseDataTest {

    @Test
    public void checkPoseDataConstruction() {
        double[] translation = {0.0, 0.0, 0.0};
        double[] rotation = {0.0, 0.0, 0.0, 2.0};
        PoseData pt = new PoseData(translation, rotation);
        assertEquals(translation[0], pt.getTranslation()[0], 0.0);
        assertEquals(translation[1], pt.getTranslation()[1], 0.0);
        assertEquals(translation[2], pt.getTranslation()[2], 0.0);
        assertEquals(rotation[0], pt.getRotation()[0], 0.0);
        assertEquals(rotation[1], pt.getRotation()[1], 0.0);
        assertEquals(rotation[2], pt.getRotation()[2], 0.0);
        assertEquals(rotation[3], pt.getRotation()[3], 0.0);
    }
}