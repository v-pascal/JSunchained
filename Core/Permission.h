#ifndef UNCHAINED_PERMISSION_H_
#define UNCHAINED_PERMISSION_H_

#include <Unchained/Global.h>

#if defined(TARGET_OS_ANDROID) || defined(TARGET_OS_WINDOWS)
#include <Unchained/Core/Reply.h>

#else
#include "Reply.h"
#import "NSResources.h"

#endif
#include <vector>

#define PERMISSION_MASK_CAMERA          0x00000001

#define PERMISSION_MASK_ALL             PERMISSION_MASK_CAMERA

#ifndef UNCHAINED_COMPONENT
#define PERMISSION_ACCES_ALLOWED        1
#define PERMISSION_ACCES_NOT_ALLOWED    2
#endif

#define HTTP_REPLY_HEAD                 "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: "


//////
class Permission : public Reply {

private:
#ifndef UNCHAINED_COMPONENT
    std::vector<std::string*> mPermURL;
    inline bool checkPerm(const std::string* url) const {

        LOGV(UNCHAINED_LOG_PERMISSION, 0, LOG_FORMAT(" - u:%p"), __PRETTY_FUNCTION__, __LINE__, url);
        LOGV(UNCHAINED_LOG_PERMISSION, 0, LOG_FORMAT(" - u:%s"), __PRETTY_FUNCTION__, __LINE__, url->c_str());
        for (std::vector<std::string*>::const_iterator iter = mPermURL.begin(); iter != mPermURL.end(); ++iter)
            if (!(*iter)->compare(*url))
                return true;

        return false;
    };

    unsigned int mPermission;
    std::string mURL;

    volatile unsigned char mAllowed;
    bool mWaiting;

#ifndef TARGET_OS_ANDROID
    NSAlert* mAlertDlg;
#endif
#endif
    bool allowPermission();

public:
    Permission(char** response);
    virtual ~Permission();

    enum {
        PERM_PROGRESS = 0,
        PERM_ALLOWED,
        PERM_NOT_ALLOWED
    };
#ifndef UNCHAINED_COMPONENT
    inline void setAllowed(unsigned char allowed) { mAllowed = allowed; }
#endif
    typedef struct {

        unsigned int perm;
        const std::string* url;

    } Info;

    //
    bool reply(const void* data);

};

#endif // UNCHAINED_PERMISSION_H_
