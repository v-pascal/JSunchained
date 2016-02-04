#ifndef UNCHAINED_H_
#define UNCHAINED_H_

#include "Global.h"
#include <string>

#ifdef TARGET_OS_WINDOWS

#ifdef UNCHAINED_EXPORTS
#define UNCHAINED_API	__declspec(dllexport)
#else
#define UNCHAINED_API	__declspec(dllimport)
#endif

#else
#define UNCHAINED_API
#endif

#ifdef TARGET_OS_WINDOWS
extern "C" {
#endif

typedef struct {

#ifdef TARGET_OS_ANDROID
    JavaVM* jvm;
    jclass cls;
    jobject res;

#endif
    float xDpi;
    float yDpi;

#ifdef TARGET_OS_WINDOWS
    StartCamCB startCam;
    StopCamCB stopCam;

    const char* appPath;
#endif

} PlatformData;

////// Core
UNCHAINED_API void unchainedInit(const PlatformData* data);
UNCHAINED_API const char* unchainedKey();
UNCHAINED_API bool unchainedReady();
UNCHAINED_API void unchainedPermission(short allowed);
#ifdef TARGET_OS_WINDOWS
UNCHAINED_API unsigned char unchainedReset(const char* url);
#else
unsigned char unchainedReset(const std::string &url);
#endif

////// Activity
#ifdef TARGET_OS_WINDOWS
UNCHAINED_API unsigned char unchainedStart(const char* url, const char* version);
#else
unsigned char unchainedStart(const std::string &url, const std::string &version);
#endif
#ifdef TARGET_OS_ANDROID
void unchainedPause(bool finishing, bool lockScreen);
void unchainedDestroy();
#else
UNCHAINED_API void unchainedResume();
UNCHAINED_API void unchainedPause();
#endif
UNCHAINED_API void unchainedStop();

////// Resources
UNCHAINED_API void unchainedAccel(float x, float y, float z);
UNCHAINED_API void unchainedCamera(const unsigned char* data);

#ifdef TARGET_OS_WINDOWS
}
#endif

#endif // UNCHAINED_H_
