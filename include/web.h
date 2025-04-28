#ifndef __WEB_H__
#define __WEB_H__

#define PrefKey_WeatherSpace                "WeatherInfo"
#define PrefKey_WeatherCodeCur              "WeatherCodeCur"
#define PrefKey_WeatherTemperateCur         "WeatherTemperateCur"
#define PrefKey_WifiConfiged                "WifiConfiged"
#define PrefKey_WifiSSID                    "WifiSSID"

void vWifiInit(void);

bool boValidWifi(void);

void handleRoot();

void handleConfigWifi();

void handleNotFound();

void wifiConfig();

void restoreWiFi();

bool IsWifiConfig(void);

String GetWifiSSID(void);

uint8_t GetCurWeatherCode();

void SetupWifi(void);
#endif