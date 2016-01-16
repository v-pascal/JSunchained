#ifndef UNCHAINED_REPLY_H_
#define UNCHAINED_REPLY_H_

#if defined(__ANDROID__) || defined(_WINDLL)
#include <Unchained/Log/Log.h>
#include <Unchained/Tools/Tools.h>

#else // iOS
#include "Log.h"
#include "Tools.h"

#endif
#include <string>
#include <assert.h>

#define HTTP_REPLY_JSON     "\nContent-Type: application/json; charset=utf-8\nContent-Length: "
#define HTTP_GET            "GET /"


//////
class Reply {

protected:
    char** mResponse;
    static int mMaxSize;
    int mLength;

    void checkIncBuffer();

public:
    Reply(char** response);
    virtual ~Reply();

    virtual bool reply(const void* data) { return false; }
    virtual void reset() { }

    inline int getLength() const { return mLength; }

};

#endif // UNCHAINED_REPLY_H_
