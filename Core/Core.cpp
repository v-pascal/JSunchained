#include "Core.h"

#if defined(TARGET_OS_ANDROID) || defined(TARGET_OS_WINDOWS)
#include <Unchained/Log/Log.h>
#include <Unchained/Tools/Tools.h>

#else
#include "Log.h"
#include "Tools.h"

#endif

#define PORT_MIN            1024
#define PORT_MAX            1124

#define HTTP_REPLY_CLOSE    "\nContent-Type: text/xml; charset=utf-8\nContent-Length: 5\n\nclose"
#define HTTP_HEAD_BODY      "\nContent-Type: text/xml; charset=utf-8\n\n"


Core* Core::mThis = NULL;

//////
Core::Core() : mThread(NULL), mAbort(false), mPort(0), mClose(false), mRunning(false), mPermission(&mReply),
        mSensors(&mReply), mStorage(&mReply), mDiscover(&mReply), mVideo(&mReply), mConnCount(1) {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mReply = NULL;
    mSocket = new Socket(true);
}
Core::~Core() {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (mThread) {

        mAbort = true;
        mThread->join();
        delete mThread;
    }
    if (mSocket)
        delete mSocket;
    clear();
    if (mReply)
        delete [] mReply;
}

void Core::clear() {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mSensors.accel(0.f, 0.f, 0.f);
    mRunning = false;
    mClose = false;
    mAbort = false;
    for (Connexions::iterator iter = mConnexions.begin(); iter != mConnexions.end(); ++iter)
        delete (*iter);
    mConnexions.clear();
}
void Core::setHead(const std::string &url) {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - u:%s"), __PRETTY_FUNCTION__, __LINE__, url.c_str());
    mHead = HTTP_REPLY_HEAD;
#ifdef TARGET_OS_ANDROID
    mHead.append(url);
#else
    size_t pos = url.find('/', sizeof("http://"));
    if (pos != std::string::npos)
        mHead.append(url.substr(0, pos));
    else
        mHead.append(url);
#endif
}

const char* Core::key() {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    int random = std::rand();
    mKey = numToHex<int>(random);

    time_t now = time(NULL) - (5 * 24 * 60 * 60);
#ifdef TARGET_OS_WINDOWS
    struct tm* date = NULL;
    gmtime_s(date, &now);
#else
    struct tm* date = gmtime(&now);
#endif
    mKey += (date->tm_sec % 10) | 0x30;
    mKey += '_';

    std::string tmp;
    tmp += (((date->tm_year % 100) - (date->tm_year % 10)) / 10) | 0x30;
    tmp += (date->tm_min % 10) | 0x30;
    tmp += ((date->tm_min - (date->tm_min % 10)) / 10) | 0x30;
    tmp += (date->tm_hour % 10) | 0x30;
    tmp += (date->tm_mon % 10) | 0x30;
    tmp += ((date->tm_mon - (date->tm_mon % 10)) / 10) | 0x30;
    tmp += (date->tm_mday % 10) | 0x30;
    tmp += ((date->tm_mday - (date->tm_mday % 10)) / 10) | 0x30;
    tmp += (date->tm_year % 10) | 0x30; // Year 2100 limited!
    int prod = date->tm_sec % 10;
    if (!prod)
        prod = date->tm_sec / 10;
    if (!prod)
        prod = 7;
    mKey.append(numToHex<int>(prod * strToNum<int>(tmp)));
    mKey += ((date->tm_hour - (date->tm_hour % 10)) / 10) | 0x30;
    mKey += '_';

    LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Date: %s"), __PRETTY_FUNCTION__, __LINE__, tmp.c_str());
    mKey.append(numToHex<int>(random / prod));
    mKey += ((date->tm_sec - (date->tm_sec % 10)) / 10) | 0x30;
    mKey += '_';

    mKey.append(mVersion);

    LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Key: %s"), __PRETTY_FUNCTION__, __LINE__, mKey.c_str());
    mKey = encodeURL(encodeB64(mKey));

    LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - URL: %s"), __PRETTY_FUNCTION__, __LINE__, mKey.c_str());
    return mKey.c_str();
}
unsigned char Core::reset(const std::string &url) {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - u:%s"), __PRETTY_FUNCTION__, __LINE__, url.c_str());
    mMutex.lock();
    exit(true);
    while (mSocket->getClientCount())
        close(0);

    setHead(url);
    if (Camera::getInstance()->isStarted())
        Camera::getInstance()->stop();
    mMutex.unlock();

    return ERR_ID_NONE;
}
unsigned char Core::start(const std::string &url, const std::string &version) {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - u:%s; v:%s"), __PRETTY_FUNCTION__, __LINE__, url.c_str(), version.c_str());
    mVersion = (!version.compare(UNCHAINED_NULL_STRING))? "X":version;

    size_t pos = mVersion.find('.');
    while (pos != std::string::npos) {

        mVersion.replace(pos, 1, 1, '_');
        pos = mVersion.find('.');
    }
    setHead(url);
    if (!mSocket->open()) {

        LOGE(LOG_FORMAT(" - Failed to open socket"), __PRETTY_FUNCTION__, __LINE__);
        return ERR_ID_OPEN_SOCKET;
    }
    clear();
    mThread = new boost::thread(startCoreThread, this);
    return ERR_ID_NONE;
}
#ifdef TARGET_OS_ANDROID
void Core::pause(bool finishing, bool lockScreen) {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - f:%s; l:%s"), __PRETTY_FUNCTION__, __LINE__, (finishing)? "true":"false",
            (lockScreen)? "true":"false");
    if (!finishing)
        Camera::getInstance()->pause(lockScreen);
}
#else // !Android
void Core::resume() {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
#ifdef TARGET_OS_IOS
    [mSensors.mMotion resume];
#endif
    Camera::getInstance()->resume();
}
void Core::pause() {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
#ifdef TARGET_OS_IOS
    [mSensors.mMotion pause];
#endif
    Camera::getInstance()->pause();
}
#endif
void Core::stop() {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
#ifdef TARGET_OS_IOS
    [mSensors.mMotion stop];
#endif
    mClose = true;
}

