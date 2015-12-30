#ifndef UNCHAINED_DISCOVER_H_
#define UNCHAINED_DISCOVER_H_

#ifdef __ANDROID__
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
