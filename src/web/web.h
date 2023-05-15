#ifndef __WEB_H__
#define __WEB_H__

#define PrefKey_WeatherSpace                "WeatherInfo"
#define PrefKey_WeatherCodeCur              "WeatherCodeCur"
#define PrefKey_WeatherTemperateCur         "WeatherTemperateCur"

void vWifiInit(void);

bool boValidWifi(void);

void handleRoot();

void handleConfigWifi();

void handleNotFound();

void wifiConfig();

int GetCurWeatherCode();

void SetupWifi(void);
#endif