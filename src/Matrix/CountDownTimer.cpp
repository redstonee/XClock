#include "CountDownTimer.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "FastLED.h"
#include "../Key/ClockKey.h"
#include "../Sound/Sound.h"

NS_DT_BEGIN

typedef struct
{
    uint8_t u8Min;
    uint8_t u8Sec;
    bool boTiming;
}tstCountTimerType;

tstCountTimerType GlobaCountTimer = {0,0,false};
TimerHandle_t CounterTimer = nullptr;
TimerHandle_t ExitTimer = nullptr;
bool boActive = false;

bool boIsCountDownTimerActive()
{
    return boActive;
}
/********************************Timer sence and layer****************************************/

void CounterTimerCb(TimerHandle_t xTimer)
{
    if(GlobaCountTimer.u8Sec-- <= 00)
    {        
        if(GlobaCountTimer.u8Min-- <= 0)
        {
            GlobaCountTimer.u8Min = 0;
            GlobaCountTimer.u8Sec = 0;
            GlobaCountTimer.boTiming = false;
            xTimerStop(CounterTimer,10);
            xTimerStart(ExitTimer,10);
            if(enSndID_None == enGetCurSndID())
            {
                boReqSound(enSndID_Alarm2,1);
            }
        }
        else
        {
            GlobaCountTimer.u8Sec = 59;
        }
    }
}

void ExitTimerCb(TimerHandle_t xTimer)
{
    boActive = false;
}

bool CountDownScene::init()
{
    CountDownLayer *Timerlayer = CountDownLayer::create();
    Timerlayer->setContentSize(Size(32,8));
    Timerlayer->setPosition(0,0);
    //ClockLayer->setOpacity
    this->addChild(Timerlayer);
    Timerlayer->initLayer();
    boActive = true;
    return true;
}

bool CountDownLayer::initLayer()
{
    auto listener = EventListenerButton::create();
    listener ->onBtnDuringLongPress = DT_CALLBACK_2(CountDownLayer::BtnDuringLongPressHandler,this);
    listener ->onBtnLongPressStart = DT_CALLBACK_2(CountDownLayer::BtnLongPressStartHandler,this);
    listener ->onBtnClick = DT_CALLBACK_2(CountDownLayer::BtnClickHandler,this);   
    listener ->onBtnDoubleClick = DT_CALLBACK_2(CountDownLayer::BtnDoubleClickHandler,this);  
    _eventDispatcher->addEventListenerWithSceneGraphPriority ( listener, this );
    DTRGB TextColor = {255,255,255};
    enTimerSts = Timer_Dis;
    if(GlobaCountTimer.boTiming)
    {
        TimerIcon = FrameSprite::create(icon_TimerActive,sizeof(icon_TimerActive),BMP_GIF);
    }
    else
    {
        TimerIcon = FrameSprite::create(icon_TimerInactive,sizeof(icon_TimerInactive),BMP_BMP);
        GlobaCountTimer.u8Min = 25;
        GlobaCountTimer.u8Sec = 0;
    }
    
    TimerIcon->setPosition(-1,-1);
    TimerIcon->setAutoSwitch(true);
    TimePt = TextSprite::create(Size(2,5),Size(2,5),TextColor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    TimePtcanvas = TimePt->getSpriteCanvas();
    Min = TextSprite::create(Size(8,5),Size(8,5),TextColor,(std::to_string(GlobaCountTimer.u8Min/10) + std::to_string(GlobaCountTimer.u8Min%10)),TextSprite::TextAlign::TextAlignRight,&TomThumb);
    Mincanvas = Min->getSpriteCanvas();
    Sec = TextSprite::create(Size(8,5),Size(8,5),TextColor,(std::to_string(GlobaCountTimer.u8Sec/10) + std::to_string(GlobaCountTimer.u8Sec%10)),TextSprite::TextAlign::TextAlignRight,&TomThumb);
    Seccanvas = Sec->getSpriteCanvas();    
    Min->setPosition(10,1); 
    TimePt->setPosition(18,1);
    Sec->setPosition(20,1);
    this->addChild(TimePt);
    this->addChild(Min);
    this->addChild(Sec);
    this->addChild(TimerIcon);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(CountDownLayer::TimerUpdate),0.1);
    if(nullptr == CounterTimer)
    {
        CounterTimer = xTimerCreate
                   ( /* Just a text name, not used by the RTOS
                     kernel. */
                     "FeatureTimer",
                     /* The timer period in ticks, must be
                     greater than 0. */
                     (portTICK_PERIOD_MS*1000),
                     /* The timers will auto-reload themselves
                     when they expire. */
                     pdTRUE,
                     /* The ID is used to store a count of the
                     number of times the timer has expired, which
                     is initialised to 0. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when
                     it expires. */
                     CounterTimerCb
                   );
    }
    if(nullptr == ExitTimer)
    {
        ExitTimer = xTimerCreate
                   ( /* Just a text name, not used by the RTOS
                     kernel. */
                     "FeatureTimer",
                     /* The timer period in ticks, must be
                     greater than 0. */
                     (portTICK_PERIOD_MS*CountDownTimerExit_TO),
                     /* The timers will auto-reload themselves
                     when they expire. */
                     pdTRUE,
                     /* The ID is used to store a count of the
                     number of times the timer has expired, which
                     is initialised to 0. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when
                     it expires. */
                     ExitTimerCb
                   );
    }
    
    return true;
}



