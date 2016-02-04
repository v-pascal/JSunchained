#ifndef UNCHAINED_STORAGE_H_
#define UNCHAINED_STORAGE_H_

#include <Unchained/Global.h>

#if defined(TARGET_OS_ANDROID) || defined(TARGET_OS_WINDOWS)
#include <Unchained/Core/Reply.h>
#else
#include "Reply.h"
#endif
#include <vector>

#define HTTP_REPLY_PNG      "\nContent-Type: image/png\nContent-Length: "
#define HTTP_REPLY_JPG      "\nContent-Type: image/jpeg\nContent-Length: "


//////
class Storage : public Reply {

private:
    std::vector<std::string*> mPictures;
    short mPicIdx;

public:
    Storage(char** response);
    virtual ~Storage();

    enum {

        FOLDER_TYPE_PICTURES = 0,
        FOLDER_TYPE_MOVIES,
#ifdef TARGET_OS_ANDROID
        FOLDER_TYPE_APPLICATION
#else
        FOLDER_TYPE_DOCUMENTS
#endif
    };
    static std::string getFolder(unsigned char type);

    //
    void init();

    inline const char* getBody() const {

        //return HTTP_REPLY_PNG;
        return HTTP_REPLY_JPG;
    };

    bool defaultReply();
    bool reply(const void* data);

};

#endif // UNCHAINED_STORAGE_H_
