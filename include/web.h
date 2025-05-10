#ifndef __WEB_H__
#define __WEB_H__

#define PrefKey_WeatherSpace                "WeatherInfo"
#define PrefKey_WeatherCodeCur              "WeatherCodeCur"
#define PrefKey_WeatherTemperateCur         "WeatherTemperateCur"
#define PrefKey_WifiConfiged                "WifiConfiged"
#define PrefKey_WifiSSID                    "WifiSSID"

void initWiFi(void);

void wifiConfig();

void clearWiFiConfig();

bool isWifiConfigured(void);

String getSSIDConfig(void);

uint8_t GetCurWeatherCode();

#endif