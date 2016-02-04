#ifndef UNCHAINED_TOOLS_H_
#define UNCHAINED_TOOLS_H_
#if defined(__cplusplus)

#include <Unchained/Global.h>

#if !defined(TARGET_OS_ANDROID) && !defined(TARGET_OS_WINDOWS)
#import <Foundation/Foundation.h>
#endif

#include <limits>
#include <cmath>
#include <cstdarg>
#include <sstream>
#include <string>
#include <assert.h>
#include <time.h>
#include <float.h>

#define UNCHAINED_MEM_SCALE             16 // Multiple of 2
#define UNCHAINED_GET_MEM2ALLOC(n)      (unsigned int)((((unsigned int)(n / UNCHAINED_MEM_SCALE)) * UNCHAINED_MEM_SCALE) + \
                                            (((n % UNCHAINED_MEM_SCALE))? UNCHAINED_MEM_SCALE:0))


static const float UNCHAINED_PI = 3.14159265358979f;

//////
#ifdef TARGET_OS_WINDOWS
inline bool fequal(float a, float b) { return (fabs(a - b) <= (FLT_EPSILON * fmax(fabs(a), fabs(b)))); }
#else
inline bool fequal(float a, float b) { return (fabs(a - b) <= (FLT_EPSILON * std::max<float>(fabs(a), fabs(b)))); }
#endif

//////
template<typename T>
inline T delta(T lastVal, T newVal) {

    if ((newVal < 0) && (lastVal > 0)) {
        return (newVal - std::numeric_limits<T>::min()) + (std::numeric_limits<T>::max() - lastVal);
    }
#ifdef TARGET_OS_ANDROID
    return std::abs<T>(newVal - lastVal);
#else
    T res = newVal - lastVal;
    return (res > 0)? res:-res;
#endif
};

//////
template<typename T>
inline std::string numToStr(T number) {

    std::ostringstream oss;
    oss << number;
    return oss.str();
};
template<typename T>
inline std::wstring numToWStr(T number) {

    std::wostringstream oss;
    oss << number;
    return oss.str();
};
// WARNING: Use type size >= sizeof(short) only

template<typename T>
inline T strToNum(const std::string& str) {

    std::istringstream iss(str);
    T res;
    return (iss >> res)? res:0;
};
template<typename T>
inline T wstrToNum(const std::wstring& str) {

    std::wistringstream iss(str);
    T res;
    return (iss >> res)? res:0;
};

template<typename T>
inline std::string numToHex(T number) {

    std::stringstream res;
    res << std::hex << number;
    return res.str();
};

//////
#ifdef TARGET_OS_ANDROID
#include <jni.h>

extern "C" JNIEnv* getJavaEnv(unsigned char logLevel, const char* function, int line);
extern "C" void detachThreadJVM(unsigned char logLevel);

extern "C" wchar_t* java2wstring(const jchar* utf16, jsize len);
extern "C" jbyte* wstring2byteArray(const std::wstring& wString);

#endif

//////
#ifdef TARGET_OS_ANDROID
/*
#define UNCHAINED_UID_ERROR        "#UID-ERROR#"
#define UNCHAINED_COUNTRY_ERROR    "#CTR-ERROR#"

struct UID {
    typedef enum {
        ANDROID_ID = 0,
        DEVICE_ID
    } Type;
};
extern std::string getUID(UID::Type type, unsigned char logLevel);
extern std::string getCountry(unsigned char logLevel); // // ISO 3166-1 alpha-3
*/

#elif defined(TARGET_OS_WINDOWS)


#else
extern NSString* getCountry(); // // ISO 3166-1 alpha-2

#endif

//////
extern std::string encodeURL(const std::string& url);
extern std::string encodeB64(const std::string& field);

//////
#ifdef TARGET_OS_ANDROID
/*
class Launcher {

private:
    std::string mPackage;
    std::string mActivity;
    std::string mData;

public:
    Launcher(std::string package, std::string activity, std::string data);
    virtual ~Launcher();

    //////
    bool go();
    // WARNING: Calling this method will pause the current application

};
*/
#endif

//////
/*
#ifdef TARGET_OS_ANDROID
extern "C" bool alertMessage(unsigned char logLevel, const char* msg);
#else
extern "C" bool alertMessage(unsigned char logLevel, double duration, const char* msg);
// 'duration' in seconds
#endif
*/

#endif // __cplusplus
#endif // UNCHAINED_TOOLS_H_
