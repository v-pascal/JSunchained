#include "Sensors.h"

#if defined(__ANDROID__) || defined(_WINDLL)
#include <Unchained/Log/Log.h>
#else // iOS
#include "Log.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#define MAX_RESPONSE_SIZE       64

// Json
#define ACCEL_JSON              "{\"accel\":{\"x\":%f,\"y\":%f,\"z\":%f}}"


//////
Sensors::Sensors(char** response) : Reply(response) {

    LOGV(UNCHAINED_LOG_SENSORS, 0, LOG_FORMAT(" - r:%p"), __PRETTY_FUNCTION__, __LINE__, response);
    memset(&mAccel, 0, sizeof(Coordinates));

#if !defined(__ANDROID__) && !defined(_WINDLL) // iOS
    mMotion = [[NSMotion alloc] init];
#endif
}
Sensors::~Sensors() { LOGV(UNCHAINED_LOG_SENSORS, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__); }

bool Sensors::reply(const void* data) {

    mLength = MAX_RESPONSE_SIZE;
    checkIncBuffer();
    switch (*static_cast<const unsigned char*>(data)) {

        case TYPE_ACCEL: {
            sprintf_s(*mResponse, MAX_RESPONSE_SIZE, ACCEL_JSON, mAccel.x, mAccel.y, mAccel.z);
            mLength = static_cast<int>(strlen(*mResponse));
            break;
        }
#ifdef DEBUG
        default: {

            LOGE(LOG_FORMAT(" - Unexpected sensor type: %d"), __PRETTY_FUNCTION__, __LINE__,
                    *static_cast<const unsigned char*>(data));
            assert(NULL);
            return false;
        }
#endif
    }
    return true;
}
