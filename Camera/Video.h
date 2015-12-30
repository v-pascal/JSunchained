#ifndef UNCHAINED_VIDEO_H_
#define UNCHAINED_VIDEO_H_

#ifdef __ANDROID__
#include <Unchained/Core/Reply.h>
#include <Unchained/Features/Camera/Camera.h>
#else
#include "Reply.h"
#include "Camera.h"
#endif

#define HTTP_GET_CAMERA     (HTTP_GET "camera/st")
#define HTTP_REPLY_JPEG     "\nContent-Type: image/jpeg\nContent-Length: "


//////
class Video : public Reply {

private:
    Camera* mCamera;
    char* mBackup;

public:
    Video(char** response);
    virtual ~Video();

    enum {

        REQ_START = 0,
        REQ_STOP,
        REQ_FRAME
    };

    //
    bool reply(const void* data);
    void reset();

};

#endif // UNCHAINED_VIDEO_H_
