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

/**
 * Interfaces between native C++ code and Java code.
 */
public class TangoJNINative {
  static {
    System.loadLibrary("cpp_video_overlay_example");
  }

  // Check that the installed version of the Tango API is up to date.
  public static native boolean checkTangoVersion(VideoOverlayActivity activity,
      int minTangoVersion);

  // Setup the configuration file of the Tango Service. We are also setting up
  // the auto-recovery option from here.
  public static native int setupConfig();

  // Connect to the Tango Service.
  // This function will start the Tango Service pipeline, in this case, it will
  // start the video overlay update.
  public static native int connect();

  // Disconnect from the Tango Service, release all the resources that the app is
  // holding from the Tango Service.
  public static native void disconnect();

  // Release all image-buffer resources that are allocated from the program.
  public static native void freeBufferData();

  // Allocate OpenGL resources for rendering.
  public static native void initGlContent();

  // Setup the view port width and height.
  public static native void setupGraphic(int width, int height);

  // Main render loop.
  public static native byte[] render();

  // Set texture method.
  public static native void setYUVMethod();

  // Set texture method.
  public static native void setTextureMethod();
}
