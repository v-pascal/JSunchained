#ifndef UNCHAINED_H_
#define UNCHAINED_H_

#include "Global.h"
#include <string>


typedef struct {

#ifdef __ANDROID__
    JavaVM* jvm;
    jclass cls;
    jobject res;

#endif
    float xDpi;
    float yDpi;

} PlatformData;

////// Core
void unchainedInit(const PlatformData* data);
const char* unchainedKey();
bool unchainedReady();
void unchainedPermission(short allowed);
unsigned char unchainedReset(const std::string &url);

////// Activity
unsigned char unchainedStart(const std::string &url, const std::string &version);
#ifdef __ANDROID__
void unchainedPause(bool finishing, bool lockScreen);
void unchainedDestroy();
#else
void unchainedResume();
void unchainedPause();
#endif
void unchainedStop();

////// Resources
void unchainedAccel(float x, float y, float z);
void unchainedCamera(const unsigned char* data);


#endif // UNCHAINED_H_
