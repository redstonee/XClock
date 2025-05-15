#pragma once

#include <Arduino.h>

namespace Network
{
    enum class WiFiStatus
    {
        CONNECTED,
        DISCONNECTED,
        CONFIGURING
    };

    void begin();

    WiFiStatus getWiFiStatus();

    String getSSID();

    void runSmartConfig();
}