//
void Core::kill() {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    exit(true);
    delete mSocket;
    mSocket = NULL;

    mClose = true;
    mAbort = true;
}
void Core::exit(bool reset) {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - (c:%s; r:%s)"), __PRETTY_FUNCTION__, __LINE__, (mClose)? "true":"false",
            (reset)? "true":"false");
    assert(mClose || reset);

    std::string reply(mHead);
    reply.append(HTTP_REPLY_CLOSE);
    for (short i = (mSocket->getClientCount() - 1); i > UNCHAINED_NO_DATA; --i) {

        if (mSocket->Send(reply.c_str(), reply.length(), i) != reply.length()) {
            LOGE(LOG_FORMAT(" - Failed to close connection"), __PRETTY_FUNCTION__, __LINE__);
        }
    }
    mAbort = !reset;
}
bool Core::close(unsigned char idx) {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - i:%d"), __PRETTY_FUNCTION__, __LINE__, idx);
    mSocket->closeClient(idx);
    delete mConnexions[idx];
    mConnexions.erase(mConnexions.begin() + idx);
    return true;
}
bool Core::reply(int len, unsigned char idx) {

    //LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - l:%d; i:%d"), __PRETTY_FUNCTION__, __LINE__, len, idx);
    int sent = 0;
    while (len > 0) {

        int packetLen = (len > MAX_SOCKET_BUFFER)? MAX_SOCKET_BUFFER:len;
        if (mSocket->Send(mReply + sent, packetLen, idx) != packetLen)
            return false;

        if (len > MAX_SOCKET_BUFFER)
            boost::this_thread::sleep(boost::posix_time::microseconds(100));

        sent += MAX_SOCKET_BUFFER;
        len -= MAX_SOCKET_BUFFER;
    }
    return true;
}

