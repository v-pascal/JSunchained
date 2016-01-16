#ifndef UNCHAINED_GLOBAL_H_
#define UNCHAINED_GLOBAL_H_

#define UNCHAINED_VERSION_1_0_0
#define UNCHAINED_VERSION           "1.0.0"

#define UNCHAINED_NO_DATA           -1
#define UNCHAINED_NULL_STRING       " "

#ifdef __ANDROID__
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

#ifdef __ANDROID__
#include <jni.h>

extern JavaVM* g_jVM;
extern jclass g_jResClass;
extern jobject g_jResObj;

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
