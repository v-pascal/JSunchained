#ifndef UNCHAINED_SENSORS_H_
#define UNCHAINED_SENSORS_H_

#include <Unchained/Global.h>

#if defined(TARGET_OS_ANDROID) || defined(TARGET_OS_WINDOWS)
#include <Unchained/Core/Reply.h>

#else
#import "NSResources.h"
#include "Reply.h"

#endif


//////
class Sensors : public Reply {

private:
    typedef struct {
        float x;
        float y;
        float z;

    } Coordinates;
    Coordinates mAccel;

public:
    Sensors(char** response);
    virtual ~Sensors();

    enum {
        TYPE_ACCEL = 0
    };

#ifdef TARGET_OS_IOS
    NSMotion* mMotion;
#endif

    //
    inline void accel(float x, float y, float z) {

        mAccel.x = x;
        mAccel.y = y;
        mAccel.z = z;
    }
    bool reply(const void* data);

};

#endif // UNCHAINED_SENSORS_H_
