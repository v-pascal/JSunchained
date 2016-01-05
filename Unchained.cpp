#include "Unchained.h"

#ifdef __ANDROID__
#include <jni.h>
#include <Unchained/Log/Log.h>
#include <Unchained/Core/Core.h>

#else
#include "Log.h"
#include "Core.h"

#import <libGST/libGST.h>

#endif


#ifdef __ANDROID__
JavaVM* g_jVM = NULL;
jclass g_jResClass = NULL;
jobject g_jResObj = NULL;

#endif

Core* unchainedCore = NULL;

////// Core
void unchainedInit(const PlatformData* data) {

    LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - d:%p"), __PRETTY_FUNCTION__, __LINE__, data);
#ifdef __ANDROID__
    g_jVM = data->jvm;
    g_jResClass = data->cls;
    g_jResObj = data->res;

#else
    lib_gst_init();
#endif
    unchainedCore = Core::getInstance();
}
const char* unchainedKey() { return unchainedCore->key(); }
bool unchainedReady() { return unchainedCore->isReady(); }
void unchainedPermission(short allowed) { unchainedCore->setPermission(allowed); }
unsigned char unchainedReset(const std::string &url) {

    LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - u:%s"), __PRETTY_FUNCTION__, __LINE__, url.c_str());
    return unchainedCore->reset(url);
}

////// Activity
unsigned char unchainedStart(const std::string &url, const std::string &version) {

    LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - u:%s; v:%s"), __PRETTY_FUNCTION__, __LINE__, url.c_str(), version.c_str());
    return unchainedCore->start(url, version);
}
#ifdef __ANDROID__
void unchainedPause(bool finishing, bool lockScreen) { unchainedCore->pause(finishing, lockScreen); }
void unchainedDestroy() {

    LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Core::freeInstance();
}
#else
void unchainedResume() { unchainedCore->resume(); }
void unchainedPause() { unchainedCore->pause(); }
#endif
void unchainedStop() {

    LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    unchainedCore->stop();
}

////// Resources
void unchainedAccel(float x, float y, float z) {

    //LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - x:%f; y:%f; z:%f"), __PRETTY_FUNCTION__, __LINE__, x, y, z);
    unchainedCore->accel(x, y, z);
}
void unchainedCamera(const unsigned char* data) { Camera::getInstance()->updateBuffer(data); }
