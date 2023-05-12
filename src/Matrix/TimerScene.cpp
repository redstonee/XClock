#include "TimerScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "FastLED.h"
#include "../Key/ClockKey.h"

NS_DT_BEGIN

typedef struct
{
    uint8_t u8MSec;
    uint8_t u8Min;
    uint8_t u8Sec;
    bool boTiming;
}tstTimerType;

tstTimerType GlobalTimer = {0,0,0,false};
TimerHandle_t TimerCounter = nullptr;
/********************************Timer sence and layer****************************************/

void TimerCounterCb(TimerHandle_t xTimer)
{
    if(++GlobalTimer.u8MSec >= 100)
    {
        GlobalTimer.u8MSec = 0;
        if(++GlobalTimer.u8Sec >= 60)
        {
            GlobalTimer.u8Sec = 0;
            if(++GlobalTimer.u8Min >= 100)
            {
                GlobalTimer.u8Min = 0;
                GlobalTimer.u8Sec = 0;
                GlobalTimer.u8MSec = 0;
            }
        }
    }
}

bool TimerScene::init()
{
    TimerLayer *Timerlayer = TimerLayer::create();
    Timerlayer->setContentSize(Size(32,8));
    Timerlayer->setPosition(0,0);
    //ClockLayer->setOpacity
    this->addChild(Timerlayer);
    Timerlayer->initLayer();
    
    return true;
}

bool TimerLayer::initLayer()
{
    auto listener = EventListenerButton::create();
    listener ->onBtnDuringLongPress = DT_CALLBACK_2(TimerLayer::BtnDuringLongPressHandler,this);
    listener ->onBtnLongPressStart = DT_CALLBACK_2(TimerLayer::BtnLongPressStartHandler,this);
    listener ->onBtnClick = DT_CALLBACK_2(TimerLayer::BtnClickHandler,this);   
    listener ->onBtnDoubleClick = DT_CALLBACK_2(TimerLayer::BtnDoubleClickHandler,this);  
    _eventDispatcher->addEventListenerWithSceneGraphPriority ( listener, this );
    if(false == GlobalTimer.boTiming)
    {
        GlobalTimer.u8MSec = 0;
        GlobalTimer.u8Min = 0;
        GlobalTimer.u8Sec = 0;
    }
    DTRGB TextColor = {255,255,255};
    MSec = TextSprite::create(Size(8,5),Size(8,5),TextColor,(std::to_string(GlobalTimer.u8MSec/10)+std::to_string(GlobalTimer.u8MSec%10)),TextSprite::TextAlign::TextAlignRight,&TomThumb);
    MSeccanvas = MSec->getSpriteCanvas();
    TimePt1 = TextSprite::create(Size(2,5),Size(2,5),TextColor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    TimePt1canvas = TimePt1->getSpriteCanvas();
    Min = TextSprite::create(Size(8,5),Size(8,5),TextColor,(std::to_string(GlobalTimer.u8Min/10) + std::to_string(GlobalTimer.u8Min%10)),TextSprite::TextAlign::TextAlignRight,&TomThumb);
    Mincanvas = Min->getSpriteCanvas();
    TimePt2 = TextSprite::create(Size(2,5),Size(2,5),TextColor,".",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    TimePt2canvas = TimePt2->getSpriteCanvas();
    Sec = TextSprite::create(Size(8,5),Size(8,5),TextColor,(std::to_string(GlobalTimer.u8Sec/10) + std::to_string(GlobalTimer.u8Sec%10)),TextSprite::TextAlign::TextAlignRight,&TomThumb);
    Seccanvas = Sec->getSpriteCanvas();
    MSec->setPosition(23,1);
    TimePt1->setPosition(11,1);
    Min->setPosition(3,1);
    TimePt2->setPosition(21,1);    
    Sec->setPosition(13,1);
    this->addChild(MSec);
    this->addChild(TimePt1);
    this->addChild(Min);
    this->addChild(TimePt2);
    this->addChild(Sec);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(TimerLayer::TimerUpdate),0.03);
    if(nullptr == TimerCounter)
    {
        TimerCounter = xTimerCreate
                   ( /* Just a text name, not used by the RTOS
                     kernel. */
                     "FeatureTimer",
                     /* The timer period in ticks, must be
                     greater than 0. */
                     (portTICK_PERIOD_MS*10),
                     /* The timers will auto-reload themselves
                     when they expire. */
                     pdTRUE,
                     /* The ID is used to store a count of the
                     number of times the timer has expired, which
                     is initialised to 0. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when
                     it expires. */
                     TimerCounterCb
                   );
    }
    
    return true;
}



void TimerLayer::TimerUpdate(float dt)
{
    static tstTimerType TimerPre = GlobalTimer;
    if(GlobalTimer.boTiming)
    {
        if(0 == TimePt1->getNumberOfRunningActions())
        {
            TimePt1->runAction(RepeatForever::create(Blink::create(1,1)));
        }        
    }
    else
    {
        if(0 != TimePt1->getNumberOfRunningActions())
        {
            TimePt1->stopAllActions();
            TimePt1->setVisible(true);
        }        
    }
    if(TimerPre.u8MSec != GlobalTimer.u8MSec)
    {
        MSeccanvas->canvasReset();
        MSeccanvas->print((std::to_string(GlobalTimer.u8MSec/10)+std::to_string(GlobalTimer.u8MSec%10)).c_str());
    }
    if(TimerPre.u8Min != GlobalTimer.u8Min)
    {
        Mincanvas->canvasReset();
        Mincanvas->print((std::to_string(GlobalTimer.u8Min/10) + std::to_string(GlobalTimer.u8Min%10)).c_str());
    }
    if(TimerPre.u8Sec != GlobalTimer.u8Sec)
    {
        Seccanvas->canvasReset();
        Seccanvas->print((std::to_string(GlobalTimer.u8Sec/10) + std::to_string(GlobalTimer.u8Sec%10)).c_str());
    }
    TimerPre = GlobalTimer;
}

void TimerLayer::BtnClickHandler(int8_t keyCode, Event* event)
{
    if(enKey_OK == keyCode)
    {   
        if(xTimerIsTimerActive(TimerCounter))
        {
            xTimerStop(TimerCounter,10);
            GlobalTimer.boTiming = false;
        }
        else
        {
            xTimerStart(TimerCounter,10);
            GlobalTimer.boTiming = true;
        }
    }
}

void TimerLayer::BtnDoubleClickHandler(int8_t keyCode, Event* event)
{

}

void TimerLayer::BtnLongPressStartHandler(int8_t keyCode, Event* event)
{
    if(enKey_OK == keyCode)
    {   
        if(xTimerIsTimerActive(TimerCounter))
        {
            xTimerStop(TimerCounter,10);
        }
        GlobalTimer.boTiming = false;
        GlobalTimer.u8MSec = 0;
        GlobalTimer.u8Min = 0;
        GlobalTimer.u8Sec = 0;
    }
}

void TimerLayer::BtnDuringLongPressHandler(int8_t keyCode, Event* event)
{

}


NS_DT_END