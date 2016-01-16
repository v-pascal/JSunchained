#ifndef UNCHAINED_BLUETOOTH_H_
#define UNCHAINED_BLUETOOTH_H_

#include <boost/thread.hpp>
#include <string>


static const char NO_BLUETOOTH_DEVICE[] = "#BT-NONE#";

//////
class Bluetooth {

public:
    static bool isEnabled();

    static bool discover();
    static bool isDiscovering();
    static std::string getDevice(unsigned char index);

};

#endif // UNCHAINED_BLUETOOTH_H_
