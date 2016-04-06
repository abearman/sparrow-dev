/*
 * Copyright 2014 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.projecttango.experiments.quickstartjava;

import com.google.atap.tangoservice.Tango;
import com.google.atap.tangoservice.Tango.OnTangoUpdateListener;
import com.google.atap.tangoservice.TangoConfig;
import com.google.atap.tangoservice.TangoCoordinateFramePair;
import com.google.atap.tangoservice.TangoErrorException;
import com.google.atap.tangoservice.TangoEvent;
import com.google.atap.tangoservice.TangoOutOfDateException;
import com.google.atap.tangoservice.TangoPoseData;
import com.google.atap.tangoservice.TangoXyzIjData;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import org.apache.http.protocol.HTTP;
import org.apache.http.message.BasicHeader;
import org.apache.http.entity.StringEntity;
import org.apache.http.HttpStatus;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.StatusLine;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.URISyntaxException;
import java.nio.FloatBuffer;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.net.URI;

import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;


/**
 * Main Activity for the Tango Java Quickstart. Demonstrates establishing a
 * connection to the {@link Tango} service and printing the {@link TangoPose}
 * data to the LogCat. Also demonstrates Tango lifecycle management through
 * {@link TangoConfig}.
 */
public class MainActivity extends Activity {


    private static final String TAG = MainActivity.class.getSimpleName();
    private static final String addr = "10.34.165.157";
    private static final int port = 5000;
    private static final String sTranslationFormat = "x%fy%fz%f";
    private static final String sRotationFormat = "i%fj%fk%fl%f";

    private static final int SECS_TO_MILLISECS = 1000;
    private static final double UPDATE_INTERVAL_MS = 100.0;
    private static final DecimalFormat FORMAT_THREE_DECIMAL = new DecimalFormat("0.000");

    private double mPreviousTimeStamp;
    private double mTimeToNextUpdate = UPDATE_INTERVAL_MS;

    private TextView mTranslationTextView;
    private TextView mRotationTextView;
    private TextView mAverageZTextView;
    private TextView mPointCountTextView;
    private TextView mCollisionTextView;

    private Tango mTango;
    private TangoConfig mConfig;
    private boolean mIsTangoServiceConnected;

    private HttpClient httpclient;

    private Socket mSocket;

    // for point cloud
    private double mXyIjPreviousTimeStamp;
    private double mXyzIjTimeToNextUpdate = UPDATE_INTERVAL_MS;

    // collision detector
    private CollisionDetector collisionDetector;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTranslationTextView = (TextView) findViewById(R.id.translation_textview);
        mRotationTextView = (TextView) findViewById(R.id.rotation_textview);
        mPointCountTextView = (TextView) findViewById(R.id.point_count_textview);
        mAverageZTextView = (TextView) findViewById(R.id.average_z_textview);
        mCollisionTextView = (TextView) findViewById(R.id.collision_textview);

        // Instantiate Tango client
        mTango = new Tango(this);

        // Instantiate collision detector
        collisionDetector = new CollisionDetector(1.0);

        // Set up Tango configuration for motion tracking
        // If you want to use other APIs, add more appropriate to the config
        // like: mConfig.putBoolean(TangoConfig.KEY_BOOLEAN_DEPTH, true)
        mConfig = mTango.getConfig(TangoConfig.CONFIG_TYPE_CURRENT);
        mConfig.putBoolean(TangoConfig.KEY_BOOLEAN_MOTIONTRACKING, true);
        mConfig.putBoolean(TangoConfig.KEY_BOOLEAN_DEPTH, true);

        this.httpclient = new DefaultHttpClient();

