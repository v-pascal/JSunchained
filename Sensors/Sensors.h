#ifndef UNCHAINED_SENSORS_H_
#define UNCHAINED_SENSORS_H_

#ifdef __ANDROID__
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
    
#ifndef __ANDROID__
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
