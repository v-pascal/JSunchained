#ifndef UNCHAINED_DISCOVER_H_
#define UNCHAINED_DISCOVER_H_

#include <Unchained/Global.h>

#if defined(TARGET_OS_ANDROID) || defined(TARGET_OS_WINDOWS)
#include <Unchained/Core/Reply.h>
#else
#include "Reply.h"
#endif


//////
class Discover : public Reply {

private:
    bool mDiscovering;

    void discoveringReply();

public:
    Discover(char** response);
    virtual ~Discover();

    //
    bool reply(const void* data);

};

#endif // UNCHAINED_DISCOVER_H_
