#include "ClockScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "../RTC/SD3078.h"
NS_DT_BEGIN

bool ClockScene::init()
{
    TimeLayer *ClockLayer = TimeLayer::create();
    ClockLayer->setContentSize(Size(22,5));
    ClockLayer->setPosition(9,1);
    //ClockLayer->setOpacity
    this->addChild(ClockLayer);
    ClockLayer->initLayer();
    return true;
}

bool TimeLayer::initLayer()
{
    SD3078Time = new SD3078();
    SD3078Time->ReadTime(&ClockTime);
    std::string hour1 = std::to_string((ClockTime.u8Hour&0x70)>>4);
    std::string hour2 = std::to_string((ClockTime.u8Hour&0x0f));
    std::string min1 = std::to_string((ClockTime.u8Min&0xf0)>>4);
    std::string min2 = std::to_string((ClockTime.u8Min&0x0f));
    DTRGB timecolor = DTRGB(0,255,0);
    Hour_1 = TextSprite::create(Size(4,5),Size(4,5),timecolor,hour1,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Hour1canvas = Hour_1->getSpriteCanvas();
    Hour_2 = TextSprite::create(Size(4,5),Size(4,5),timecolor,hour2,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Hour2canvas = Hour_2->getSpriteCanvas();
    MinPt = TextSprite::create(Size(2,5),Size(2,5),timecolor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Min_1 = TextSprite::create(Size(4,5),Size(4,5),timecolor,min1,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Min1canvas = Min_1->getSpriteCanvas();
    Min_2 = TextSprite::create(Size(4,5),Size(4,5),timecolor,min2,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Min2canvas = Min_2->getSpriteCanvas();
    Hour_1->setTransparent(true);
    Hour_1->setPosition(0,0);
    Hour_2->setTransparent(true);    
    Hour_2->setPosition(4,0);
    MinPt->setTransparent(true);
    MinPt->setPosition(8,0);
    Blink *TimeBlink = Blink::create(1,1);
    MinPt->runAction(RepeatForever::create(TimeBlink));
    Min_1->setTransparent(true);
    Min_1->setPosition(10,0);
    Min_2->setTransparent(true);
    Min_2->setPosition(14,0);
    this->addChild(Hour_1);
    this->addChild(Hour_2);
    this->addChild(MinPt);
    this->addChild(Min_1);
    this->addChild(Min_2);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(TimeLayer::TimeUpdate),0.5);
    //MoveBy *moveBy1 = MoveBy::create(0.5,Vec2(0,5));
    //MoveBy *moveBy2 = MoveBy::create(0.5,Vec2(0,5));
    //SpriteCanvas *TestSprite = Min_2->getSpriteCanvas();
    //Min_2->runAction(moveBy1);
    //Min_3->runAction(moveBy2);
    return true;
}

void TimeLayer::TimeUpdate(float dt)
{
    tst3078Time time;
    SD3078Time->ReadTime(&time);
    if(time.u8Hour != ClockTime.u8Hour)
    {
        std::string hour1 = std::to_string((time.u8Hour&0x70)>>4);
        std::string hour2 = std::to_string((time.u8Hour&0x0f));
        Hour1canvas->canvasReset();
        Hour1canvas->print(hour1.c_str());
        Hour2canvas->canvasReset();
        Hour2canvas->print(hour2.c_str());
    }
    if(time.u8Min != ClockTime.u8Min)
    {
        std::string min1 = std::to_string((time.u8Min&0xf0)>>4);
        std::string min2 = std::to_string((time.u8Min&0x0f));
        Min1canvas->canvasReset();
        Min1canvas->print(min1.c_str());
        Min2canvas->canvasReset();
        Min2canvas->print(min2.c_str());
    }
    ClockTime = time;
    //Serial.printf("Time: %x:%x:%x:%x:%x:%x:%x\n",ClockTime.u8Year,ClockTime.u8Month,ClockTime.u8Day,ClockTime.u8Week,ClockTime.u8Hour,ClockTime.u8Min,ClockTime.u8Sec);
}
NS_DT_END