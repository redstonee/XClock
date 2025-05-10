#include "AlarmClk.h"
#include <Preferences.h>
#include <vector>

std::vector<AlarmConfig> AlarmClkList;

static const char *TAG = "AlarmClk";

void boInitAlarmClkList(void)
{
    uint8_t alarm_num = 0;
    Preferences pref;
    pref.begin(PrefKey_AlarmNameSpace);
    alarm_num = pref.getUChar(PrefKey_AlarmNum, 0);
    if (alarm_num == 0) // no value,add a default value
    {
        alarm_num = 1;
        pref.putUChar((PrefKey_AlarmMin + std::to_string(0)).c_str(), 0);     // add a default alarm
        pref.putUChar((PrefKey_AlarmHour + std::to_string(0)).c_str(), 6);    // add a default alarm
        pref.putUChar((PrefKey_AlarmWeek + std::to_string(0)).c_str(), 0x1f); // bit6 Sunday, bit5 Saturday, bit4 Friday,bit3 Thursday,bit2 Wednesday, bit1 tuesday, bit0 Monday
        pref.putBool((PrefKey_AlarmActive + std::to_string(0)).c_str(), false);
        pref.putUChar(PrefKey_AlarmNum, alarm_num);
        ESP_LOGI(TAG, "No alarm, add default\n\r");
    }
    for (uint8_t i = 0; i < alarm_num; i++)
    {
        AlarmConfig alarm_tmp = {
            0,
        };
        alarm_tmp.minute = pref.getUChar((PrefKey_AlarmMin + std::to_string(i)).c_str(), 0);
        alarm_tmp.hour = pref.getUChar((PrefKey_AlarmHour + std::to_string(i)).c_str(), 6);
        alarm_tmp.week = pref.getUChar((PrefKey_AlarmWeek + std::to_string(i)).c_str(), 0x1f);
        alarm_tmp.isActive = pref.getBool((PrefKey_AlarmActive + std::to_string(i)).c_str(), false);
        alarm_tmp.alarmStatus = Alarm_Idle;
        AlarmClkList.push_back(alarm_tmp);
    }
    pref.end();
    uint8_t alarm_total = AlarmClkList.size();
    AlarmConfig alarm_temp;
    for (uint8_t i = 0; i < alarm_total; i++)
    {
        alarm_temp = AlarmClkList.at(i);
        ESP_LOGI(TAG, "Alarm%d %d:%d:%x Active:%x \n\r", i, alarm_temp.hour, alarm_temp.minute, alarm_temp.week, alarm_temp.isActive);
    }
}

size_t getAlarmClockCount(void)
{
    return AlarmClkList.size();
}

AlarmConfig getAlarmClock(uint8_t index)
{
    AlarmConfig stAlarmClk = {
        0,
    };
    if (index >= AlarmClkList.size())
    {
        stAlarmClk = AlarmClkList.at(AlarmClkList.size() - 1);
    }
    else
    {
        stAlarmClk = AlarmClkList.at(index);
    }
    return stAlarmClk;
}

bool boAddAlarmClk(AlarmConfig *alarmclk)
{
    if (AlarmClkList.size() >= 255)
        return false; // max 255 alarm clocks

    uint8_t index = AlarmClkList.size();
    AlarmConfig alarm_tmp = *alarmclk;
    Preferences pref;
    pref.begin(PrefKey_AlarmNameSpace);
    pref.putUChar((PrefKey_AlarmMin + std::to_string(index)).c_str(), alarm_tmp.minute);
    pref.putUChar((PrefKey_AlarmHour + std::to_string(index)).c_str(), alarm_tmp.hour);
    pref.putUChar((PrefKey_AlarmWeek + std::to_string(index)).c_str(), alarm_tmp.week);
    pref.putBool((PrefKey_AlarmActive + std::to_string(index)).c_str(), alarm_tmp.isActive);
    pref.putUChar(PrefKey_AlarmNum, index + 1);
    pref.end();
    AlarmClkList.push_back(alarm_tmp);
}

bool boDelAlarmClk(uint8_t index)
{
    bool res = true;
    if (index >= AlarmClkList.size())
    {
        res = false;
    }
    else
    {
        AlarmClkList.erase(AlarmClkList.begin() + index);
        uint8_t alarm_num = AlarmClkList.size();
        AlarmConfig alarm_tmp;
        Preferences pref;
        pref.begin(PrefKey_AlarmNameSpace);
        for (uint8_t i = 0; i < alarm_num; i++)
        {
            alarm_tmp = AlarmClkList.at(i);
            pref.putUChar((PrefKey_AlarmMin + std::to_string(i)).c_str(), alarm_tmp.minute);
            pref.putUChar((PrefKey_AlarmHour + std::to_string(i)).c_str(), alarm_tmp.hour);
            pref.putUChar((PrefKey_AlarmWeek + std::to_string(i)).c_str(), alarm_tmp.week);
            pref.putBool((PrefKey_AlarmActive + std::to_string(i)).c_str(), alarm_tmp.isActive);
        }
        pref.putUChar(PrefKey_AlarmNum, alarm_num);
        pref.end();
    }

    return res;
}

bool boSetAlarmClk(uint8_t index, AlarmConfig *alarmclk)
{
    bool res = true;
    if (index >= AlarmClkList.size())
    {
        res = false;
    }
    else
    {
        AlarmConfig alarm_tmp = *alarmclk;
        AlarmClkList[index] = alarm_tmp;
        Preferences pref;
        pref.begin(PrefKey_AlarmNameSpace);
        pref.putUChar((PrefKey_AlarmMin + std::to_string(index)).c_str(), alarm_tmp.minute);
        pref.putUChar((PrefKey_AlarmHour + std::to_string(index)).c_str(), alarm_tmp.hour);
        pref.putUChar((PrefKey_AlarmWeek + std::to_string(index)).c_str(), alarm_tmp.week);
        pref.putBool((PrefKey_AlarmActive + std::to_string(index)).c_str(), alarm_tmp.isActive);
        pref.end();
    }

    return res;
}

void vSetAlarmClkSts(uint8_t index, AlarmStatus Status)
{
    AlarmClkList[index].alarmStatus = Status;
}