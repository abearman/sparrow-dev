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

package com.projecttango.examples.cpp.videooverlay;

import android.opengl.GLSurfaceView;
import android.os.AsyncTask;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import java.net.URISyntaxException;

//import com.github.nkzawa.socketio.client.Ack;
//import com.github.nkzawa.socketio.client.IO;
//import com.github.nkzawa.socketio.client.Socket;

//import io.socket.engineio.client.Socket;
import io.socket.client.Socket;
import io.socket.client.IO;
import io.socket.client.Ack;
import io.socket.emitter.Emitter;
import jp.co.cyberagent.android.gpuimage.GPUImageNativeLibrary;

import android.util.Base64;

import org.json.JSONException;
import org.json.JSONObject;

import java.nio.*;
import java.net.*;

/**
 * Render render all GL content to the glSurfaceView.
 */
public class Renderer implements GLSurfaceView.Renderer {

  private static final String addr = "10.1.1.188";
  private static final int port = 5000;
  private Socket mSocket;

  Renderer() {
    try {
        mSocket = IO.socket("http://" + addr + ":" + port); // + "/camera");
        mSocket.on(Socket.EVENT_CONNECT, new Emitter.Listener() {

          @Override
          public void call(Object... args) {
            System.out.println("SUCCESSFULLY CONNECTED TO SOCKET!");
          }

        });
       mSocket.connect();
      // mSocket = new Socket("http://" + addr + ":" + port + "/camera");
      // mSocket.open();
      System.out.println("Socket connection has been established");
    } catch (URISyntaxException e) {
      e.printStackTrace();
    }
  }

  static boolean sendFrame = true;

  // Render loop of the Gl context.
  public void onDrawFrame(GL10 gl) {
    byte[] img_data = TangoJNINative.render();
    // mSocket.send(img_data);
    //img_data.length == 2764800
//    img_data.length == 1.5*1280*720
//    if (sendFrame && img_data.length == 1.5*1280*720) {
//      int[] rgb_data = new int[2764800];
//      GPUImageNativeLibrary.YUVtoRBGA(img_data, 1280, 720, rgb_data);
//      System.out.println("Image data: " + rgb_data.length);
//      System.out.println("Sending image");
//
//      ByteBuffer byteBuffer = ByteBuffer.allocate(rgb_data.length * 4);
//      IntBuffer intBuffer = byteBuffer.asIntBuffer();
//      intBuffer.put(rgb_data);
//
//      byte[] byte_data = byteBuffer.array();
//
//      byte[] encoded_data = Base64.encode(byte_data, Base64.DEFAULT);
//      JSONObject json = new JSONObject();
//      try {
//        json.put("image64", encoded_data);
//      } catch (JSONException e) {
//        e.printStackTrace();
//      }

    if (sendFrame && img_data.length == (2764800 / 25)) {
      System.out.println("SENDING FRAME");
      JSONObject json = new JSONObject();
      try {
        json.put("image", img_data);
      } catch (JSONException e) {
        e.printStackTrace();
      }
      sendFrame = false;
    mSocket.emit("frame", json, new Ack() {
      @Override
      public void call(Object... args) {
        System.out.println("Image received");
        sendFrame = true;
      }
    });
  }
    // img_data = null;
    /*
    AsyncTask.execute(new Runnable() {
      @Override
      public void run() {
        mSocket.emit("frame", img_data);
      }
    });
    */
  }

  // Called when the surface size changes.
  public void onSurfaceChanged(GL10 gl, int width, int height) {
    TangoJNINative.setupGraphic(width, height);
  }

  // Called when the surface is created or recreated.
  public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    TangoJNINative.initGlContent();
  }
}
