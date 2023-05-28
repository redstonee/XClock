#ifndef __MAIN__
#define __MAIN__
#include <Arduino.h>
#include "RTC/SD3078.h"
#include "FastLED.h"

#define BAT_ADC_EN_PORT     (26)
#define BAT_CHARGE_STS_PORT (35)
#define BAT_ADC_CH          (39)
#define BAT_FULL_VOLT       (2150)
#define BAT_EMPTY_VOLT      (1600)
#define BAT_FULL_LVL        (100)
#define BAT_FILTER_CNT      (20)
#define MIC_ADC_EN_PORT     (16)
#define LED_POWER_PORT      (27)
#define MIC_ADC_PORT        (34)
#define AlarmRecallTimeout  (portTICK_PERIOD_MS*1000*60*2)
typedef struct 
{
    bool boCharging;/* Charging status */
    uint8_t BattLvl;/*battery level*/
}tstBattSts;

tst3078Time stGetCurTime(void);

tstBattSts stGetBattSts(void);

QueueHandle_t pGetTimeSettingQ(void);

bool RequestWakeup(bool boHMIDis);

bool boNeedWakeup(bool boHMIDis);

bool ClearWakeupRequest(bool boHMIDis);

void vSetWeatherFlag(bool flag);

#endif