        try {
            mSocket = IO.socket("http://" + addr + ":" + port + "/pose");
            mSocket.connect();
            System.out.println("Socket connection has been established");
        } catch (URISyntaxException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onResume() {
        // Lock the Tango configuration and reconnect to the service each time
        // the app
        // is brought to the foreground.
        super.onResume();
        if (!mIsTangoServiceConnected) {
            try {
                setTangoListeners();
            } catch (TangoErrorException e) {
                Toast.makeText(this, "Tango Error! Restart the app!",
                        Toast.LENGTH_SHORT).show();
            }
            try {
                mTango.connect(mConfig);
                mIsTangoServiceConnected = true;
            } catch (TangoOutOfDateException e) {
                Toast.makeText(getApplicationContext(),
                        "Tango Service out of date!", Toast.LENGTH_SHORT)
                        .show();
            } catch (TangoErrorException e) {
                Toast.makeText(getApplicationContext(),
                        "Tango Error! Restart the app!", Toast.LENGTH_SHORT)
                        .show();
            }
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        // When the app is pushed to the background, unlock the Tango
        // configuration and disconnect
        // from the service so that other apps will behave properly.
        try {
            mTango.disconnect();
            mIsTangoServiceConnected = false;
        } catch (TangoErrorException e) {
            Toast.makeText(getApplicationContext(), "Tango Error!",
                    Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mSocket.disconnect();
    }

    private void setTangoListeners() {
        // Select coordinate frame pairs
        ArrayList<TangoCoordinateFramePair> framePairs = new ArrayList<TangoCoordinateFramePair>();
        framePairs.add(new TangoCoordinateFramePair(
                TangoPoseData.COORDINATE_FRAME_START_OF_SERVICE,
                TangoPoseData.COORDINATE_FRAME_DEVICE));

        // Add a listener for Tango pose data
        mTango.connectListener(framePairs, new OnTangoUpdateListener() {

            @Override
            public void onPoseAvailable(TangoPoseData tangoPose) {

                // Format Translation and Rotation data
                final String translationMsg = String.format(sTranslationFormat,
                        tangoPose.translation[0], tangoPose.translation[1],
                        tangoPose.translation[2]);
                final String rotationMsg = String.format(sRotationFormat,
                        tangoPose.rotation[0], tangoPose.rotation[1],
			            tangoPose.rotation[2], tangoPose.rotation[3]);

		        PoseData pose = new PoseData(tangoPose);

                // Output to LogCat
                String logMsg = translationMsg + "_" + rotationMsg;

                // keeping this uncommented for the purpose of testing without server
                HttpResponse response;
                String responseString = null;
                JSONObject json = new JSONObject();
                try{
                    //HttpPost post = new HttpPost("http://" + addr + ":" + port + "/pose/update");


                    json.put("x", Double.toString(tangoPose.translation[0]));
                    json.put("y", Double.toString(tangoPose.translation[1]));
                    json.put("z", Double.toString(tangoPose.translation[2]));
                    json.put("i", Double.toString(tangoPose.rotation[0]));
                    json.put("j", Double.toString(tangoPose.rotation[1]));
                    json.put("k", Double.toString(tangoPose.rotation[2]));
                    json.put("l", Double.toString(tangoPose.rotation[3]));

                    try {
                        mSocket.emit("pose_update", json);
                        Log.i(TAG, logMsg);
                    } catch (Exception e) {
                        System.out.println("Exception when emitting");
                        e.printStackTrace();
                    }

                    //json.put("pose", logMsg);

                    /*
                    StringEntity se = new StringEntity(json.toString());
                    se.setContentEncoding(new BasicHeader(HTTP.CONTENT_TYPE, "application/json"));
                    post.setEntity(se);
                    response = httpclient.execute(post);
                    System.out.println(response.getStatusLine().getStatusCode());
                    response.getEntity().getContent().close();
                    */

                } catch(Exception e){
                        e.printStackTrace();
                }


                final double deltaTime = (tangoPose.timestamp - mPreviousTimeStamp)
                        * SECS_TO_MILLISECS;
                mPreviousTimeStamp = tangoPose.timestamp;
                mTimeToNextUpdate -= deltaTime;

                // Throttle updates to the UI based on UPDATE_INTERVAL_MS.
                if (mTimeToNextUpdate < 0.0) {
                    mTimeToNextUpdate = UPDATE_INTERVAL_MS;

                    // Display data in TextViews. This must be done inside a
                    // runOnUiThread call because
                    // it affects the UI, which will cause an error if performed
                    // from the Tango
                    // service thread
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mRotationTextView.setText(rotationMsg);
                            mTranslationTextView.setText(translationMsg);
                        }
                    });
                }
            }

            @Override
            public void onXyzIjAvailable(final TangoXyzIjData xyzIj) {

                System.out.println("XYZIJ is available");

                final double currentTimeStamp = xyzIj.timestamp;
                final double pointCloudFrameDelta = (currentTimeStamp - mXyIjPreviousTimeStamp)
                        * SECS_TO_MILLISECS;
                mXyIjPreviousTimeStamp = currentTimeStamp;
                final double averageDepth = getAveragedDepth(xyzIj.xyz);

                mXyzIjTimeToNextUpdate -= pointCloudFrameDelta;

                if (mXyzIjTimeToNextUpdate < 0.0) {
                    mXyzIjTimeToNextUpdate = UPDATE_INTERVAL_MS;
                    final String pointCountString = "point count: " + Integer.toString(xyzIj.xyzCount);
                    System.out.println(pointCountString);

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mPointCountTextView.setText(pointCountString);
                            String averageDepthString = "average depth: " + FORMAT_THREE_DECIMAL.format(averageDepth);
                            mAverageZTextView.setText(averageDepthString);
                            System.out.println(averageDepthString);
                            if (collisionDetector.goingToCollide(averageDepth)) {
                                mCollisionTextView.setText("You're going to collide!");
                            } else {
                                mCollisionTextView.setText("");
                            }
                        }
                    });
                }
            }

            @Override
            public void onTangoEvent(TangoEvent arg0) {
                // Ignoring TangoEvents
            }

            @Override
            public void onFrameAvailable(int arg0) {
                // Ignoring onFrameAvailable Events
            }
        });
    }

    /**
     * Calculates the average depth from a point cloud buffer.
     *
     * @param pointCloudBuffer
     * @return Average depth.
     */
    private float getAveragedDepth(FloatBuffer pointCloudBuffer) {
        int pointCount = pointCloudBuffer.capacity() / 3;
        float totalZ = 0;
        float averageZ = 0;
        for (int i = 0; i < pointCloudBuffer.capacity() - 3; i = i + 3) {
            totalZ = totalZ + pointCloudBuffer.get(i + 2);
        }
        if (pointCount != 0) {
            averageZ = totalZ / pointCount;
        }
        return averageZ;
    }



}
