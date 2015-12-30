#include "Bluetooth.h"

#ifdef __ANDROID__
#include <Unchained/Log/Log.h>
#include <Unchained/Tools/Tools.h>
#else
#include "Log.h"
#include "Tools.h"
#endif


//////
bool Bluetooth::isEnabled() {

#ifdef __ANDROID__
    LOGV(UNCHAINED_LOG_BLUETOOTH, 0, LOG_FORMAT(" - (j:%p; c:%p; o:%p)"), __PRETTY_FUNCTION__, __LINE__, g_jVM, g_jResClass,
            g_jResObj);
    assert(g_jVM);
    assert(g_jResClass);
    assert(g_jResObj);

    JNIEnv* env = getJavaEnv(UNCHAINED_LOG_BLUETOOTH, __PRETTY_FUNCTION__, __LINE__);
    if (!env)
        return false;

    jmethodID mthd = env->GetMethodID(g_jResClass, "isBluetooth", "()Z");
    if (!mthd) {

        LOGE(LOG_FORMAT(" - Failed to get 'isBluetooth' java method"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    return static_cast<bool>(env->CallBooleanMethod(g_jResObj, mthd));

#else




    //NotImplemented();




    return true;
#endif
}

bool Bluetooth::discover() {

#ifdef __ANDROID__
    LOGV(UNCHAINED_LOG_BLUETOOTH, 0, LOG_FORMAT(" - (j:%p; c:%p; o:%p)"), __PRETTY_FUNCTION__, __LINE__, g_jVM, g_jResClass,
            g_jResObj);
    assert(g_jVM);
    assert(g_jResClass);
    assert(g_jResObj);

    JNIEnv* env = getJavaEnv(UNCHAINED_LOG_BLUETOOTH, __PRETTY_FUNCTION__, __LINE__);
    if (!env)
        return false;

    jmethodID mthd = env->GetMethodID(g_jResClass, "discover", "()V");
    if (!mthd) {

        LOGE(LOG_FORMAT(" - Failed to get 'discover' java method"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    env->CallVoidMethod(g_jResObj, mthd);

#else





#endif
    return true;
}
bool Bluetooth::isDiscovering() {

#ifdef __ANDROID__
    LOGV(UNCHAINED_LOG_BLUETOOTH, 0, LOG_FORMAT(" - (j:%p; c:%p; o:%p)"), __PRETTY_FUNCTION__, __LINE__, g_jVM, g_jResClass,
            g_jResObj);
    assert(g_jVM);
    assert(g_jResClass);
    assert(g_jResObj);

    JNIEnv* env = getJavaEnv(UNCHAINED_LOG_BLUETOOTH, __PRETTY_FUNCTION__, __LINE__);
    if (!env)
        return false;

    jmethodID mthd = env->GetMethodID(g_jResClass, "isDiscovering", "()Z");
    if (!mthd) {

        LOGE(LOG_FORMAT(" - Failed to get 'isDiscovering' java method"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    return static_cast<bool>(env->CallBooleanMethod(g_jResObj, mthd));

#else





    return true;
#endif
}
std::string Bluetooth::getDevice(unsigned char index) {

    LOGV(UNCHAINED_LOG_BLUETOOTH, 0, LOG_FORMAT(" - i:%d"), __PRETTY_FUNCTION__, __LINE__, index);
    std::string device; // Error

#ifdef __ANDROID__
    LOGI(UNCHAINED_LOG_BLUETOOTH, 0, LOG_FORMAT(" - (j:%p; c:%p; o:%p)"), __PRETTY_FUNCTION__, __LINE__, g_jVM, g_jResClass,
            g_jResObj);
    assert(g_jVM);
    assert(g_jResClass);
    assert(g_jResObj);

    JNIEnv* env = getJavaEnv(UNCHAINED_LOG_STORAGE, __PRETTY_FUNCTION__, __LINE__);
    if (!env)
        return device;

    jmethodID mthd = env->GetMethodID(g_jResClass, "getDevice", "(S)Ljava/lang/String;");
    if (!mthd) {

        LOGE(LOG_FORMAT(" - Failed to get 'getDevice' java method"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return device;
    }
    jstring strDevice = static_cast<jstring>(env->CallObjectMethod(g_jResObj, mthd, static_cast<short>(index)));
    if (strDevice) {

        const char* lpDevice = env->GetStringUTFChars(strDevice, 0);
        device.assign(lpDevice);
        env->ReleaseStringUTFChars(strDevice, lpDevice);

        // Check avoid #JNI1 issue case
        if (device == " ")
            device.assign(NO_BLUETOOTH_DEVICE); // No more device
    }
#ifdef DEBUG
    else { // #JNI1 issue found

        LOGF(LOG_FORMAT(" - #JNI1 issue found"), __PRETTY_FUNCTION__, __LINE__);
        //assert(NULL); // Let's #JNI1 crash
        device.clear();
    }
#endif

#else











#endif
    return device;
}