void CountDownLayer::TimerUpdate(float dt)
{
    static tstCountTimerType TimerPre = GlobaCountTimer;
    if(GlobaCountTimer.boTiming)
    {
        if(0 == TimePt->getNumberOfRunningActions())
        {
            TimePt->runAction(RepeatForever::create(Blink::create(1,1)));
            TimerIcon->setSpriteFrame(SpriteFrame::create(icon_TimerActive,sizeof(icon_TimerActive),BMP_GIF));
        }        
    }
    else
    {
        if(0 != TimePt->getNumberOfRunningActions())
        {
            TimePt->stopAllActions();
            TimePt->setVisible(true);
            TimerIcon->setSpriteFrame(SpriteFrame::create(icon_TimerInactive,sizeof(icon_TimerInactive),BMP_BMP));
        }        
    }
    if(TimerPre.u8Min != GlobaCountTimer.u8Min)
    {
        Mincanvas->canvasReset();
        Mincanvas->print((std::to_string(GlobaCountTimer.u8Min/10) + std::to_string(GlobaCountTimer.u8Min%10)).c_str());
    }
    if(TimerPre.u8Sec != GlobaCountTimer.u8Sec)
    {
        Seccanvas->canvasReset();
        Seccanvas->print((std::to_string(GlobaCountTimer.u8Sec/10) + std::to_string(GlobaCountTimer.u8Sec%10)).c_str());
    }
    if(enTimerSts == Timer_Dis)
    {
        if(0 != Min->getNumberOfRunningActions())
        {
            Min->stopAllActions();
            Min->setVisible(true);
        }
        if(0 != Sec->getNumberOfRunningActions())
        {
            Sec->stopAllActions();
            Sec->setVisible(true);
        }
    }
    else if(enTimerSts == Timer_MinSetting)
    {
        if(0 == Min->getNumberOfRunningActions())
        {
            Min->runAction(RepeatForever::create(Blink::create(1,1)));
        }
        if(0 != Sec->getNumberOfRunningActions())
        {
            Sec->stopAllActions();
            Sec->setVisible(true);
        }
    }
    else
    {
        if(0 != Min->getNumberOfRunningActions())
        {
            Min->stopAllActions();
            Min->setVisible(true);
        }
        if(0 == Sec->getNumberOfRunningActions())
        {
            Sec->runAction(RepeatForever::create(Blink::create(1,1)));
        }
    }
    TimerPre = GlobaCountTimer;
}

void CountDownLayer::TimerMachine(int8_t keyCode, int8_t event)
{
    if(enKey_OK == keyCode)
    {   
        if(enKey_ShortPress == event)
        {
            if(enTimerSts == Timer_Dis)
            {
                if(xTimerIsTimerActive(CounterTimer))
                {
                    xTimerStop(CounterTimer,10);
                    xTimerStart(ExitTimer,10);
                    GlobaCountTimer.boTiming = false;
                }
                else
                {
                    xTimerStart(CounterTimer,10);
                    xTimerStop(ExitTimer,10);
                    GlobaCountTimer.boTiming = true;
                } 
            }
            else if(enTimerSts == Timer_MinSetting)
            {
                enTimerSts = Timer_Dis;
            }    
            else
            {
                enTimerSts = Timer_MinSetting;
            }  
        }
        else if(enKey_LongPressStart == event)
        {
            if(enTimerSts == Timer_Dis)
            {
                enTimerSts = Timer_SecSetting;
                if(xTimerIsTimerActive(CounterTimer))
                {
                    xTimerStop(CounterTimer,10);
                    xTimerStart(ExitTimer,10);
                }
                GlobaCountTimer.boTiming = false;
            }
        }
           
    }
    else if(enKey_Right == keyCode)
    {
        if(enTimerSts == Timer_MinSetting)
        {
            if(++GlobaCountTimer.u8Min > 99)
            {
                GlobaCountTimer.u8Min = 0;
            }
        }    
        else if(enTimerSts == Timer_SecSetting)
        {
            if(++GlobaCountTimer.u8Sec >= 60)
            {
                GlobaCountTimer.u8Sec = 0;
            }
        }
    }
    else if(enKey_Left == keyCode)
    {
        if(enTimerSts == Timer_MinSetting)
        {
            if(GlobaCountTimer.u8Min-- <= 0)
            {
                GlobaCountTimer.u8Min = 99;
            }
        }    
        else if(enTimerSts == Timer_SecSetting)
        {
            if(GlobaCountTimer.u8Sec-- <= 0)
            {
                GlobaCountTimer.u8Sec = 59;
            }
        }
    }
}

void CountDownLayer::BtnClickHandler(int8_t keyCode, Event* event)
{
    TimerMachine(keyCode,enKey_ShortPress);
}

void CountDownLayer::BtnDoubleClickHandler(int8_t keyCode, Event* event)
{
    TimerMachine(keyCode,enKey_DoubleClick);
}

void CountDownLayer::BtnLongPressStartHandler(int8_t keyCode, Event* event)
{
    TimerMachine(keyCode,enKey_LongPressStart);
}

void CountDownLayer::BtnDuringLongPressHandler(int8_t keyCode, Event* event)
{
    TimerMachine(keyCode,enKey_LongPress);
}


NS_DT_END