#ifndef __MAIN__
#define __MAIN__
#include <Arduino.h>
#include "FastLED.h"
#include "SD3078.hpp"

#define AlarmRecallTimeout  (portTICK_PERIOD_MS*1000*60*2)



QueueHandle_t pGetTimeSettingQ(void);

void RequestWakeup(bool boHMIDis);

void ClearWakeupRequest(bool boHMIDis);


#endif