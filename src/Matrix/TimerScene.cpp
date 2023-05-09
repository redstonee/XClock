#include "TimerScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "FastLED.h"
#include "../Key/ClockKey.h"

NS_DT_BEGIN

typedef struct
{
    uint8_t u8Hour;
    uint8_t u8Min;
    uint8_t u8Sec;
    bool boTiming;
}tstTimerType;

tstTimerType GlobalTimer = {0,0,0,false};

/********************************Timer sence and layer****************************************/

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
        GlobalTimer.u8Hour = 0;
        GlobalTimer.u8Min = 0;
        GlobalTimer.u8Sec = 0;
    }
    DTRGB TextColor = {255,255,255};
    Hour = TextSprite::create(Size(8,5),Size(8,5),TextColor,(std::to_string(GlobalTimer.u8Hour/10)+std::to_string(GlobalTimer.u8Hour%10)),TextSprite::TextAlign::TextAlignRight,&TomThumb);
    Hourcanvas = Hour->getSpriteCanvas();
    TimePt1 = TextSprite::create(Size(2,5),Size(2,5),TextColor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    TimePt1canvas = TimePt1->getSpriteCanvas();
    Min = TextSprite::create(Size(8,5),Size(8,5),TextColor,(std::to_string(GlobalTimer.u8Min/10) + std::to_string(GlobalTimer.u8Min%10)),TextSprite::TextAlign::TextAlignRight,&TomThumb);
    Mincanvas = Min->getSpriteCanvas();
    TimePt2 = TextSprite::create(Size(2,5),Size(2,5),TextColor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    TimePt2canvas = TimePt1->getSpriteCanvas();
    Sec = TextSprite::create(Size(8,5),Size(8,5),TextColor,(std::to_string(GlobalTimer.u8Sec/10) + std::to_string(GlobalTimer.u8Sec%10)),TextSprite::TextAlign::TextAlignRight,&TomThumb);
    Seccanvas = Min->getSpriteCanvas();
    Hour->setPosition(2,1);
    TimePt1->setPosition(10,1);
    Min->setPosition(12,1);
    TimePt2->setPosition(20,1);    
    Sec->setPosition(22,1);
    this->addChild(Hour);
    this->addChild(TimePt1);
    this->addChild(Min);
    this->addChild(TimePt2);
    this->addChild(Sec);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(TimerLayer::TimerUpdate),0.1);
    return true;
}

void TimerLayer::TimerUpdate(float dt)
{
    static tstTimerType TimerPre = GlobalTimer;
    if(GlobalTimer.boTiming)
    {
        TimePt2->runAction(RepeatForever::create(Blink::create(1,1)));
        if(TimerPre.u8Hour != GlobalTimer.u8Hour)
        {
            Hourcanvas->canvasReset();
            Hourcanvas->print((std::to_string(GlobalTimer.u8Hour/10)+std::to_string(GlobalTimer.u8Hour%10)).c_str());
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
    }
    else
    {
        TimePt2->stopAllActions();
        TimePt2->setVisible(true);
    }
}

void TimerLayer::BtnClickHandler(int8_t keyCode, Event* event)
{

}

void TimerLayer::BtnDoubleClickHandler(int8_t keyCode, Event* event)
{

}

void TimerLayer::BtnLongPressStartHandler(int8_t keyCode, Event* event)
{

}

void TimerLayer::BtnDuringLongPressHandler(int8_t keyCode, Event* event)
{

}


NS_DT_END