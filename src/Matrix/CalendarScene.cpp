#include "CalendarScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "../RTC/SD3078.h"
#include "../main.h"
#include "FastLED.h"
#include "../Key/ClockKey.h"

NS_DT_BEGIN


bool CalYearScene::init()
{
    YearLayer *YearLayer = YearLayer::create();
    YearLayer->setContentSize(Size(32,8));
    YearLayer->setPosition(0,0);
    //ClockLayer->setOpacity
    this->addChild(YearLayer);
    YearLayer->initLayer();
    return true;
}

bool YearLayer::initLayer()
{
    CRGBPalette16 currentPalette( RainbowColors_p );
    ClockTime = stGetCurTime();
    TimeSettingQ = pGetTimeSettingQ();
    ColorIndex = 80;  
    CRGB ColorFromPat = ColorFromPalette( currentPalette, ColorIndex);
    DTRGB Color;
    Color.r = ColorFromPat.r;
    Color.g = ColorFromPat.g;
    Color.b = ColorFromPat.b;
    auto listener = EventListenerButton::create();
    listener ->onBtnDuringLongPress = DT_CALLBACK_2(YearLayer::BtnDuringLongPressHandler,this);
    listener ->onBtnLongPressStart = DT_CALLBACK_2(YearLayer::BtnLongPressStartHandler,this);
    listener ->onBtnClick = DT_CALLBACK_2(YearLayer::BtnClickHandler,this);    
    _eventDispatcher->addEventListenerWithSceneGraphPriority ( listener, this );
    FrameSprite* CalendarIcon = FrameSprite::create(icon_calendar,sizeof(icon_calendar),BMP_GIF);
    CalendarIcon->setPosition(0,0);
    CalendarIcon->setAutoSwitch(true);
    std::string year = std::to_string(ClockTime.u8Year>>4) + std::to_string((ClockTime.u8Year&0x0f));
    YearH = TextSprite::create(Size(8,5),Size(8,5),Color,"20",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    YearHcanvas = YearH->getSpriteCanvas();
    YearL = TextSprite::create(Size(8,5),Size(8,5),Color,year,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    YearLcanvas = YearL->getSpriteCanvas();
    YearH->setTransparent(true);
    YearH->setPosition(12,1);
    YearL->setTransparent(true);    
    YearL->setPosition(20,1);
    this->addChild(CalendarIcon);
    this->addChild(YearH);
    this->addChild(YearL);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(YearLayer::YearUpdate),0.5);
    return true;
}

void YearLayer::BtnClickHandler(int8_t keyCode, Event* event)
{
    if(true == boYearSetting)
    {
        uint8_t year_temp = (ClockTimeSetting.u8Year>>4)*10 + (ClockTimeSetting.u8Year&0x0f);
        if(enKey_OK == keyCode)
        {
            boYearSetting = false;
            SendSettingYear(&ClockTimeSetting);
        }
        else if(enKey_Left == keyCode)
        {
            if(--year_temp > 99)
            {
                year_temp = 99;
            }
        }
        else if(enKey_Right == keyCode)
        {
            if(++year_temp > 99)
            {
                year_temp = 0;
            }
        }
        ClockTimeSetting.u8Year = ((year_temp/10)<<4) + (year_temp%10);
    }
    else
    {

    }
}

void YearLayer::BtnLongPressStartHandler(int8_t keyCode , Event * event )
{
    if(enKey_OK == keyCode)
    {
        boYearSetting = true;
        ClockTimeSetting = ClockTime;
    }
}

void YearLayer::BtnDuringLongPressHandler(int8_t keyCode, Event* event)
{
    if(true == boYearSetting)
    {
        uint8_t year_temp = (ClockTimeSetting.u8Year>>4)*10 + (ClockTimeSetting.u8Year&0x0f);
        if(enKey_Left == keyCode)
        {
            if(--year_temp > 99)
            {
                year_temp = 99;
            }
        }
        else if(enKey_Right == keyCode)
        {
            if(++year_temp > 99)
            {
                year_temp = 0;
            }
        }
        ClockTimeSetting.u8Year = ((year_temp/10)<<4) + (year_temp%10);
    }
    else
    {

    }
}

void YearLayer::SendSettingYear(tst3078Time* settingtime)
{
    if(TimeSettingQ != nullptr)
    {
        if( xQueueSend( TimeSettingQ,
                       ( void * ) settingtime,
                       ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
        }
    }
}

void YearLayer::YearUpdate(float dt)
{
    tst3078Time time;
    time = stGetCurTime();
    static tst3078Time OldSettingTime = {0,};
    if(true == boYearSetting)
    {
        if(0 == YearL->getNumberOfRunningActions())
        {
            YearL->runAction(RepeatForever::create(Blink::create(1,2)));
        }
        if(ClockTimeSetting.u8Year != OldSettingTime.u8Year)
        {
            std::string year = std::to_string(ClockTimeSetting.u8Year>>4) + std::to_string((ClockTimeSetting.u8Year&0x0f));
            YearLcanvas->canvasReset();
            YearLcanvas->print(year.c_str());         
        }
    }
    else
    {
        if(0 != YearL->getNumberOfRunningActions())
        {
            ClockTime = time;
            YearL->stopAllActions();            
            std::string year = std::to_string(ClockTime.u8Year>>4) + std::to_string((ClockTime.u8Year&0x0f));
            YearLcanvas->canvasReset();
            YearLcanvas->print(year.c_str());  
            YearL->setVisible(true);
        }
    }
}

NS_DT_END