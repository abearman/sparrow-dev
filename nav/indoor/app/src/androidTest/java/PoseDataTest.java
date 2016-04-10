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
        double[] rotation = {0.0, 0.0, 0.0, 0.0};
        PoseData pt = new PoseData(translation, rotation);
        assertEquals(translation, pt.getTranslation());
        assertEquals(rotation, pt.getRotation());
    }
}