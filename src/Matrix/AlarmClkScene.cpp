#include "AlarmClkScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "../RTC/SD3078.h"
#include "FastLED.h"
#include "../Key/ClockKey.h"

NS_DT_BEGIN


/********************************Date sence and layer****************************************/

bool AlarmClkScene::init()
{
    AlarmClkLayer *Alarmlayer = AlarmClkLayer::create();
    Alarmlayer->setContentSize(Size(32,8));
    Alarmlayer->setPosition(0,0);
    //ClockLayer->setOpacity
    this->addChild(Alarmlayer);
    Alarmlayer->initLayer();
    return true;
}

bool AlarmClkLayer::initLayer()
{
    CRGBPalette16 currentPalette( RainbowColors_p );
    u8TotalAlarmNum = u8GetAlarmClkNum();
    if(u8TotalAlarmNum)
    {
        u8CurrentAlarm = 0;
        AlarmTime = stGetAlarmClk(u8CurrentAlarm);
    }
    else
    {
        AlarmTime.u8Hour = 0;
        AlarmTime.u8Min = 0;
        AlarmTime.u8Week = 0;
        AlarmTime.boActive = false;
        boAddAlarmClk(&AlarmTime);
    }
    auto listener = EventListenerButton::create();
    listener ->onBtnDuringLongPress = DT_CALLBACK_2(AlarmClkLayer::BtnDuringLongPressHandler,this);
    listener ->onBtnLongPressStart = DT_CALLBACK_2(AlarmClkLayer::BtnLongPressStartHandler,this);
    listener ->onBtnClick = DT_CALLBACK_2(AlarmClkLayer::BtnClickHandler,this);    
    _eventDispatcher->addEventListenerWithSceneGraphPriority ( listener, this );
    FrameSprite* ClkIcon = FrameSprite::create(icon_ClockActive,sizeof(icon_ClockActive),BMP_GIF);
    ClkIcon->setPosition(0,0);
    ClkIcon->setAutoSwitch(true);
    DTRGB clkcolor(255,255,255);
    if(AlarmTime.boActive)
    {
        clkcolor.r = 0;
        clkcolor.g = 255;
        clkcolor.b = 0;
    }
    else
    {
        clkcolor.r = 255;
        clkcolor.g = 255;
        clkcolor.b = 255;
    }    
    std::string hour = std::to_string(AlarmTime.u8Hour>>4) + std::to_string((AlarmTime.u8Hour&0x0f));
    Hour = TextSprite::create(Size(8,5),Size(8,5),clkcolor,hour,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Hourcanvas = Hour->getSpriteCanvas();
    TimePt = TextSprite::create(Size(2,5),Size(2,5),clkcolor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    TimePtcanvas = TimePt->getSpriteCanvas();
    std::string min = std::to_string(AlarmTime.u8Min>>4) + std::to_string((AlarmTime.u8Min&0x0f));
    Min = TextSprite::create(Size(8,5),Size(8,5),clkcolor,min,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Mincanvas = Min->getSpriteCanvas();
    Hour->setTransparent(true);
    Hour->setPosition(12,1);
    TimePt->setTransparent(true);
    TimePt->setPosition(20,1);
    Min->setTransparent(true);    
    Min->setPosition(22,1);
    this->addChild(ClkIcon);
    this->addChild(Hour);
    this->addChild(TimePt);
    this->addChild(Min);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(AlarmClkLayer::AlarmUpdate),0.5);
    return true;
}

void AlarmClkLayer::AlarmUpdate(float dt)
{
    // tst3078Time time;
    // time = stGetCurTime();
    // static tst3078Time OldSettingTime = {0,};
    // static tenAlarmState OldState = State_AlarmDis;
    // if(State_AlarmDis == enAlarmState)
    // {
    //     if(0 != Hour->getNumberOfRunningActions())
    //     {
    //         Hour->stopAllActions();
    //         Hour->setVisible(true);
    //     }
    //     if(0 != Min->getNumberOfRunningActions())
    //     {
    //         Min->stopAllActions();
    //         Min->setVisible(true);
    //     }
    //     if(time.u8Day != AlarmTime.u8Day)
    //     {
    //         std::string day = std::to_string(time.u8Day>>4) + std::to_string((time.u8Day&0x0f));
    //         Mincanvas->canvasReset();
    //         Mincanvas->print(day.c_str());
    //     }
    //     if(time.u8Month != AlarmTime.u8Month)
    //     {
    //         std::string month = std::to_string(time.u8Month>>4) + std::to_string((time.u8Month&0x0f));
    //         Hourcanvas->canvasReset();
    //         Hourcanvas->print(month.c_str());
    //     }
    //     AlarmTime = time;
    // }
    // else if(State_AlarmSetMin == enAlarmState)
    // {
    //     if(0 != Hour->getNumberOfRunningActions())
    //     {
    //         Hour->stopAllActions();
    //         Hour->setVisible(true);
    //     }
    //     if(0 == Min->getNumberOfRunningActions())
    //     {
    //         Min->runAction(RepeatForever::create(Blink::create(1,2)));
    //     }
    //     std::string day = std::to_string(ClockTimeSetting.u8Day>>4) + std::to_string((ClockTimeSetting.u8Day&0x0f));
    //     Mincanvas->canvasReset();
    //     Mincanvas->print(day.c_str());
    // }
    // else if(State_AlarmSetHour == enAlarmState)
    // {
    //     if(0 != Min->getNumberOfRunningActions())
    //     {
    //         Min->stopAllActions();
    //         std::string day = std::to_string(ClockTimeSetting.u8Day>>4) + std::to_string((ClockTimeSetting.u8Day&0x0f));
    //         Mincanvas->canvasReset();
    //         Mincanvas->print(day.c_str());
    //         Min->setVisible(true);
    //     }
    //     if(0 == Hour->getNumberOfRunningActions())
    //     {
    //         Hour->runAction(RepeatForever::create(Blink::create(1,2)));
    //     }
    //     std::string month = std::to_string(ClockTimeSetting.u8Month>>4) + std::to_string((ClockTimeSetting.u8Month&0x0f));
    //     Hourcanvas->canvasReset();
    //     Hourcanvas->print(month.c_str());
    // }
    
    Serial.printf("Alarm Clk sts:%d alarm idx:%d week idx:%d time: %d:%d:%x \n",enAlarmState,u8CurrentAlarm,SettingWeekIdx,AlarmTime.u8Hour,AlarmTime.u8Min,AlarmTime.u8Week);

}

void AlarmClkLayer::BtnClickHandler(int8_t keyCode, Event* event)
{
    AlarmStateMachine(keyCode,enKey_ShortPress);
}

void AlarmClkLayer::BtnLongPressStartHandler(int8_t keyCode, Event* event)
{
    AlarmStateMachine(keyCode,enKey_LongPressStart);
}

void AlarmClkLayer::BtnDuringLongPressHandler(int8_t keyCode, Event* event)
{
    AlarmStateMachine(keyCode,enKey_LongPress);
}

void AlarmClkLayer::StateDisHandle(int8_t key_type, int8_t key_event)
{
    if(enKey_OK == key_type)
    {
        if(enKey_ShortPress == key_event)//next alarm clock
        {
            if(++u8CurrentAlarm >= u8TotalAlarmNum)
            {
                u8CurrentAlarm = 0;
            }
            AlarmTime = stGetAlarmClk(u8CurrentAlarm);
        }
        else if(enKey_LongPressStart == key_event)//enter setting state
        {
            enAlarmState = State_AlarmSetMin;
        }
    }
    else if(enKey_Left == key_type)
    {
        if(enKey_ShortPress == key_event) //previous alarm clock
        {
            if(--u8CurrentAlarm >= u8TotalAlarmNum)
            {
                u8CurrentAlarm = u8TotalAlarmNum;
            }
            AlarmTime = stGetAlarmClk(u8CurrentAlarm);
        }
        else if(enKey_DoubleClick == key_event)//disable the alarm clock
        {
            AlarmTime.boActive = false;
        }
        else if(enKey_LongPressStart == key_event)//add new alarm clock
        {
            tstAlarmClk newAlarmClk = {0,0,0,false};
            if(boAddAlarmClk(&newAlarmClk))
            {
                u8CurrentAlarm++;
                u8TotalAlarmNum = u8GetAlarmClkNum();
                AlarmTime = stGetAlarmClk(u8CurrentAlarm);
            }
        }
    }
    else if(enKey_Right == key_type)
    {
        if(enKey_ShortPress == key_event) //next alarm clock
        {
            if(++u8CurrentAlarm >= u8TotalAlarmNum)
            {
                u8CurrentAlarm = 0;
            }
            AlarmTime = stGetAlarmClk(u8CurrentAlarm);
        }
        else if(enKey_DoubleClick == key_event)//enable the alarm clock
        {
                AlarmTime.boActive = true;
        }
        else if(enKey_LongPressStart == key_event)//delete the alarm clock
        {
            if(boDelAlarmClk(u8CurrentAlarm))
            {
                u8CurrentAlarm--;
                u8TotalAlarmNum = u8GetAlarmClkNum();
                AlarmTime = stGetAlarmClk(u8CurrentAlarm);
            }
        }
    }
}

void AlarmClkLayer::StateSetMinHandle(int8_t key_type, int8_t key_event)
{
    if(enKey_OK == key_type)
    {
        if(enKey_ShortPress == key_event)//enter hour setting state
        {
            enAlarmState = State_AlarmSetHour;
        }
    }
    else if(enKey_Left == key_type)
    {
        /*minutes - 1*/
        if(--AlarmTime.u8Min >= 60)
        {
            AlarmTime.u8Min = 60;
        }
    }
    else if(enKey_Right == key_type)
    {
        /*minutes + 1*/
        if(++AlarmTime.u8Min >= 60)
        {
            AlarmTime.u8Min = 0;
        }
    }
}

void AlarmClkLayer::StateSetHourHandle(int8_t key_type, int8_t key_event)
{
    if(enKey_OK == key_type)
    {
        if(enKey_ShortPress == key_event)//enter week setting state
        {
            enAlarmState = State_AlarmSetWeek;
            SettingWeekIdx = 0;
        }
    }
    else if(enKey_Left == key_type)
    {
        /*hour - 1*/
        if(--AlarmTime.u8Hour >= 24)
        {
            AlarmTime.u8Hour = 23;
        }
    }
    else if(enKey_Right == key_type)
    {
        /*hour + 1*/
        if(++AlarmTime.u8Hour >= 24)
        {
            AlarmTime.u8Hour = 0;
        }
    }
}

void AlarmClkLayer::StateSetWeekHandle(int8_t key_type, int8_t key_event)
{
    if(enKey_OK == key_type)
    {
        if(enKey_ShortPress == key_event)//current day need enable
        {
            AlarmTime.u8Week |= (1<<SettingWeekIdx);
        }
        else if(enKey_LongPressStart == key_event)//finish this alarm setting go back to display state
        {
            boSetAlarmClk(u8CurrentAlarm,&AlarmTime);
            enAlarmState = State_AlarmDis;
        }
    }
    else if(enKey_Left == key_type)
    {
        /*week index -1*/
        if(--SettingWeekIdx >=6)
        {
            SettingWeekIdx = 6;
        }
    }
    else if(enKey_Right == key_type)
    {
        /*minutes + 1*/
        if(++SettingWeekIdx >=7)
        {
            SettingWeekIdx = 0;
        }
    }
}

void AlarmClkLayer::AlarmStateMachine(int8_t key_type, int8_t key_event)
{  
    switch (enAlarmState)
    {
        case State_AlarmDis:
            StateDisHandle(key_type,key_event);
            break;
        case State_AlarmSetMin:
            StateSetMinHandle(key_type,key_event);
            break;
        case State_AlarmSetHour:
            StateSetHourHandle(key_type,key_event);
            break;
        case State_AlarmSetWeek:
            StateSetWeekHandle(key_type,key_event);
            break;
        default:
            break;
    }
}

void AlarmClkLayer::SendSettingAlarm(tst3078Time* settingtime)
{
    // if(TimeSettingQ != nullptr)
    // {
    //     if( xQueueSend( TimeSettingQ,
    //                    ( void * ) settingtime,
    //                    ( TickType_t ) 10 ) != pdPASS )
    //     {
    //         /* Failed to post the message, even after 10 ticks. */
    //     }
    // }
}

NS_DT_END