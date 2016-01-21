#include "Unchained.h"

#if defined(__ANDROID__) || defined(_WINDLL)
#ifdef __ANDROID__
#include <jni.h>
#endif
#include <Unchained/Log/Log.h>
#include <Unchained/Core/Core.h>

#else // iOS
#include "Log.h"
#include "Core.h"

#import <libGST/libGST.h>

#endif


#ifdef __ANDROID__
JavaVM* g_jVM = NULL;
jclass g_jResClass = NULL;
jobject g_jResObj = NULL;

#elif defined(_WINDLL)
StartCamCB g_cbStartCam = NULL;
StopCamCB g_cbStopCam = NULL;

#endif

Core* unchainedCore = NULL;

#ifdef _WINDLL
extern "C" {
#endif

////// Core
UNCHAINED_API void unchainedInit(const PlatformData* data) {

	LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - d:%p"), __PRETTY_FUNCTION__, __LINE__, data);
#ifdef __ANDROID__
	g_jVM = data->jvm;
	g_jResClass = data->cls;
	g_jResObj = data->res;

#elif defined(_WINDLL)
    g_cbStartCam = data->startCam;
    g_cbStopCam = data->stopCam;

#else
	lib_gst_init();
#endif
	unchainedCore = Core::getInstance();
}
UNCHAINED_API const char* unchainedKey() { return unchainedCore->key(); }
UNCHAINED_API bool unchainedReady() { return unchainedCore->isReady(); }
UNCHAINED_API void unchainedPermission(short allowed) { unchainedCore->setPermission(allowed); }
UNCHAINED_API unsigned char unchainedReset(const std::string &url) {

	LOGV(UNCHAINED_LOG_MAIN, 0, LOG_FORMAT(" - u:%s"), __PRETTY_FUNCTION__, __LINE__, url.c_str());
	return unchainedCore->reset(url);
}

////// Activity
UNCHAINED_API unsigned char unchainedStart(const std::string &url, const std::string &version) {

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
UNCHAINED_API void unchainedCamera(const unsigned char* data) { Camera::getInstance()->updateBuffer(data); }

#ifdef _WINDLL
}
#endif
