#include "AlarmClk.h"
#include <Preferences.h>
#include <vector>

std::vector<tstAlarmClk> AlarmClkList;

void boInitAlarmClkList(void)
{
    uint8_t alarm_num = 0;
    Preferences pref;
    pref.begin(PrefKey_AlarmNameSpace);
    alarm_num = pref.getUChar(PrefKey_AlarmNum,0);
    if(alarm_num == 0)                     //no value,add a default value
    {
        alarm_num = 1;
        pref.putUChar((PrefKey_AlarmMin + std::to_string(0)).c_str(),0);  //add a default alarm
        pref.putUChar((PrefKey_AlarmHour + std::to_string(0)).c_str(),6);  //add a default alarm
        pref.putUChar((PrefKey_AlarmWeek + std::to_string(0)).c_str(),0x1f);//bit6 Sunday, bit5 Saturday, bit4 Friday,bit3 Thursday,bit2 Wednesday, bit1 tuesday, bit0 Monday 
        pref.putBool((PrefKey_AlarmActive + std::to_string(0)).c_str(),false);
        pref.putUChar(PrefKey_AlarmNum,alarm_num);
        Serial.printf("No alarm, add default\n");
    }
    for(uint8_t i = 0;i < alarm_num; i++)
    {
        tstAlarmClk alarm_tmp = {0,};
        alarm_tmp.u8Min = pref.getUChar((PrefKey_AlarmMin + std::to_string(i)).c_str(),0); 
        alarm_tmp.u8Hour = pref.getUChar((PrefKey_AlarmHour + std::to_string(i)).c_str(),6);  
        alarm_tmp.u8Week = pref.getUChar((PrefKey_AlarmWeek + std::to_string(i)).c_str(),0x1f);
        alarm_tmp.boActive = pref.getBool((PrefKey_AlarmActive + std::to_string(i)).c_str(),false);
        AlarmClkList.push_back(alarm_tmp);
    }
    pref.end();
    uint8_t alarm_total = AlarmClkList.size();
    tstAlarmClk alarm_temp;
    for(uint8_t i = 0; i < alarm_total; i++)
    {
        alarm_temp = AlarmClkList.at(i);
        Serial.printf("Alarm%d %d:%d:%x Active:%x \n",i,alarm_temp.u8Hour,alarm_temp.u8Min,alarm_temp.u8Week,alarm_temp.boActive);
    }
}

uint8_t u8GetAlarmClkNum(void)
{
    uint8_t num = 0;
    num = AlarmClkList.size();
    return num;
}

tstAlarmClk stGetAlarmClk(uint8_t index)
{
    tstAlarmClk stAlarmClk = {0,};
    if(index >= AlarmClkList.size())
    {
        stAlarmClk = AlarmClkList.at(AlarmClkList.size()-1);
    }
    else
    {
        stAlarmClk = AlarmClkList.at(index);
    }    
    return stAlarmClk;
}

bool boAddAlarmClk(tstAlarmClk *alarmclk)
{
    bool res = true;
    uint8_t index = AlarmClkList.size();
    tstAlarmClk alarm_tmp = *alarmclk;    
    Preferences pref;
    pref.begin(PrefKey_AlarmNameSpace);
    pref.putUChar((PrefKey_AlarmMin + std::to_string(index)).c_str(),alarm_tmp.u8Min);  
    pref.putUChar((PrefKey_AlarmHour + std::to_string(index)).c_str(),alarm_tmp.u8Hour);  
    pref.putUChar((PrefKey_AlarmWeek + std::to_string(index)).c_str(),alarm_tmp.u8Week);
    pref.putBool((PrefKey_AlarmActive + std::to_string(index)).c_str(),alarm_tmp.boActive);
    pref.putUChar(PrefKey_AlarmNum,index+1);
    pref.end();
    AlarmClkList.push_back(alarm_tmp);
    return res;
}

bool boDelAlarmClk(uint8_t index)
{
    bool res = true;
    if(index >= AlarmClkList.size())
    {
        res = false;
    }
    else
    {
        AlarmClkList.erase(AlarmClkList.begin() + index);
        uint8_t alarm_num = AlarmClkList.size();
        tstAlarmClk alarm_tmp;
        Preferences pref;
        pref.begin(PrefKey_AlarmNameSpace);
        for(uint8_t i = 0; i < alarm_num; i++)
        {
            alarm_tmp = AlarmClkList.at(i);
            pref.putUChar((PrefKey_AlarmMin + std::to_string(i)).c_str(),alarm_tmp.u8Min);  
            pref.putUChar((PrefKey_AlarmHour + std::to_string(i)).c_str(),alarm_tmp.u8Hour);  
            pref.putUChar((PrefKey_AlarmWeek + std::to_string(i)).c_str(),alarm_tmp.u8Week);
            pref.putBool((PrefKey_AlarmActive + std::to_string(i)).c_str(),alarm_tmp.boActive);    
        }
        pref.putUChar(PrefKey_AlarmNum,alarm_num);
        pref.end();
    }
    
    return res;
}

bool boSetAlarmClk(uint8_t index,tstAlarmClk* alarmclk)
{
    bool res = true;
    if(index >= AlarmClkList.size())
    {
        res = false;
    }
    else
    {
        tstAlarmClk alarm_tmp = *alarmclk;
        AlarmClkList[index] = alarm_tmp;
        Preferences pref;
        pref.begin(PrefKey_AlarmNameSpace);
        pref.putUChar((PrefKey_AlarmMin + std::to_string(index)).c_str(),alarm_tmp.u8Min);  
        pref.putUChar((PrefKey_AlarmHour + std::to_string(index)).c_str(),alarm_tmp.u8Hour);  
        pref.putUChar((PrefKey_AlarmWeek + std::to_string(index)).c_str(),alarm_tmp.u8Week);
        pref.putBool((PrefKey_AlarmActive + std::to_string(index)).c_str(),alarm_tmp.boActive);
        pref.end();
    }
    
    return res;
}