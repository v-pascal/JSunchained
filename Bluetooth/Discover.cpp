#include "Discover.h"

#if defined(__ANDROID__) || defined(_WINDLL)
#include <Unchained/Features/Bluetooth/Bluetooth.h>
#else // iOS
#include "Bluetooth.h"
#endif
#include <algorithm>

#define NO_BLUETOOTH                "\"Bluetooth not available...\""
#define NO_DEVICE_FOUND             "\"No device found...\""
#define MAX_DISCOVER_REPLY          64
#define MAX_NO_BLUETOOTH_REPLY      128

// Json
#define DISCOVER_JSON               "{\"bluetooth\":{\"discovering\":%c,\"devices\":[%s]}}"


//////
Discover::Discover(char** response) : Reply(response), mDiscovering(false) {

    LOGV(UNCHAINED_LOG_DISCOVER, 0, LOG_FORMAT(" - r:%p"), __PRETTY_FUNCTION__, __LINE__, response);
}
Discover::~Discover() { LOGV(UNCHAINED_LOG_DISCOVER, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__); }

void Discover::discoveringReply() {

    LOGV(UNCHAINED_LOG_DISCOVER, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mLength = MAX_DISCOVER_REPLY;
    checkIncBuffer();
    sprintf_s(*mResponse, MAX_DISCOVER_REPLY, DISCOVER_JSON, '1', "");
    mLength = static_cast<int>(strlen(*mResponse));
}
bool Discover::reply(const void* data) {

    LOGV(UNCHAINED_LOG_DISCOVER, 0, LOG_FORMAT(" - d:%p (d:%s)"), __PRETTY_FUNCTION__, __LINE__, data,
            (mDiscovering)? "true":"false");
    if (!mDiscovering) {
        if (Bluetooth::isEnabled()) {

            Bluetooth::discover();
            mDiscovering = true;
            discoveringReply();
        }
        else {

            mLength = MAX_NO_BLUETOOTH_REPLY;
            checkIncBuffer();
            sprintf_s(*mResponse, MAX_NO_BLUETOOTH_REPLY, DISCOVER_JSON, '0', NO_BLUETOOTH);
            mLength = static_cast<int>(strlen(*mResponse));
        }
    }
    else if (Bluetooth::isDiscovering())
        discoveringReply();

    else {

        LOGI(UNCHAINED_LOG_DISCOVER, 0, LOG_FORMAT(" - Discovering has finished"), __PRETTY_FUNCTION__, __LINE__);
        mDiscovering = false;

        std::string devices;
        for (unsigned char i = 0; ; ++i) {

            std::string device(Bluetooth::getDevice(i));
            if (!device.compare(NO_BLUETOOTH_DEVICE))
                break;

            if (!devices.empty())
                devices += ',';

            devices += '\"';
            devices.append(device);
            devices += '\"';
        }
        if (devices.empty())
            devices = NO_DEVICE_FOUND;
        else
            std::replace(devices.begin(), devices.end(), '\n', ' ');
        mLength = static_cast<int>(MAX_DISCOVER_REPLY + devices.length());
        checkIncBuffer();
        sprintf_s(*mResponse, mLength, DISCOVER_JSON, '0', devices.c_str());
        mLength = static_cast<int>(strlen(*mResponse));
    }
    return true;
}
