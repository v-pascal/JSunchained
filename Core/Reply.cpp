#include "Reply.h"


int Reply::mMaxSize = 0;

//////
Reply::Reply(char** response) : mResponse(response), mLength(0) { }
Reply::~Reply() { }

void Reply::checkIncBuffer() {

    //LOGV(UNCHAINED_LOG_REPLY, 0, LOG_FORMAT(" - (l:%d; m:%d)"), __PRETTY_FUNCTION__, __LINE__, mLength, mMaxSize);
    if (mLength > mMaxSize) {
        if (*mResponse)
            delete [] *mResponse;

        mMaxSize = UNCHAINED_GET_MEM2ALLOC(mLength);
        *mResponse = new char[mMaxSize];
    }
}
