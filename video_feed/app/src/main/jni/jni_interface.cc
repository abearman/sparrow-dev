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

#define GLM_FORCE_RADIANS

#include <jni.h>
#include <tango-video-overlay/video_overlay_app.h>
#include <utility>

static tango_video_overlay::VideoOverlayApp app;

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jboolean JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_checkTangoVersion(
    JNIEnv* env, jobject, jobject activity, jint min_tango_version) {
  return app.CheckTangoVersion(env, activity, min_tango_version);
}

JNIEXPORT jint JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_setupConfig(
    JNIEnv*, jobject) {
  return app.TangoSetupConfig();
}

JNIEXPORT jint JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_connect(
    JNIEnv*, jobject) {
  return app.TangoConnect();
}

JNIEXPORT void JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_disconnect(
    JNIEnv*, jobject) {
  app.TangoDisconnect();
}

JNIEXPORT void JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_initGlContent(
    JNIEnv*, jobject) {
  app.InitializeGLContent();
}

JNIEXPORT void JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_setupGraphic(
    JNIEnv*, jobject, jint width, jint height) {
  app.SetViewPort(width, height);
}

JNIEXPORT jbyteArray JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_render(JNIEnv* env,
                                                                      jobject) {
    std::vector<uint8_t> rgb = app.Render();
//    LOGE("**********RENDER**********: %d", yuv.size());
    jbyteArray ret = env->NewByteArray(rgb.size());
    env->SetByteArrayRegion (ret, 0, rgb.size(), (const signed char *)rgb.data());
    // delete yuv;
    return ret;
    /*
    jbyteArray ret = env->NewByteArray(1.5*1280*720);
    if (yuv->width != 1280 && yuv->height != 720) {
        LOGE("Invalid image dimensions: %d x %d", yuv->width, yuv->height);
        LOGE("Freeing TangoImageBuffer...");
        delete yuv;
        return ret;
    }
    if (ret == NULL) {
        LOGE("Cannot allocate JNI Byte Array for Image Frame");
        return NULL;
    }
    env->SetByteArrayRegion (ret, 0, 1.5*1280*720, (const signed char *)yuv->data);
    LOGE("Freeing TangoImageBuffer...");
    delete yuv;
    return ret;
     */

    // return app.Render();
}

JNIEXPORT void JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_freeBufferData(
    JNIEnv*, jobject) {
  app.FreeBufferData();
}

JNIEXPORT void JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_setYUVMethod(
    JNIEnv*, jobject) {
  app.SetTextureMethod(0);
}

JNIEXPORT void JNICALL
Java_com_projecttango_examples_cpp_videooverlay_TangoJNINative_setTextureMethod(
    JNIEnv*, jobject) {
  app.SetTextureMethod(1);
}

#ifdef __cplusplus
}
#endif
