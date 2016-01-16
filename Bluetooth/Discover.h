#ifndef UNCHAINED_DISCOVER_H_
#define UNCHAINED_DISCOVER_H_

#if defined(__ANDROID__) || defined(_WINDLL)
#include <Unchained/Core/Reply.h>
#else // iOS
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
