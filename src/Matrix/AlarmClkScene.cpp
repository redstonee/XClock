#include "AlarmClkScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "../RTC/SD3078.h"
#include "../main.h"
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
    ClockTime = stGetCurTime();
    auto listener = EventListenerButton::create();
    listener ->onBtnDuringLongPress = DT_CALLBACK_2(AlarmClkLayer::BtnDuringLongPressHandler,this);
    listener ->onBtnLongPressStart = DT_CALLBACK_2(AlarmClkLayer::BtnLongPressStartHandler,this);
    listener ->onBtnClick = DT_CALLBACK_2(AlarmClkLayer::BtnClickHandler,this);    
    _eventDispatcher->addEventListenerWithSceneGraphPriority ( listener, this );
    FrameSprite* ClkIcon = FrameSprite::create(icon_ClockActive,sizeof(icon_ClockActive),BMP_GIF);
    ClkIcon->setPosition(0,0);
    ClkIcon->setAutoSwitch(true);
    DTRGB clkcolor(255,255,255);
    std::string hour = std::to_string(ClockTime.u8Hour>>4) + std::to_string((ClockTime.u8Hour&0x0f));
    Hour = TextSprite::create(Size(8,5),Size(8,5),clkcolor,hour,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Hourcanvas = Hour->getSpriteCanvas();
    TimePt = TextSprite::create(Size(2,5),Size(2,5),clkcolor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    TimePtcanvas = TimePt->getSpriteCanvas();
    std::string min = std::to_string(ClockTime.u8Min>>4) + std::to_string((ClockTime.u8Min&0x0f));
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
    tst3078Time time;
    time = stGetCurTime();
    static tst3078Time OldSettingTime = {0,};
    static tenAlarmState OldState = State_AlarmDis;
    if(State_AlarmDis == enAlarmState)
    {
        if(0 != Hour->getNumberOfRunningActions())
        {
            Hour->stopAllActions();
            Hour->setVisible(true);
        }
        if(0 != Min->getNumberOfRunningActions())
        {
            Min->stopAllActions();
            Min->setVisible(true);
        }
        if(time.u8Day != ClockTime.u8Day)
        {
            std::string day = std::to_string(time.u8Day>>4) + std::to_string((time.u8Day&0x0f));
            Mincanvas->canvasReset();
            Mincanvas->print(day.c_str());
        }
        if(time.u8Month != ClockTime.u8Month)
        {
            std::string month = std::to_string(time.u8Month>>4) + std::to_string((time.u8Month&0x0f));
            Hourcanvas->canvasReset();
            Hourcanvas->print(month.c_str());
        }
        ClockTime = time;
    }
    else if(State_AlarmSetMin == enAlarmState)
    {
        if(0 != Hour->getNumberOfRunningActions())
        {
            Hour->stopAllActions();
            Hour->setVisible(true);
        }
        if(0 == Min->getNumberOfRunningActions())
        {
            Min->runAction(RepeatForever::create(Blink::create(1,2)));
        }
        std::string day = std::to_string(ClockTimeSetting.u8Day>>4) + std::to_string((ClockTimeSetting.u8Day&0x0f));
        Mincanvas->canvasReset();
        Mincanvas->print(day.c_str());
    }
    else if(State_AlarmSetHour == enAlarmState)
    {
        if(0 != Min->getNumberOfRunningActions())
        {
            Min->stopAllActions();
            std::string day = std::to_string(ClockTimeSetting.u8Day>>4) + std::to_string((ClockTimeSetting.u8Day&0x0f));
            Mincanvas->canvasReset();
            Mincanvas->print(day.c_str());
            Min->setVisible(true);
        }
        if(0 == Hour->getNumberOfRunningActions())
        {
            Hour->runAction(RepeatForever::create(Blink::create(1,2)));
        }
        std::string month = std::to_string(ClockTimeSetting.u8Month>>4) + std::to_string((ClockTimeSetting.u8Month&0x0f));
        Hourcanvas->canvasReset();
        Hourcanvas->print(month.c_str());
    }
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

void AlarmClkLayer::AlarmStateMachine(int8_t key_type, int8_t key_event)
{
    uint8_t month_temp = (ClockTimeSetting.u8Month>>4)*10 + (ClockTimeSetting.u8Month&0x0f);
    uint8_t day_temp = (ClockTimeSetting.u8Day>>4)*10 + (ClockTimeSetting.u8Day&0x0f);
    switch(enAlarmState)
    {
        case enAlarmState:
            if(key_type == enKey_OK)
            {
                if(key_event == enKey_LongPressStart)
                {
                    enAlarmState = State_DaySet;
                    ClockTimeSetting = ClockTime;
                }
            }
            break;
        case State_DaySet:            
            if(key_type == enKey_OK)
            {
                if(key_event == enKey_ShortPress)
                {
                    enAlarmState = State_AlarmSetHour;
                }
            }
            else if(key_type == enKey_Left)
            {
                
                if(--day_temp > 31)
                {
                   day_temp = 31;
                }
            }
            else if(key_type == enKey_Right)
            {
                if(++day_temp > 31)
                {
                    day_temp = 0;
                }
            }
            ClockTimeSetting.u8Day = ((day_temp/10)<<4) + (day_temp%10);
            break;
        case State_AlarmSetHour:
            if(key_type == enKey_OK)
            {
                if(key_event == enKey_ShortPress)
                {
                    enAlarmState = enAlarmState;
                    SendSettingAlarm(&ClockTimeSetting);
                }
            }
            else if(key_type == enKey_Left)
            {
                
                if(--month_temp > 12)
                {
                   month_temp = 12;
                }
            }
            else if(key_type == enKey_Right)
            {
                if(++month_temp > 12)
                {
                    month_temp = 0;
                }
            }
            ClockTimeSetting.u8Month = ((month_temp/10)<<4) + (month_temp%10);
            break;
        default:break;
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