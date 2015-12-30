#ifndef UNCHAINED_CAMERA_H_
#define UNCHAINED_CAMERA_H_

#include <boost/thread.hpp>
#include <gst/app/gstappsink.h>

#define CAM_WIDTH           640
#define CAM_HEIGHT          480
#define MAX_JPEG_SIZE       (CAM_WIDTH * CAM_HEIGHT * 3)


//////
class Camera {

    friend void eos(GstAppSink* sink, gpointer data);

private:
    Camera();
    virtual ~Camera();

    static Camera* mThis;

    char* mCamBuffer; // Camera BMP buffer (BGR)
    boost::mutex mMutex;

    bool mStarted;
#ifndef __ANDROID__
    bool mPaused; // iOS only coz on Android the stop camera is managed B4 'pause' method call
#endif
#ifdef DEBUG
    unsigned int mLog;
#endif
    short mWidth;
    short mHeight;
    unsigned int mBufferLen;

    void updateFrame(GstBuffer* jpeg);

public:
    static Camera* getInstance() {
        if (!mThis)
            mThis = new Camera;
        return mThis;
    }
    static void freeInstance() {
        if (mThis) {
            delete mThis;
            mThis = NULL;
        }
    }

    inline bool isStarted() const { return mStarted; }

    inline short getWidth() const { return mWidth; }
    inline short getHeight() const { return mHeight; }

    inline void lock() { mMutex.lock(); }
    inline const char* getBuffer() const { return mCamBuffer; }
    inline unsigned int getLength() const { return mBufferLen; }
    inline void unlock() { mMutex.unlock(); }

    //////
    bool start(short width, short height);

#ifdef __ANDROID__
    void pause(bool lockScreen);
    // WARNING: Do not use this method (reserved)
#else
private:
    void pause();
    void resume();
    // -> Pause/Resume operation reserved
#endif

public:
    bool stop();
    void updateBuffer(const unsigned char* data);

};

#endif // UNCHAINED_CAMERA_H_
