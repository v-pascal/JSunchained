#include "Camera.h"

#ifdef __ANDROID__
#include <Unchained/Tools/Tools.h>
#include <Unchained/Log/Log.h>
#include <Unchained/Storage/Storage.h>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#else
#include "Tools.h"
#include "Log.h"
#include "Storage.h"

#include <libGST/libGST.h>

#endif
#include <assert.h>
#include <iostream>
#include <fstream>

#ifdef __ANDROID__
#define BUFFER_SIZE_NV21        static_cast<unsigned int>(CAM_WIDTH * CAM_HEIGHT * 1.5f)
#else
#define BUFFER_SIZE_BGRA        (CAM_WIDTH * CAM_HEIGHT * 4)
#endif

Camera* Camera::mThis = NULL;

//////
Camera::Camera() : mStarted(false), mWidth(0), mHeight(0), mCamBuffer(NULL), mBufferLen(0) {

    LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
#ifdef DEBUG
    mLog = 0;
#endif

#ifndef __ANDROID__
    mPaused = false;
#endif
}
Camera::~Camera() {

    LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    //if (mStarted) stop(); // Crash when close
    if (mCamBuffer)
        delete [] mCamBuffer;
}

bool Camera::start(short width, short height) {

#ifdef __ANDROID__
    LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(" - w:%d; h:%d (j:%p; c:%p; o:%p)"), __PRETTY_FUNCTION__, __LINE__, width,
            height, g_jVM, g_jResClass, g_jResObj);
    assert(g_jVM);
    assert(g_jResClass);
    assert(g_jResObj);
    assert(!mStarted);

    JNIEnv* env = getJavaEnv(UNCHAINED_LOG_CAMERA, __PRETTY_FUNCTION__, __LINE__);
    if (!env)
        return false;

    jmethodID mthd = env->GetMethodID(g_jResClass, "startCamera", "(SS)Z");
    if (!mthd) {

        LOGE(LOG_FORMAT(" - Failed to get 'startCamera' java method"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    mWidth = width;
    mHeight = height;

    if (!env->CallBooleanMethod(g_jResObj, mthd, width, height)) {
#else
    LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(" - w:%d; h:%d (a:%p; s:%s)"), __PRETTY_FUNCTION__, __LINE__, width, height,
         g_AppleOS, (mStarted)? "true":"false");
    assert(g_AppleOS);
    assert(!mStarted);

    mWidth = width;
    mHeight = height;

    if (![g_AppleOS startCamera:width andHeight:height]) {
#endif
        LOGE(LOG_FORMAT(" - Failed to start camera with format %dx%d"), __PRETTY_FUNCTION__, __LINE__, width, height);
        //assert(NULL); // Can occured when Create/Stop operation is done quickly
        return false;
    }
    if (!mCamBuffer) {

        mCamBuffer = new char[MAX_JPEG_SIZE];
        std::memset(mCamBuffer, 0, MAX_JPEG_SIZE);
    }
    mStarted = true;
    return true;
}
#ifdef __ANDROID__
void Camera::pause(bool lockScreen) {

#ifdef DEBUG
    LOGV(UNCHAINED_LOG_CAMERA, (mLog % 100), LOG_FORMAT(" - l:%s (s:%s)"), __PRETTY_FUNCTION__, __LINE__,
            (lockScreen)? "true":"false", (mStarted)? "true":"false");
#endif
#else
void Camera::pause() {

    LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(" - (s:%s)"), __PRETTY_FUNCTION__, __LINE__, (mStarted)? "true":"false");
#endif
    if (!mStarted)
        return;

#ifndef __ANDROID__
    assert(!mPaused);
    mPaused = true;
#endif

    mMutex.lock();
    if (mCamBuffer) {

        delete [] mCamBuffer;
        mCamBuffer = NULL;
        mBufferLen = 0;
    }
    mMutex.unlock();
}
#ifndef __ANDROID__
void Camera::resume() {

    LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(" - (s:%s; w:%d; h:%d)"), __PRETTY_FUNCTION__, __LINE__,
            (mStarted)? "true":"false", mWidth, mHeight);
    if (!mStarted)
        return;

    assert(mPaused);
    mPaused = false;
}
#endif

bool Camera::stop() {

#ifdef __ANDROID__
    LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(" - (j:%p; c:%p; o:%p)"), __PRETTY_FUNCTION__, __LINE__, g_jVM, g_jResClass,
            g_jResObj);
    assert(g_jVM);
    assert(g_jResClass);
    assert(g_jResObj);
    assert(mStarted);

    JNIEnv* env = getJavaEnv(UNCHAINED_LOG_CAMERA, __PRETTY_FUNCTION__, __LINE__);
    if (!env)
        return false;

    jmethodID mthd = env->GetMethodID(g_jResClass, "stopCamera", "()Z");
    if (!mthd) {

        LOGE(LOG_FORMAT(" - Failed to get 'stopCamera' java method"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    if (!env->CallBooleanMethod(g_jResObj, mthd)) {
#else
    LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(" - (a:%p; s:%s)"), __PRETTY_FUNCTION__, __LINE__, g_AppleOS,
            (mStarted)? "true":"false");
    assert(g_AppleOS);
    assert(mStarted);

    if (![g_AppleOS stopCamera]) {
#endif
        // Fail & display error only when not in pause operation coz at least it is impossible to know
        // if the camera has already been started or not
        LOGE(LOG_FORMAT(" - Failed to stop camera"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    mStarted = false;
    return true;
}

void Camera::updateFrame(GstBuffer* jpeg) {

    //LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(" - j:%p"), __PRETTY_FUNCTION__, __LINE__, jpeg);
    mMutex.lock();
    if (!mCamBuffer) {

        mCamBuffer = new char[MAX_JPEG_SIZE];
        std::memset(mCamBuffer, 0, MAX_JPEG_SIZE);
    }
    mBufferLen = gst_buffer_extract(jpeg, 0, mCamBuffer, gst_buffer_get_size(jpeg));
    mMutex.unlock();
}

typedef struct {

    Camera* obj;
    GMainLoop* loop;

} eosData;
void eos(GstAppSink* sink, gpointer data) {

    //LOGV(UNCHAINED_LOG_CAMERA, 0, LOG_FORMAT(" - a:%p; d:%p"), __PRETTY_FUNCTION__, __LINE__, sink, data);
    static_cast<eosData*>(data)->obj->updateFrame(gst_sample_get_buffer(gst_app_sink_pull_sample(GST_APP_SINK(sink))));
    g_main_loop_quit(static_cast<eosData*>(data)->loop);
}
void Camera::updateBuffer(const unsigned char* data) {

#ifdef DEBUG
    ++mLog;
    LOGV(UNCHAINED_LOG_CAMERA, (mLog % 100), LOG_FORMAT(" - d:%p (w:%d; h:%d)"), __PRETTY_FUNCTION__, __LINE__, data,
            mWidth, mHeight);
    //assert(mStarted); // This method can be called before this flag has been set
#endif
#ifndef __ANDROID__
    if (mPaused)
        return;
#endif
    GstElement *pipeline, *appsrc, *conv, *jpegenc, *appsink;
    GMainLoop* loop = g_main_loop_new(NULL, false);

    pipeline = gst_pipeline_new("pipeline");
    appsrc = gst_element_factory_make("appsrc", "source");
    conv = gst_element_factory_make("videoconvert", "conv");
    jpegenc = gst_element_factory_make("jpegenc", "enc");
    appsink = gst_element_factory_make("appsink", "sink");

    assert(pipeline);
    assert(appsrc);
    assert(conv);
    assert(jpegenc);
    assert(appsink);

    g_object_set(G_OBJECT(appsrc), "caps",
            gst_caps_new_simple("video/x-raw",
#ifdef __ANDROID__
                 "format", G_TYPE_STRING, "NV21",
#else
                 "format", G_TYPE_STRING, "BGRA",
#endif
                 "width", G_TYPE_INT, CAM_WIDTH,
                 "height", G_TYPE_INT, CAM_HEIGHT,
                 "framerate", GST_TYPE_FRACTION, 1, 1,
                 NULL),
            NULL);
    gst_bin_add_many(GST_BIN(pipeline), appsrc, conv, jpegenc, appsink, NULL);
    gst_element_link_many(appsrc, conv, jpegenc, appsink, NULL);

    GstBuffer* raw = gst_buffer_new_wrapped_full(GST_MEMORY_FLAG_READONLY, const_cast<unsigned char*>(data),
#ifdef __ANDROID__
            BUFFER_SIZE_NV21, 0, BUFFER_SIZE_NV21, const_cast<unsigned char*>(data), NULL);
#else
            BUFFER_SIZE_BGRA, 0, BUFFER_SIZE_BGRA, const_cast<unsigned char*>(data), NULL);
#endif
    gst_app_src_push_buffer(GST_APP_SRC(appsrc), raw);
    gst_app_src_end_of_stream(GST_APP_SRC(appsrc));

    GstAppSinkCallbacks callbacks = { eos, NULL, NULL };
    eosData eosParam  = { this, loop };
    gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, &eosParam, NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
    g_main_loop_unref(loop);
}
