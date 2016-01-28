#include "Socket.h"

#if defined(__ANDROID__) || defined(_WINDLL)
#include <Unchained/Log/Log.h>
#include <Unchained/Tools/Tools.h>

#else // iOS
#include "Log.h"
#include "Tools.h"

#endif

#ifdef _WINDLL
#include <ws2tcpip.h>

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#endif

#ifdef _WINDLL
#undef UNCHAINED_NO_DATA
#define UNCHAINED_NO_DATA       INVALID_SOCKET
#endif

#define MAX_CLIENT_COUNT        254
#define ACCEPT_DELAY            500 // Half a second


//////
Socket::Socket(bool server) : mServer(server), mThread(NULL), mSocket(UNCHAINED_NO_DATA), mAbort(true) {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - s:%s"), __PRETTY_FUNCTION__, __LINE__, (server)? "true":"false");
}
Socket::~Socket() {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    shutdown();
}

bool Socket::open() {
    
    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - (s:%d)"), __PRETTY_FUNCTION__, __LINE__, mSocket);
    assert(mSocket == UNCHAINED_NO_DATA);

#ifdef _WINDLL
    mSocket = WSAStartup(MAKEWORD(2, 2), &mWSAD);
    if (mSocket != 0) {

        LOGE(LOG_FORMAT(" - WSAStartup failed with error: %d"), __PRETTY_FUNCTION__, __LINE__, mSocket);
        mSocket = UNCHAINED_NO_DATA;
        return false;
    }
#endif
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WINDLL
    if (mSocket == INVALID_SOCKET) {
#else
    if (mSocket < 0) {
#endif
        LOGE(LOG_FORMAT(" - Failed to open socket (%d)"), __PRETTY_FUNCTION__, __LINE__, mSocket);
        mSocket = UNCHAINED_NO_DATA;
#ifdef _WINDLL
        WSACleanup();
#endif
        return false;
    }
#ifdef _WINDLL
    u_long blockMode = 0;
    if (mServer)
        blockMode = 1; // Non-blocking

    ioctlsocket(mSocket, FIONBIO, &blockMode);
#else
    if (mServer)
        fcntl(mSocket, F_SETFL, fcntl(mSocket, F_GETFL, 0) | O_NONBLOCK);
    else
        fcntl(mSocket, F_SETFL, fcntl(mSocket, F_GETFL, 0) & ~O_NONBLOCK);
#endif
    return true;
}
void Socket::shutdown() {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - (t:%p)"), __PRETTY_FUNCTION__, __LINE__, mThread);
    if (mThread) {

        mAbort = true;
        mThread->join();
        delete mThread;
        mThread = NULL;
    }
    for (std::vector<int>::iterator iter = mClients.begin(); iter != mClients.end(); ++iter)
#ifdef _WINDLL
        closesocket(*iter);
#else
        close(*iter);
#endif
    mClients.clear();

#ifndef _WINDLL
    if (!(mSocket < 0))
        close(mSocket);
#else
    if (mSocket != INVALID_SOCKET)
        closesocket(mSocket);

    WSACleanup();
#endif
    mSocket = UNCHAINED_NO_DATA;
}

int Socket::receive(char* buffer, size_t max, unsigned char clientIdx) const {

#ifdef _WINDLL
    if (mSocket == INVALID_SOCKET) {
#else
    if (mSocket < 0) {
#endif
        LOGW(LOG_FORMAT(" - Socket not opened"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return UNCHAINED_NO_DATA;
    }
    assert((!mServer) || ((mServer) && (mClients.size() > clientIdx)));
    int socket = (!mServer)? mSocket:mClients[clientIdx];
#ifdef _WINDLL
    return recv(socket, buffer, max, 0);
#else
    return static_cast<int>(read(socket, buffer, max));
#endif
}
int Socket::Send(const char* buffer, size_t len, unsigned char clientIdx) const {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - b:%p; s:%d; i:%d (s:%s; l:%d)"), __PRETTY_FUNCTION__, __LINE__, buffer,
            static_cast<int>(len), clientIdx, (mServer)? "true":"false", static_cast<int>(mClients.size()));
#ifdef _WINDLL
    if (mSocket == INVALID_SOCKET) {
#else
    if (mSocket < 0) {
#endif
        LOGW(LOG_FORMAT(" - Socket not opened"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    assert((!mServer) || ((mServer) && (mClients.size() > clientIdx)));
    int socket = (!mServer)? mSocket:mClients[clientIdx];
#ifdef _WINDLL
    return send(socket, buffer, static_cast<int>(len), 0);
#else
    return static_cast<int>(write(socket, buffer, len));
#endif
}

bool Socket::connexion(const std::string &host, int port) {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - h:%s; p:%d (s:%s; s:%d)"), __PRETTY_FUNCTION__, __LINE__, host.c_str(), port,
            (mServer)? "true":"false", mSocket);
    assert(!mServer);

    if (!open())
        return false;

    // Method #1
    struct in_addr ipAddr;
    inet_pton(AF_INET, host.c_str(), &ipAddr);
#if defined(__ANDROID__) || defined(_WINDLL)
    struct hostent* server = gethostbyaddr((const char*)&ipAddr, sizeof(ipAddr), AF_INET);
#else // iOS
    struct hostent* server = gethostbyaddr(&ipAddr, sizeof(ipAddr), AF_INET);
#endif
    if (!server) {

        LOGI(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - Address %s not found"), __PRETTY_FUNCTION__, __LINE__, host.c_str());
        server = gethostbyname(host.c_str());
    }
    if (!server) {
        LOGI(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - Host %s not found"), __PRETTY_FUNCTION__, __LINE__, host.c_str());
    }
    else {

        struct sockaddr_in servAddr;
        std::memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        memcpy(&servAddr.sin_addr.s_addr, server->h_addr, server->h_length);

        servAddr.sin_port = htons(port);
        if (connect(mSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {

            LOGW(LOG_FORMAT(" - Failed to connect #1: %s:%d (%d)"), __PRETTY_FUNCTION__, __LINE__, host.c_str(), port,
                 static_cast<int>(errno));
#ifdef _WINDLL
            WSACleanup();
#endif
        }
        else {

            LOGI(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - Connected #1: %s:%d"), __PRETTY_FUNCTION__, __LINE__, host.c_str(),
                 port);
#ifdef _WINDLL
            u_long nonBlock = 1;
            ioctlsocket(mSocket, FIONBIO, &nonBlock);
#else
            fcntl(mSocket, F_SETFL, fcntl(mSocket, F_GETFL, 0) | O_NONBLOCK);
#endif
            return true; // Connected
        }
    }
#ifdef _WINDLL
    closesocket(mSocket);
    WSACleanup();
#else
    close(mSocket);
#endif
    mSocket = UNCHAINED_NO_DATA;

    // Method #2
    struct addrinfo hints, *result;
    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int gaiRes = getaddrinfo(host.c_str(), numToStr<int>(port).c_str(), &hints, &result);
    if (gaiRes != 0) {

        LOGW(LOG_FORMAT(" - Failed to get address info: %s"), __PRETTY_FUNCTION__, __LINE__, gai_strerror(gaiRes));
#ifdef _WINDLL
        WSACleanup();
#endif
        return false;
    }
    for (struct addrinfo* walk = result; walk; walk = walk->ai_next) {
        if (!open()) {

            freeaddrinfo(result);
#ifdef _WINDLL
            WSACleanup();
#endif
            return false;
        }
        if (connect(mSocket, walk->ai_addr, walk->ai_addrlen) < 0) {
            LOGW(LOG_FORMAT(" - Connexion failed (err: %d)"), __PRETTY_FUNCTION__, __LINE__, static_cast<int>(errno));
#ifdef _WINDLL
            WSACleanup();
#endif
        }
        else {

            LOGI(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - Connected #2: %s:%d"), __PRETTY_FUNCTION__, __LINE__, host.c_str(),
                 port);
#ifdef _WINDLL
            u_long nonBlock = 1;
            ioctlsocket(mSocket, FIONBIO, &nonBlock);
#else
            fcntl(mSocket, F_SETFL, fcntl(mSocket, F_GETFL, 0) | O_NONBLOCK);
#endif
            freeaddrinfo(result);
            return true; // Connected
        }
    }
    LOGW(LOG_FORMAT(" - Failed to connect #2: %s:%d"), __PRETTY_FUNCTION__, __LINE__, host.c_str(), port);
    freeaddrinfo(result);
#ifdef _WINDLL
    closesocket(mSocket);
    WSACleanup();
#else
    close(mSocket);
#endif
    mSocket = UNCHAINED_NO_DATA;
    return false;
}

bool Socket::start(int port) {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - p:%d (s:%s)"), __PRETTY_FUNCTION__, __LINE__, port,
            (mServer)? "true":"false");
    assert(mServer);
    if (mThread) {

        LOGW(LOG_FORMAT(" - Server already started"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
#ifdef _WINDLL
    if (mSocket == INVALID_SOCKET) {
#else
    if (mSocket < 0) {
#endif
        LOGW(LOG_FORMAT(" - Socket not opened"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(mSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {

        LOGW(LOG_FORMAT(" - Failed to bind (%d)"), __PRETTY_FUNCTION__, __LINE__, port);
        return false;
    }
    listen(mSocket, MAX_CLIENT_COUNT);

    mAbort = false;
    mThread = new boost::thread(Socket::startServerThread, this);
    return true;
}
bool Socket::stop() {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - (s:%s)"), __PRETTY_FUNCTION__, __LINE__, (mServer)? "true":"false");
    assert(mServer);
    if (!mThread) {

        LOGW(LOG_FORMAT(" - Server already stopped"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
        return false;
    }
    shutdown();
    return true;
}
void Socket::closeClient(unsigned char clientIdx) {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - i:%d (s:%d)"), __PRETTY_FUNCTION__, __LINE__, clientIdx,
            static_cast<int>(mClients.size()));
    assert(mClients.size() > clientIdx);

    mMutex.lock();
    std::vector<int>::iterator iter = (mClients.begin() + clientIdx); // Index still good even if just added one
#ifdef _WINDLL
    closesocket(*iter);
    WSACleanup();
#else
    close(*iter);
#endif
    mClients.erase(iter);
    mMutex.unlock();
}

void Socket::serverThreadRunning() {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - Begin"), __PRETTY_FUNCTION__, __LINE__);
    while (!mAbort) {

        boost::this_thread::sleep(boost::posix_time::milliseconds(ACCEPT_DELAY));

        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newSocket = accept(mSocket, (struct sockaddr*)&cli_addr, &clilen);
#ifdef _WINDLL
        if (newSocket == INVALID_SOCKET) {

            int err = WSAGetLastError();
            switch (err) {
                case WSAEWOULDBLOCK:
                    break; // No new client

                default: {

                    LOGW(LOG_FORMAT(" - Failed to accept client (%d)"), __PRETTY_FUNCTION__, __LINE__, err);
#else
        if (newSocket < 0) {

            int err = errno;
            switch (err) {
                case EWOULDBLOCK: // EAGAIN
                    break; // No new client

                default: {

                    LOGW(LOG_FORMAT(" - Failed to accept client (%d)"), __PRETTY_FUNCTION__, __LINE__, err);
#endif
                    assert(NULL);
                    break;
                }
            }
            continue;
        }
#ifdef _WINDLL
        u_long nonBlock = 1;
        ioctlsocket(newSocket, FIONBIO, &nonBlock);
#else
        fcntl(newSocket, F_SETFL, fcntl(newSocket, F_GETFL, 0) | O_NONBLOCK);
#endif
        mMutex.lock();
        mClients.push_back(newSocket);
        mMutex.unlock();
    }
    LOGI(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - Finish"), __PRETTY_FUNCTION__, __LINE__);
}
void Socket::startServerThread(Socket* tcp) {

    LOGV(UNCHAINED_LOG_INTERNET, 0, LOG_FORMAT(" - t:%p"), __PRETTY_FUNCTION__, __LINE__, tcp);
    tcp->serverThreadRunning();
}
