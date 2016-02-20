#include "Unchained.h"

#if defined(TARGET_OS_ANDROID) || defined(TARGET_OS_WINDOWS)
#ifdef TARGET_OS_ANDROID
#include <jni.h>
#endif
#include <Unchained/Log/Log.h>
#include <Unchained/Core/Core.h>

#else
#include "Log.h"
#include "Core.h"

#ifdef TARGET_OS_IOS
#import <libGST/libGST.h>
#endif

#endif


#ifdef TARGET_OS_ANDROID
JavaVM* g_jVM = NULL;
jclass g_jResClass = NULL;
jobject g_jResObj = NULL;

#elif defined(TARGET_OS_WINDOWS)
StartCamCB g_cbStartCam = NULL;
StopCamCB g_cbStopCam = NULL;

std::string* g_AppPath = NULL;

#endif

Core* unchainedCore = NULL;

#ifdef TARGET_OS_WINDOWS
extern "C" {
#endif

////// Core
UNCHAINED_API void unchainedInit(const PlatformData* data) {

	LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - d:%p"), __PRETTY_FUNCTION__, __LINE__, data);
#ifdef TARGET_OS_ANDROID
	g_jVM = data->jvm;
	g_jResClass = data->cls;
	g_jResObj = data->res;

#elif defined(TARGET_OS_WINDOWS)
    g_cbStartCam = data->startCam;
    g_cbStopCam = data->stopCam;

    g_AppPath = new std::string(data->appPath);

#elif defined(TARGET_OS_IOS)
	lib_gst_init();
#endif
	unchainedCore = Core::getInstance();
}
UNCHAINED_API const char* unchainedKey() { return unchainedCore->key(); }
UNCHAINED_API bool unchainedReady() { return unchainedCore->isReady(); }
UNCHAINED_API void unchainedPermission(short allowed) { unchainedCore->setPermission(allowed); }
#ifdef TARGET_OS_WINDOWS
UNCHAINED_API unsigned char unchainedReset(const char* url) {

    LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - u:%s"), __PRETTY_FUNCTION__, __LINE__, url);
#else
unsigned char unchainedReset(const std::string &url) {

    LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - u:%s"), __PRETTY_FUNCTION__, __LINE__, url.c_str());
#endif
	return unchainedCore->reset(url);
}

////// Activity
#ifdef TARGET_OS_WINDOWS
UNCHAINED_API unsigned char unchainedStart(const char* url, const char* version) {

    LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - u:%s; v:%s"), __PRETTY_FUNCTION__, __LINE__, url, version);
#else
unsigned char unchainedStart(const std::string &url, const std::string &version) {

    LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - u:%s; v:%s"), __PRETTY_FUNCTION__, __LINE__, url.c_str(), version.c_str());
#endif
    return unchainedCore->start(url, version);
}
#ifdef TARGET_OS_ANDROID
void unchainedPause(bool finishing, bool lockScreen) { unchainedCore->pause(finishing, lockScreen); }
void unchainedDestroy() {

	LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
	Core::freeInstance();
}
#else
UNCHAINED_API void unchainedResume() { unchainedCore->resume(); }
UNCHAINED_API void unchainedPause() { unchainedCore->pause(); }
#endif
UNCHAINED_API void unchainedStop() {

	LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    unchainedCore->stop();
}

////// Resources
UNCHAINED_API void unchainedAccel(float x, float y, float z) {

	//LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - x:%f; y:%f; z:%f"), __PRETTY_FUNCTION__, __LINE__, x, y, z);
	unchainedCore->accel(x, y, z);
}
UNCHAINED_API void unchainedCamera(const unsigned char* data) {

    //LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - d:%p"), __PRETTY_FUNCTION__, __LINE__, data);
#ifdef TARGET_OS_WINDOWS
    if (!data) {

        LOGF(LOG_FORMAT(" - Failed to start camera"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL); // Not implement yet // TODO: Stop camera from JavaScript library
        return;
    }
#endif
    Camera::getInstance()->updateBuffer(data);
}

#ifdef TARGET_OS_WINDOWS
}
#endif
