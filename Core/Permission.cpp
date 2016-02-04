#include "Permission.h"

#define MAX_RESPONSE_SIZE   48

// Json
#define PERM_JSON           "{\"permission\":{\"allowed\":%d,\"mask\":%d}}"


//////
Permission::Permission(char** response) : Reply(response) {

    LOGV(UNCHAINED_LOG_PERMISSION, 0, LOG_FORMAT(" - r:%p"), __PRETTY_FUNCTION__, __LINE__, response);
#ifndef UNCHAINED_COMPONENT
    mPermission = 0;
    mAllowed = PERM_PROGRESS;

    mWaiting = false;
    
#ifndef TARGET_OS_ANDROID
    mAlertDlg = nil;
#endif
#endif
}
Permission::~Permission() {

    LOGV(UNCHAINED_LOG_PERMISSION, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
#ifndef UNCHAINED_COMPONENT
    for (std::vector<std::string*>::iterator iter = mPermURL.begin(); iter != mPermURL.end(); ++iter)
        delete (*iter);
    mPermURL.clear();

#ifndef TARGET_OS_ANDROID
    if (mAlertDlg != nil)
        [mAlertDlg release];
#endif
#endif
}

bool Permission::allowPermission() {

#ifndef UNCHAINED_COMPONENT
#ifdef TARGET_OS_ANDROID
    LOGV(UNCHAINED_LOG_PERMISSION, 0, LOG_FORMAT(" - (u:%s; j:%p; c:%p; o:%p)"), __PRETTY_FUNCTION__, __LINE__,
            mURL.substr(sizeof(HTTP_REPLY_HEAD) - 1).c_str(), g_jVM, g_jResClass, g_jResObj);
    assert(g_jVM);
    assert(g_jResClass);
    assert(g_jResObj);

    JNIEnv* env = getJavaEnv(UNCHAINED_LOG_PERMISSION, __PRETTY_FUNCTION__, __LINE__);
    if (!env)
        return false;

    jmethodID mthd = env->GetMethodID(g_jResClass, "allowPermission", "(Ljava/lang/String;I)V");
    if (!mthd) {

        LOGE(LOG_FORMAT(" - Failed to get 'allowPermission' java method"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    jstring jURL = env->NewStringUTF(mURL.substr(sizeof(HTTP_REPLY_HEAD) - 1).c_str());
    env->CallVoidMethod(g_jResObj, mthd, jURL, mPermission);
    env->DeleteLocalRef(jURL);

#else
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
        if (mAlertDlg != nil)
            [mAlertDlg release];

        mAlertDlg = [[NSAlert alloc] initWithURL:mURL.substr(sizeof(HTTP_REPLY_HEAD) - 1).c_str()
                               andPermissions:mPermission];
        [mAlertDlg show];
    }];
#endif
#endif
    return true;
}
bool Permission::reply(const void* data) {

    mLength = MAX_RESPONSE_SIZE;
    checkIncBuffer();

#ifndef UNCHAINED_COMPONENT
    if (!mWaiting) {

        const Info* info = static_cast<const Info*>(data);
        mPermission = info->perm;
        if (checkPerm(info->url)) {

            LOGI(UNCHAINED_LOG_PERMISSION, 0, LOG_FORMAT(" - %s already permissioned"), __PRETTY_FUNCTION__, __LINE__,
                    info->url->c_str());
            mAllowed = PERM_ALLOWED;
        }
        else {

            mURL = *info->url;
            mWaiting = true;
            mAllowed = PERM_PROGRESS;
            allowPermission();
        }
    }
    if ((mWaiting) && (mAllowed)) {

        if (mAllowed == 1)
            mPermURL.push_back(new std::string(mURL));
        mWaiting = false;
    }
    sprintf(*mResponse, PERM_JSON, mAllowed, mPermission);
    mLength = static_cast<int>(strlen(*mResponse));

#else
#ifdef TARGET_OS_WINDOWS
    sprintf_s(*mResponse, MAX_RESPONSE_SIZE, PERM_JSON, 1, PERMISSION_MASK_ALL);
#else
    sprintf(*mResponse, PERM_JSON, 1, PERMISSION_MASK_ALL);
#endif
    mLength = static_cast<int>(strlen(*mResponse));

#endif
    return true;
}