void Core::coreThreadRunning() {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Start"), __PRETTY_FUNCTION__, __LINE__);
    for (short i = PORT_MIN; ; ) {
        if (mSocket->start(static_cast<int>(i))) {

            LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Listening port %d"), __PRETTY_FUNCTION__, __LINE__, i);
            mPort = static_cast<int>(i);
            break;
        }
        LOGW(LOG_FORMAT(" - Try to listen port %d failed"), __PRETTY_FUNCTION__, __LINE__, i);
        if (++i > PORT_MAX)
            i = PORT_MIN;
    }
    mRunning = true;
    mStorage.init();

    mMutex.lock();
    while (!mAbort) {

        mMutex.unlock();
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        mMutex.lock();
        if (!mSocket->getClientCount())
            continue;

        unsigned char count = mSocket->getClientCount();
        /*
        if (count > mConnCount) {

            LOGF(LOG_FORMAT(" - Security issue found: processus stopped!"), __PRETTY_FUNCTION__, __LINE__);
            kill();
            break;
        }
        */
        while (count != static_cast<unsigned char>(mConnexions.size())) {

            LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - New connexion"), __PRETTY_FUNCTION__, __LINE__);
            Connexion* connexion = new Connexion;
            connexion->Length = 0;
            mConnexions.push_back(connexion);
        }
        for (unsigned char i = 0; i < count; ++i)  {

            char rcvBuffer[MAX_SOCKET_BUFFER];
            int rcvLen = static_cast<int>(mSocket->receive(rcvBuffer, MAX_SOCKET_BUFFER - 1, i));
            if (rcvLen < 0) {

                bool quit = false;
#ifdef TARGET_OS_WINDOWS
                int err = WSAGetLastError();
                switch (err) {
                    case WSAEWOULDBLOCK: {
#else
                int err = errno;
                switch (err) {
                    case EWOULDBLOCK: { // EAGAIN
#endif
                        if (!mConnexions[i]->Length)
                            break; // No data received

                        if (mClose) {

                            exit();
                            quit = true;
                            break;
                        }
                        Reply* replyObj = NULL;
                        const char* httpBody = NULL;






                        // Check "Origin: http://vp.magellan.free.fr" in the HTTP get method same as 'mHead'
                        // "Access-Control-Allow-Origin:" attribute. If not don't take care of the
                        // request & close(i)






                        bool replyRes = false;
                        if (mConnexions[i]->Buffer[0] == 'H') { // HEAD http request method (to validate the connection)

                            std::string headReply(mHead);
                            headReply.append(HTTP_HEAD_BODY);
                            if (mSocket->Send(headReply.c_str(), headReply.length(), i) != headReply.length()) {

                                LOGE(LOG_FORMAT(" - Failed to send head reply (%d)"), __PRETTY_FUNCTION__, __LINE__, i);
                                quit = close(i);
                            }
                            else
                                mConnexions[i]->Length = 0;
                            break;
                        }
                        else if ((mConnexions[i]->Buffer[sizeof(HTTP_GET) - 1] == '0') ||
                                 (mConnexions[i]->Buffer[sizeof(HTTP_GET) - 1] == '1')) { // /0/[0;n] || /1/[0;n] connCount

                            ////// Sensors request
                            unsigned char type = Sensors::TYPE_ACCEL;
                            replyRes = mSensors.reply(&type);
                            replyObj = &mSensors;
                            httpBody = HTTP_REPLY_JSON;

                            //std::string connCount(&mConnexions[i]->Buffer[sizeof(HTTP_GET) + 1]);
                            //mConnCount = strToNum<short>(connCount);
                        }
                        else if (mConnexions[i]->Buffer[sizeof(HTTP_GET) - 1] == 'p') { // /permission/*_0 || 1

                            ////// Permission request
                            LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Permission requested (%d)"), __PRETTY_FUNCTION__,
                                    __LINE__, i);
                            Permission::Info info;
                            std::string mask(mConnexions[i]->Buffer, sizeof(HTTP_GET) + 10, 11);
                            info.perm = strToNum<unsigned int>(mask);
                            info.url = &mHead;
                            replyRes = mPermission.reply(&info);
                            replyObj = &mPermission;
                            httpBody = HTTP_REPLY_JSON;
                        }
                        else if (mConnexions[i]->Buffer[sizeof(HTTP_GET) - 1] == 's') { // /storage/x.jpg [0;MAX_INT]

                            ////// Storage request (return all JPG pictures contained in 'Pictures' folder)
                            LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Picture requested (%d)"), __PRETTY_FUNCTION__,
                                    __LINE__, i);
                            replyRes = mStorage.reply(NULL);
                            replyObj = &mStorage;
                            httpBody = mStorage.getBody();
                        }
                        else if (mConnexions[i]->Buffer[sizeof(HTTP_GET) - 1] == 'b') { // /bluetooth/0 || /bluetooth/1

                            ////// Bluetooth request
                            LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Bluetooth request (%d)"), __PRETTY_FUNCTION__,
                                    __LINE__, i);
                            replyRes = mDiscover.reply(NULL);
                            replyObj = &mDiscover;
                            httpBody = HTTP_REPLY_JSON;
                        }
                        else if (mConnexions[i]->Buffer[sizeof(HTTP_GET) - 1] == 'c') {

                            ////// Camera request
                            unsigned char operation;
                            switch (mConnexions[i]->Buffer[sizeof(HTTP_GET_CAMERA) - 1]) {
                                case 'a': { // Start: /camera/start0 || /camera/start1

                                    LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Start camera requested (%d)"),
                                            __PRETTY_FUNCTION__, __LINE__, i);
                                    operation = Video::REQ_START;
                                    httpBody = HTTP_REPLY_JSON;
                                    break;
                                }
                                case 'o': { // Stop: /camera/stop0 || /camera/stop1

                                    LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Stop camera requested (%d)"),
                                            __PRETTY_FUNCTION__, __LINE__, i);
                                    operation = Video::REQ_STOP;
                                    httpBody = HTTP_REPLY_JSON;
                                    break;
                                }
                                default: { // Frame: /camera/x.jpg [0;MAX_INT]

                                    operation = Video::REQ_FRAME;
                                    httpBody = HTTP_REPLY_JPEG;
                                    break;
                                }
                            }
                            replyRes = mVideo.reply(&operation);
                            replyObj = &mVideo;
                        }

                        // Send reply
                        if (replyRes) {

                            std::string response(mHead);
                            response.append(httpBody);
                            response.append(numToStr<int>(static_cast<int>(replyObj->getLength())));
                            response += '\n';
                            response += '\n';
                            if ((mSocket->Send(response.c_str(), response.length(), i) != response.length()) ||
                                    (!reply(replyObj->getLength(), i))) {

                                LOGE(LOG_FORMAT(" - Failed to send reply (%d)"), __PRETTY_FUNCTION__, __LINE__, i);
                                quit = close(i);
                            }
                            else
                                mConnexions[i]->Length = 0;

                            replyObj->reset();
                        }
                        if ((!replyRes) && (!quit))
                            quit = close(i);
                        break;
                    }
                    default: {

                        LOGE(LOG_FORMAT(" - Receiving data error: %d (%d)"), __PRETTY_FUNCTION__, __LINE__, err, i);
                        quit = close(i);
                        break;
                    }
                }
                if (quit)
                    break;

                continue;
            }
            if (!rcvLen) {

#ifdef DEBUG
                LOGW(LOG_FORMAT(" - Unexpected socket close (%d)"), __PRETTY_FUNCTION__, __LINE__, i);
#endif
                close(i);
                break;
            }
            memcpy(mConnexions[i]->Buffer + mConnexions[i]->Length, rcvBuffer, rcvLen);
            mConnexions[i]->Length += rcvLen;
        }
    }
    mMutex.unlock();

    if ((!mClose) && (mSocket->getClientCount()))
        exit();

#ifdef TARGET_OS_ANDROID
    detachThreadJVM(UNCHAINED_LOG_CORE);
#endif
    LOGI(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - Finished"), __PRETTY_FUNCTION__, __LINE__);
}
void Core::startCoreThread(Core* core) {

    LOGV(UNCHAINED_LOG_CORE, 0, LOG_FORMAT(" - c:%p"), __PRETTY_FUNCTION__, __LINE__, core);
    core->coreThreadRunning();
}
