#ifndef __ALARMCLK_H__
#define __ALARMCLK_H__
#include <Arduino.h>

#define PrefKey_AlarmNameSpace    "AlarmClk"
#define PrefKey_AlarmNum          "AlarmClkNum"
#define PrefKey_AlarmMin          "AlarmClkMin"
#define PrefKey_AlarmHour         "AlarmClkHour"
#define PrefKey_AlarmWeek         "AlarmClkWeek"
#define PrefKey_AlarmActive       "AlarmClkActive"

typedef struct
{
    uint8_t u8Min;
    uint8_t u8Hour;
    uint8_t u8Week;//every bit mean one day, bit6 Sunday, bit5 Saturday, bit4 Friday,bit3 Thursday,bit2 Wednesday, bit1 tuesday, bit0 Monday 
    bool boActive;
}tstAlarmClk;

void boInitAlarmClkList(void);

uint8_t u8GetAlarmClkNum(void);

tstAlarmClk stGetAlarmClk(uint8_t index);

bool boAddAlarmClk(tstAlarmClk *alarmclk);

bool boDelAlarmClk(uint8_t index);

bool boSetAlarmClk(uint8_t index,tstAlarmClk* alarmclk);

#endif