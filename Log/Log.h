#ifndef UNCHAINED_LOG_H_
#define UNCHAINED_LOG_H_

#include <Unchained/Global.h>
#ifdef TARGET_OS_WINDOWS
#define __PRETTY_FUNCTION__                 __FUNCSIG__
#define LOG_FORMAT(param)                   "%s (%s): %s[%d]" param "\n"
#else
#define LOG_FORMAT(param)                   "%s[%d]" param
#endif
#define LOG_LEVEL                           5  // 4 level to add log (always log: LOGW, LOGE & LOGF)
                                               // Possible use: LOGI(LOG_LEVEL_MODULE + n, ... );
typedef enum {

#ifdef DEBUG
    LOG_UNKNOWN,    // UNKNOWN: Log at unexpected exception (such as unknown exception)
    LOG_DEFAULT,    // DEFAULT: Log unexpected default option (switch)
    LOG_VERBOSE,    // VERBOSE: Log at each funtion/method entries
    LOG_DEBUG,      // DEBUG: Log elapsed time
    LOG_INFO,       // INFO: Log infos in the code
    LOG_SILENT,     // SILENT: Shut!?!
#endif
    LOG_WARNING,    // WARNING: ...
    LOG_ERROR,      // ERROR: ...
    LOG_FATAL       // FATAL: ...

} LogType;

// mod: Modulo of value that increase to avoid too many logs: !(value % n) -> log

#ifdef TARGET_OS_ANDROID
#include <android/log.h>

#ifdef DEBUG
#define LOGU(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { __android_log_print(ANDROID_LOG_UNKNOWN, "JSunchained", format, __VA_ARGS__); }
#define LOGD(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { __android_log_print(ANDROID_LOG_DEFAULT, "JSunchained", format, __VA_ARGS__); }
#define LOGV(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { __android_log_print(ANDROID_LOG_VERBOSE, "JSunchained", format, __VA_ARGS__); }
#define LOGDE(level, mod, format, ...)      if ((level < LOG_LEVEL) && (!mod)) \
                                            { __android_log_print(ANDROID_LOG_DEBUG, "JSunchained", format, __VA_ARGS__); }
#define LOGI(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { __android_log_print(ANDROID_LOG_INFO, "JSunchained", format, __VA_ARGS__); }
#define LOGS(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { __android_log_print(ANDROID_LOG_SILENT, "JSunchained", format, __VA_ARGS__); }

#else
#define LOGU(level, mod, format, ...)       ((void)0)
#define LOGD(level, mod, format, ...)       ((void)0)
#define LOGV(level, mod, format, ...)       ((void)0)
#define LOGDE(level, mod, format, ...)      ((void)0)
#define LOGI(level, mod, format, ...)       ((void)0)
#define LOGS(level, mod, format, ...)       ((void)0)

#endif
#define LOGW(format, ...)                   __android_log_print(ANDROID_LOG_WARN, "JSunchained", format, __VA_ARGS__);
#define LOGE(format, ...)                   __android_log_print(ANDROID_LOG_ERROR, "JSunchained", format, __VA_ARGS__);
#define LOGF(format, ...)                   __android_log_print(ANDROID_LOG_FATAL, "JSunchained", format, __VA_ARGS__);

#elif defined(TARGET_OS_WINDOWS)
#include <stdio.h>
#include <windows.h>

#define MAX_LOG_SIZE                        1024

#ifdef DEBUG
#define LOGU(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { char log[MAX_LOG_SIZE]={0}; \
                                              sprintf_s(log, MAX_LOG_SIZE, format, "UNKNOWN", "JSunchained", __VA_ARGS__); \
                                              OutputDebugStringA(log); }
#define LOGD(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { char log[MAX_LOG_SIZE]={0}; \
                                              sprintf_s(log, MAX_LOG_SIZE, format, "DEFAULT", "JSunchained", __VA_ARGS__); \
                                              OutputDebugStringA(log); }
#define LOGV(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { char log[MAX_LOG_SIZE]={0}; \
                                              sprintf_s(log, MAX_LOG_SIZE, format, "VERBOSE", "JSunchained", __VA_ARGS__); \
                                              OutputDebugStringA(log); }
#define LOGDE(level, mod, format, ...)      if ((level < LOG_LEVEL) && (!mod)) \
                                            { char log[MAX_LOG_SIZE]={0}; \
                                              sprintf_s(log, MAX_LOG_SIZE, format, "DEBUG", "JSunchained", __VA_ARGS__); \
                                              OutputDebugStringA(log); }
#define LOGI(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { char log[MAX_LOG_SIZE]={0}; \
                                              sprintf_s(log, MAX_LOG_SIZE, format, "INFO", "JSunchained", __VA_ARGS__); \
                                              OutputDebugStringA(log); }
#define LOGS(level, mod, format, ...)       if ((level < LOG_LEVEL) && (!mod)) \
                                            { char log[MAX_LOG_SIZE]={0}; \
                                              sprintf_s(log, MAX_LOG_SIZE, format, "SILENT", "JSunchained", __VA_ARGS__); \
                                              OutputDebugStringA(log); }

#else
#define LOGU(level, mod, format, ...)       ((void)0)
#define LOGD(level, mod, format, ...)       ((void)0)
#define LOGV(level, mod, format, ...)       ((void)0)
#define LOGDE(level, mod, format, ...)      ((void)0)
#define LOGI(level, mod, format, ...)       ((void)0)
#define LOGS(level, mod, format, ...)       ((void)0)

#endif
#define LOGW(format, ...)                   { char log[MAX_LOG_SIZE]={0}; \
                                              sprintf_s(log, MAX_LOG_SIZE, format, "WARNING", "JSunchained", __VA_ARGS__); \
                                              OutputDebugStringA(log); }
#define LOGE(format, ...)                   { char log[MAX_LOG_SIZE]={0}; \
                                              sprintf_s(log, MAX_LOG_SIZE, format, "ERROR", "JSunchained", __VA_ARGS__); \
                                              OutputDebugStringA(log); }
#define LOGF(format, ...)                   { char log[MAX_LOG_SIZE]={0}; \
                                              sprintf_s(log, MAX_LOG_SIZE, format, "FATAL", "JSunchained", __VA_ARGS__); \
                                              OutputDebugStringA(log); }

#else

#ifdef DEBUG
extern "C" {

void LOGU(short level, unsigned int mod, const char* format, ...);
void LOGD(short level, unsigned int mod, const char* format, ...);
void LOGV(short level, unsigned int mod, const char* format, ...);
void LOGDE(short level, unsigned int mod, const char* format, ...);
void LOGI(short level, unsigned int mod, const char* format, ...);
void LOGS(short level, unsigned int mod, const char* format, ...);

} // extern "C"

#else
#define LOGU(level, mod, format, ...)       ((void)0)
#define LOGD(level, mod, format, ...)       ((void)0)
#define LOGV(level, mod, format, ...)       ((void)0)
#define LOGDE(level, mod, format, ...)      ((void)0)
#define LOGI(level, mod, format, ...)       ((void)0)
#define LOGS(level, mod, format, ...)       ((void)0)

#endif
extern "C" {

void LOGW(const char* format, ...);
void LOGE(const char* format, ...);
void LOGF(const char* format, ...);

} // extern "C"

#endif

#endif // UNCHAINED_LOG_H_
