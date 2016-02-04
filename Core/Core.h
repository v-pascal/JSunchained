#ifndef UNCHAINED_CORE_H_
#define UNCHAINED_CORE_H_

#include <Unchained/Global.h>

#if defined(TARGET_OS_ANDROID) || defined(TARGET_OS_WINDOWS)
#include <Unchained/Features/Internet/Socket.h>
#include <Unchained/Sensors/Sensors.h>
#include <Unchained/Storage/Storage.h>
#include <Unchained/Bluetooth/Discover.h>
#include <Unchained/Camera/Video.h>

#else
#include "Socket.h"
#include "Sensors.h"
#include "Storage.h"
#include "Discover.h"
#include "Video.h"

#endif
#include "Permission.h"

#include <string>
#include <vector>

#define MAX_SOCKET_BUFFER   1024


//////
class Core {

private:
    Core();
    virtual ~Core();

    static Core* mThis;

protected:
    Socket* mSocket;
    typedef struct {

        char Buffer[MAX_SOCKET_BUFFER];
        unsigned short Length;

    } Connexion;
    typedef std::vector<Connexion*> Connexions;
    Connexions mConnexions;
    short mConnCount;

    void clear();

    int mPort;
    bool mRunning;
    std::string mHead;
    char* mReply;

    std::string mVersion;
    std::string mKey;

private:
    Permission mPermission;
    Sensors mSensors;
    Storage mStorage;
    Discover mDiscover;
    Video mVideo;

    volatile bool mAbort;
    volatile bool mClose;
    boost::mutex mMutex;
    boost::thread* mThread;

    void setHead(const std::string &url);
    void kill();
    void exit(bool reset = false);
    bool close(unsigned char idx);
    bool reply(int len, unsigned char idx);

    void coreThreadRunning();
    static void startCoreThread(Core* core);

public:
    static Core* getInstance() {
        if (!mThis)
            mThis = new Core;
        return mThis;
    }
    static void freeInstance() {
        if (mThis) {
            delete mThis;
            mThis = NULL;
        }
    }

    //////
    const char* key();
    inline bool isReady() const { return mRunning; }
    inline void setPermission(short allowed) {

#ifndef UNCHAINED_COMPONENT
        mPermission.setAllowed(allowed);
#else
        LOGF(LOG_FORMAT(" - No permission need for component"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
#endif
    }
    unsigned char reset(const std::string &url);

    ////// Activity
    unsigned char start(const std::string &url, const std::string &version);
#ifdef TARGET_OS_ANDROID
    void pause(bool finishing, bool lockScreen);
#else
    void resume();
    void pause();
#endif
    void stop();

    ////// Resources
    inline void accel(float x, float y, float z) { mSensors.accel(x, y, z); }

};

#endif // UNCHAINED_CORE_H_
