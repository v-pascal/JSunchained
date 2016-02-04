#ifndef UNCHAINED_GLOBAL_H_
#define UNCHAINED_GLOBAL_H_

#define UNCHAINED_VERSION_1_0_0
#define UNCHAINED_VERSION           "1.0.0"

#define UNCHAINED_NO_DATA           -1
#define UNCHAINED_NULL_STRING       " "

#if defined(_WIN32) || defined(_WIN64)
#define TARGET_OS_WINDOWS // Universal Windows Platform
#elif defined(__ANDROID__)
#define TARGET_OS_ANDROID
#elif __APPLE__
#include "TargetConditionals.h"

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IOS)
#define TARGET_OS_IOS
#elif defined(TARGET_OS_MAC)
#define TARGET_OS_MAC
#else
#error "Unexpected target"
#endif

#elif defined(__linux__) || defined(__unix__)
#error "Not implemented yet"
#else
#error "Unexpected target"
#endif

#ifdef TARGET_OS_ANDROID
////// DEBUG | RELEASE

// Debug
#ifndef DEBUG
#define DEBUG
#endif
#undef NDEBUG

// Relase
//#ifndef NDEBUG
//#define NDEBUG
//#endif
//#undef DEBUG

// -> Not needed at compilation (only to display the code correctly)
// -> Works for all sources coz this file is the first included file (see 'Unchained.cpp')

// Needed coz the actions below do not define or undefine definitions above:
// * Following flags not working: LOCAL_CFLAGS/APP_CFLAGS/NDK_APP_CFLAGS = -DDEBUG -UNDEBUG | -DNDEBUG -UDEBUG
// * android:debuggable="false | true" in the manifest file does not work as well
// * NDK_DEBUG=0 | 1 definition has no effect

#elif defined(TARGET_OS_WINDOWS)
#ifdef _DEBUG
#define DEBUG
#endif

#endif


#define UNCHAINED_COMPONENT

////// Errors
enum {

    ERR_ID_NONE = 0,
    ERR_ID_EMPTY_URL,
    ERR_ID_EMPTY_VERSION,

    // Reserved (external)
    ERR_ID_INTERNAL,
    ERR_ID_URL_PROTOCOLE,
    ERR_ID_URL_NOT_FOUND,
    ERR_ID_URL_EXECPTION,

    // Internal
    ERR_ID_OPEN_SOCKET

};

#ifdef TARGET_OS_ANDROID
#include <jni.h>

extern JavaVM* g_jVM;
extern jclass g_jResClass;
extern jobject g_jResObj;

#elif defined(TARGET_OS_WINDOWS)
#include <string>

extern "C" {
    typedef void(__stdcall *StartCamCB)(unsigned char, short, short);
    typedef bool(__stdcall *StopCamCB)();
}

extern StartCamCB g_cbStartCam;
extern StopCamCB g_cbStopCam;

extern std::string* g_AppPath;

#endif

////// Log levels (< 5 to log)
#define UNCHAINED_LOG_MAIN          4
#define UNCHAINED_LOG_CORE          5
#define UNCHAINED_LOG_INTERNET      5
#define UNCHAINED_LOG_REPLY         5
#define UNCHAINED_LOG_PERMISSION    5
#define UNCHAINED_LOG_SENSORS       5
#define UNCHAINED_LOG_STORAGE       5
#define UNCHAINED_LOG_BLUETOOTH     5
#define UNCHAINED_LOG_DISCOVER      5
#define UNCHAINED_LOG_VIDEO         5
#define UNCHAINED_LOG_CAMERA        5


#endif // UNCHAINED_GLOBAL_H_
