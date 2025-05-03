#ifndef __ALARMCLK_H__
#define __ALARMCLK_H__
#include <Arduino.h>

#define PrefKey_AlarmNameSpace "AlarmClk"
#define PrefKey_AlarmNum "AlarmClkNum"
#define PrefKey_AlarmMin "AlarmClkMin"
#define PrefKey_AlarmHour "AlarmClkHour"
#define PrefKey_AlarmWeek "AlarmClkWeek"
#define PrefKey_AlarmActive "AlarmClkActive"

enum AlarmStatus
{
    Alarm_Idle,
    Alarm_Clicked,
    Alarm_GoOff,
    Alarm_WaitAgain
};

struct AlarmConfig
{
    uint8_t minute;
    uint8_t hour;
    uint8_t week; // every bit mean one day, bit6 Sunday, bit5 Saturday, bit4 Friday,bit3 Thursday,bit2 Wednesday, bit1 tuesday, bit0 Monday
    bool isActive;
    AlarmStatus alarmStatus;
};

void boInitAlarmClkList(void);

uint8_t u8GetAlarmClkNum(void);

AlarmConfig stGetAlarmClk(uint8_t index);

bool boAddAlarmClk(AlarmConfig *alarmclk);

bool boDelAlarmClk(uint8_t index);

bool boSetAlarmClk(uint8_t index, AlarmConfig *alarmclk);

void vSetAlarmClkSts(uint8_t, AlarmStatus);

#endif