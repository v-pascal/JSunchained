#include "Video.h"

#define MAX_RESPONSE_SIZE   32

// Json
#define CAMERA_JSON         "{\"camera\":{\"playing\":%c}}"


//////
Video::Video(char** response) : Reply(response), mBackup(NULL) {

    LOGV(UNCHAINED_LOG_VIDEO, 0, LOG_FORMAT(" - r:%p"), __PRETTY_FUNCTION__, __LINE__, response);
    mCamera = Camera::getInstance();
}
Video::~Video() {

    LOGV(UNCHAINED_LOG_VIDEO, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Camera::freeInstance();
}

bool Video::reply(const void* data) {

    char playing;
    switch (*static_cast<const unsigned char*>(data)) {

        case REQ_START: playing = (mCamera->start(CAM_WIDTH, CAM_HEIGHT))? '1':'0'; break;
        case REQ_STOP:  playing = (mCamera->stop())? '0':'1'; break;
        case REQ_FRAME: {

            mBackup = *mResponse;

            mCamera->lock();
            mLength = mCamera->getLength();
            *mResponse = const_cast<char*>(mCamera->getBuffer());
            return true;
        }
#ifdef DEBUG
        default: {

            LOGE(LOG_FORMAT(" - Unexpected camera request: %d"), __PRETTY_FUNCTION__, __LINE__,
                    *static_cast<const unsigned char*>(data));
            assert(NULL);
            playing = '0';
            return false;
        }
#endif
    }
    mLength = MAX_RESPONSE_SIZE;
    checkIncBuffer();
#ifdef TARGET_OS_WINDOWS
    sprintf_s(*mResponse, MAX_RESPONSE_SIZE, CAMERA_JSON, playing);
#else
    sprintf(*mResponse, CAMERA_JSON, playing);
#endif
    mLength = static_cast<int>(strlen(*mResponse));

    return true;
}
void Video::reset() {
    if (mBackup) {

        //LOGI(UNCHAINED_LOG_VIDEO, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        mCamera->unlock();

        *mResponse = mBackup;
        mBackup = NULL;
    }
}
