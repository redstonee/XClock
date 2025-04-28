#ifndef __MAIN__
#define __MAIN__
#include <Arduino.h>
#include "FastLED.h"
#include "SD3078.hpp"

#define AlarmRecallTimeout  (portTICK_PERIOD_MS*1000*60*2)



QueueHandle_t pGetTimeSettingQ(void);

void vSetTimeDirect(tst3078Time *time);

bool RequestWakeup(bool boHMIDis);

bool boNeedWakeup(bool boHMIDis);

bool ClearWakeupRequest(bool boHMIDis);

void vSetWeatherFlag(bool flag);

tst3078Time stGetCurTime(void);

#endif