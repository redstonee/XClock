#ifndef __WEB_H__
#define __WEB_H__

#define PrefKey_WifiConfiged                "WifiConfiged"
#define PrefKey_WifiSSID                    "WifiSSID"

void initWiFi(void);

void wifiConfig();

void clearWiFiConfig();

bool isWifiConfigured(void);

String getSSIDConfig(void);


#endif