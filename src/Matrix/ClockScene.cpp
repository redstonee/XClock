#include "ClockScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "web.h"
#include "main.h"
#include "Palette.h"
#include "Icon.h"
#include "ClockKey.h"
#include "AlarmClk.h"

NS_DT_BEGIN

#define WeekColorIdxOffset       (80)

bool ClockScene::init()
{
    TimeLayer *ClockLayer = TimeLayer::create();
    ClockLayer->setContentSize(Size(32,8));
    ClockLayer->setPosition(0,0);
    //ClockLayer->setOpacity
    this->addChild(ClockLayer);
    ClockLayer->initLayer();
    return true;
}

const unsigned char* pGetWeatherIcon(uint8_t weathercode,uint32_t *Length)
{
    const unsigned char *point = nullptr;
    /**https://seniverse.yuque.com/hyper_data/api_v3/yev2c3#M1KBK*/
    switch(weathercode)
    {
        case 0:
            point = icon_Sunny;
            *Length = sizeof(icon_Sunny);
            break;
        case 1:
            point = icon_Clear;
            *Length = sizeof(icon_Clear);
            break;
        case 2:
            point = icon_Sunny;
            *Length = sizeof(icon_Sunny);
            break;
        case 3:
            point = icon_Clear;
            *Length = sizeof(icon_Clear);
            break;
        case 4:
            point = icon_Cloudly;
            *Length = sizeof(icon_Cloudly);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            point = icon_PartCloudy;
            *Length = sizeof(icon_PartCloudy);
            break;
        case 9:
            point = icon_Overcase;
            *Length = sizeof(icon_Overcase);
            break;
        case 10:
            point = icon_Shower;
            *Length = sizeof(icon_Shower);
            break;
        case 11:
            point = icon_ThunderShower;
            *Length = sizeof(icon_ThunderShower);
            break;
        case 12:
            point = icon_ThunderShower;
            *Length = sizeof(icon_ThunderShower);
            break;
        case 13:
            point = icon_LightRain;
            *Length = sizeof(icon_LightRain);
            break;
        case 14:
            point = icon_HeavyRain;
            *Length = sizeof(icon_HeavyRain);
            break;
        case 15:
        case 16:
        case 17:
        case 18:
            point = icon_Storm;
            *Length = sizeof(icon_Storm);
            break;
        case 19:
        case 20:
            point = icon_HeavyRain;
            *Length = sizeof(icon_HeavyRain);
            break;
        case 21:
            point = icon_SnowFlurry;
            *Length = sizeof(icon_SnowFlurry);
            break;
        case 22:
        case 23:
        case 24:
        case 25:
            point = icon_Snow;
            *Length = sizeof(icon_Snow);
            break;
        case 26:
        case 27:
        case 28:
        case 29:
            point = icon_Sand;
            *Length = sizeof(icon_Sand);
            break;
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
            point = icon_Foggy;
            *Length = sizeof(icon_Foggy);
            break;
        case 35:
            point = icon_Sunny;
            *Length = sizeof(icon_Sunny);
            break;
        case 36:
            point = icon_Foggy;
            *Length = sizeof(icon_Foggy);
            break;
        case 37:
        case 38:
            point = icon_Sunny;
            *Length = sizeof(icon_Sunny);
            break;
        default:
            point = icon_Sunny;
            *Length = sizeof(icon_Sunny);
            break;
    }
    return point;
}

void TimeLayer::BtnClickHandler(int8_t keyCode, Event* event)
{
    TimeStateMachine(keyCode,enKey_ShortPress);
}

void TimeLayer::BtnLongPressStartHandler(int8_t keyCode , Event * event )
{
    TimeStateMachine(keyCode,enKey_LongPressStart);
}

void TimeLayer::BtnDuringLongPressHandler(int8_t keyCode, Event* event)
{
    TimeStateMachine(keyCode,enKey_LongPress);
}

void TimeLayer::BtnDuringLongPressStopHandler(int8_t keyCode, Event* event)
{
    TimeStateMachine(keyCode,enKey_LongPressStop);
}

void TimeLayer::TimeStateMachine(int8_t key_type, int8_t key_event)
{
    uint8_t min_temp = (ClockTimeSetting.u8Min>>4)*10 + (ClockTimeSetting.u8Min&0x0f);
    uint8_t hour_temp = ((ClockTimeSetting.u8Hour&0x30)>>4)*10 + (ClockTimeSetting.u8Hour&0x0f);/*bit7 means 12h/24h, need ignore*/
    switch(enTimests)
    {
        case State_TimeDis:
            if(key_type == enKey_OK)
            {
                if(key_event == enKey_LongPressStart)
                {
                    enTimests = State_TimeSetMin;
                    ClockTimeSetting = ClockTime;
                }
                else if(key_event == enKey_ShortPress)
                {
                    PaletteIndex++;
                    ColorIndex = 0; //reset the color index when change the palette
                    if(false == boSetGlobalPaltIdx(PaletteIndex))//over the maxium palette size return false,restart from 0;
                    {
                        Serial.printf("Store Palette fail\n\r");
                        PaletteIndex = 0;
                        boSetGlobalPaltIdx(PaletteIndex);
                    }
                    boSetGlobalColorIdx(ColorIndex);
                    PaletteIndex = u8GetGlobalPaltIdx();
                    currentPalette = pGetPalette(PaletteIndex);
                    UpdateColor(key_type,key_event);
                    
                }
            }
            else if(key_type == enKey_Left || key_type == enKey_Right)
            {
                if(key_event == enKey_LongPress || key_event == enKey_ShortPress || key_event == enKey_LongPressStop)
                {
                    UpdateColor(key_type,key_event);
                }
            }
            break;
        case State_TimeSetMin:            
            if(key_type == enKey_OK)
            {
                if(key_event == enKey_ShortPress)
                {
                    enTimests = State_TimeSetHour;
                }
            }
            else if(key_type == enKey_Left)
            {
                
                if(--min_temp > 59)
                {
                   min_temp = 59;
                }
            }
            else if(key_type == enKey_Right)
            {
                if(++min_temp > 59)
                {
                    min_temp = 0;
                }
            }
            ClockTimeSetting.u8Min = ((min_temp/10)<<4) + (min_temp%10);
            break;
        case State_TimeSetHour:
            if(key_type == enKey_OK)
            {
                if(key_event == enKey_ShortPress)
                {
                    enTimests = State_TimeSetWeek;
                }
            }
            else if(key_type == enKey_Left)
            {
                if(--hour_temp > 23)
                {
                    hour_temp = 23;
                }
            }
            else if(key_type == enKey_Right)
            {
                if(++hour_temp > 23)
                {
                    hour_temp = 0;
                }
            }
            ClockTimeSetting.u8Hour = (((hour_temp/10)<<4) + (hour_temp%10))|0x80; //24h format
            break;
        case State_TimeSetWeek:
            if(key_type == enKey_OK)
            {
                if(key_event == enKey_ShortPress)
                {
                    enTimests = State_TimeDis;
                    SendSettingTime(&ClockTimeSetting);
                }
            }
            else if(key_type == enKey_Left)
            {
                if(--ClockTimeSetting.u8Week > 6)
                {
                    ClockTimeSetting.u8Week = 6;
                }
            }
            else if(key_type == enKey_Right)
            {
                if(++ClockTimeSetting.u8Week > 6)
                {
                    ClockTimeSetting.u8Week = 0;
                }
            }
            break;
        default:break;
    }
}

void TimeLayer::UpdateColor(int8_t key_type, int8_t key_event)
{
    
    if(key_type == enKey_Left)
    {
        ColorIndex++;
    }
    else if(key_type == enKey_Right)
    {
        ColorIndex--;
    }
    if(key_event == enKey_LongPressStop || key_event == enKey_ShortPress)
    {
        boSetGlobalColorIdx(ColorIndex);           //store the color index when long press stop or short press
    }
    Serial.printf("PaletteIndex %d \n\r",PaletteIndex);
    Serial.printf("ColorIndex %d \n\r",ColorIndex);
    CRGB ColorFromPat = ColorFromPalette( currentPalette, ColorIndex);
    timecolor.r = ColorFromPat.r;
    timecolor.g = ColorFromPat.g;
    timecolor.b = ColorFromPat.b;
    // ColorFromPat = ColorFromPalette( currentPalette, ColorIndex+WeekColorIdxOffset);
    // weekcolor.r = ColorFromPat.r;
    // weekcolor.g = ColorFromPat.g;
    // weekcolor.b = ColorFromPat.b;
    if((timecolor.r > 235) && (timecolor.g > 235) && (timecolor.b > 235))
    {
        weekcolor.r = 0;
        weekcolor.g = 255;
        weekcolor.b = 0;
    }
    else
    {
        weekcolor.r = 100;
        weekcolor.g = 100;
        weekcolor.b = 100;
    }
    Serial.printf("Color r:%d g:%d b%d \n\r",timecolor.r, timecolor.g , timecolor.b);
    Hour1canvas->setTextColor(timecolor);
    Hour2canvas->setTextColor(timecolor);
    //MinPt->setColor(timecolor);
    Min1canvas->setTextColor(timecolor);
    Min2canvas->setTextColor(timecolor);
    std::string hour1 = std::to_string((ClockTime.u8Hour&0x70)>>4);
    std::string hour2 = std::to_string((ClockTime.u8Hour&0x0f));
    Hour1canvas->canvasReset();
    Hour1canvas->print(hour1.c_str());
    Hour2canvas->canvasReset();
    Hour2canvas->print(hour2.c_str());
    std::string min1 = std::to_string((ClockTime.u8Min&0xf0)>>4);
    std::string min2 = std::to_string((ClockTime.u8Min&0x0f));
    Min1canvas->canvasReset();
    Min1canvas->print(min1.c_str());
    Min2canvas->canvasReset();
    Min2canvas->print(min2.c_str());
    MinPtcanvas->setTextColor(timecolor);
    MinPtcanvas->canvasReset();
    MinPtcanvas->print(":");
    DrawWeek(ClockTime.u8Week);
}

void TimeLayer::SendSettingTime(tst3078Time* settingtime)
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

void TimeLayer::DrawWeek(uint8_t week)
{
    Serial.printf("week %d \n\r",week);
    DTRGB inactivecol = timecolor;
    inactivecol.fadeToBlackBy(200);
    if(Weekcanvas != nullptr)
    {
        for(uint8_t i = 0;i < 7;i++)
        {
            if(week !=0) /*0 means sunday*/
            {
                if(i != (week-1))
                {
                    Weekcanvas->drawLine(i*3,0,i*3 + 1,0,inactivecol);
                }
                else
                {
                    Weekcanvas->drawLine(i*3,0,i*3 + 1,0,timecolor);
                }
            }
            else
            {
                if(i != 6)
                {
                    Weekcanvas->drawLine(i*3,0,i*3 + 1,0,inactivecol);
                }
                else
                {
                    Weekcanvas->drawLine(6*3,0,6*3 + 1,0,timecolor);
                }
            }
        }
    }
}

bool TimeLayer::initLayer()
{
    ClockTime = stGetCurTime();  
    TimeSettingQ = pGetTimeSettingQ();
    ColorIndex = u8GetGlobalColorIdx();
    PaletteIndex = u8GetGlobalPaltIdx();
    Serial.printf("PaletteIndex %d \n\r",PaletteIndex);
    currentPalette = pGetPalette(PaletteIndex);
    CRGB ColorFromPat = ColorFromPalette( currentPalette, ColorIndex);
    auto listener = EventListenerButton::create();
    listener ->onBtnDuringLongPress = DT_CALLBACK_2(TimeLayer::BtnDuringLongPressHandler,this);
    listener ->onBtnLongPressStop = DT_CALLBACK_2(TimeLayer::BtnDuringLongPressStopHandler,this);
    listener ->onBtnLongPressStart = DT_CALLBACK_2(TimeLayer::BtnLongPressStartHandler,this);
    listener ->onBtnClick = DT_CALLBACK_2(TimeLayer::BtnClickHandler,this);    
    _eventDispatcher->addEventListenerWithSceneGraphPriority ( listener, this );
    std::string hour1 = std::to_string((ClockTime.u8Hour&0x70)>>4);
    std::string hour2 = std::to_string((ClockTime.u8Hour&0x0f));
    std::string min1 = std::to_string((ClockTime.u8Min&0xf0)>>4);
    std::string min2 = std::to_string((ClockTime.u8Min&0x0f));
    uint8_t WeatherCode = 0;
    WeatherCode = (uint8_t)GetCurWeatherCode();
    Serial.printf("Weather code:%d\n\r", WeatherCode);
    uint32_t gifsize = 0;
    const unsigned char *picon = pGetWeatherIcon(WeatherCode,&gifsize);
    Serial.printf("Weather icon:%x\n\r", picon);
    Weather = FrameSprite::create(picon,gifsize,BMP_GIF);
    Weather->setPosition(0,0);
    Weather->setAutoSwitch(true);
    timecolor.r = ColorFromPat.r;
    timecolor.g = ColorFromPat.g;
    timecolor.b = ColorFromPat.b;
    //ColorFromPat = ColorFromPalette( currentPalette, ColorIndex+WeekColorIdxOffset);
    if((timecolor.r > 235) && (timecolor.g > 235) && (timecolor.b > 235))
    {
        weekcolor.r = 0;
        weekcolor.g = 255;
        weekcolor.b = 0;
    }
    else
    {
        weekcolor.r = 255;
        weekcolor.g = 255;
        weekcolor.b = 255;
    }
    
    Hour_1 = TextSprite::create(Size(4,5),Size(4,5),timecolor,hour1,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Hour1canvas = Hour_1->getSpriteCanvas();
    Hour_2 = TextSprite::create(Size(4,5),Size(4,5),timecolor,hour2,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Hour2canvas = Hour_2->getSpriteCanvas();
    MinPt = TextSprite::create(Size(2,5),Size(2,5),timecolor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    MinPtcanvas = MinPt->getSpriteCanvas();
    Min_1 = TextSprite::create(Size(4,5),Size(4,5),timecolor,min1,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Min1canvas = Min_1->getSpriteCanvas();
    Min_2 = TextSprite::create(Size(4,5),Size(4,5),timecolor,min2,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Min2canvas = Min_2->getSpriteCanvas();
    Week = CanvasSprite::create(21,1);
    Weekcanvas = Week->getSpriteCanvas();
    AlarmIcon = CanvasSprite::create(1,8);
    AlarmIconCanvas = AlarmIcon->getSpriteCanvas();
    AlarmIcon->setPosition(31,0);
    AlarmIconCanvas->drawLine(0,0,0,7,DTRGB(0,255,0));
    AlarmIcon->setVisible(false);    
    Hour_1->setTransparent(true);
    Hour_1->setPosition(11,1);
    Hour_2->setTransparent(true);    
    Hour_2->setPosition(15,1);
    MinPt->setTransparent(true);
    MinPt->setPosition(19,1);
    MinPt->runAction(RepeatForever::create(Blink::create(1,1)));
    Min_1->setTransparent(true);
    Min_1->setPosition(21,1);
    Min_2->setTransparent(true);
    Min_2->setPosition(25,1);
    Week->setPosition(10,7);
    DrawWeek(ClockTime.u8Week);
    this->addChild(Weather);
    this->addChild(Hour_1);
    this->addChild(Hour_2);
    this->addChild(MinPt);
    this->addChild(Min_1);
    this->addChild(Min_2);
    this->addChild(Week,3);
    this->addChild(AlarmIcon);  
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(TimeLayer::TimeUpdate),0.1);
    //MoveBy *moveBy1 = MoveBy::create(0.5,Vec2(0,5));
    //MoveBy *moveBy2 = MoveBy::create(0.5,Vec2(0,5));
    //SpriteCanvas *TestSprite = Min_2->getSpriteCanvas();
    //Min_2->runAction(moveBy1);
    //Min_3->runAction(moveBy2);
    return true;
}

void TimeLayer::CleanUpAnimationTmp(void)
{
    auto Hour1SprtTmp = getChildByTag(Hour1AniTmpTag);
    if(Hour1SprtTmp)
    {
        if(0 == Hour1SprtTmp->getNumberOfRunningActions())
        {
            removeChild(Hour1SprtTmp);
        }
    }
    auto Hour2SprtTmp = getChildByTag(Hour2AniTmpTag);
    if(Hour2SprtTmp)
    {
        if(0 == Hour2SprtTmp->getNumberOfRunningActions())
        {
            removeChild(Hour2SprtTmp);
        }
    }
    auto Min1SprtTmp = getChildByTag(Min1AniTmpTag);
    if(Min1SprtTmp)
    {
        if(0 == Min1SprtTmp->getNumberOfRunningActions())
        {
            removeChild(Min1SprtTmp);
        }
    }
    auto Min2SprtTmp = getChildByTag(Min2AniTmpTag);
    if(Min2SprtTmp)
    {
        if(0 == Min2SprtTmp->getNumberOfRunningActions())
        {
            removeChild(Min2SprtTmp);
        }
    }
}

void TimeLayer::DigitalSwitchAnimation(TextSprite* OldDigitalSprt,uint8_t OldDigital, uint8_t NewDigital, int tmpTag)
{
    TextSprite * SprtTmp = TextSprite::create(Size(4,5),Size(4,5),timecolor,std::to_string(OldDigital),TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    SprtTmp->setPosition(OldDigitalSprt->getPositionX(),OldDigitalSprt->getPositionY());
    this->addChild(SprtTmp,0,tmpTag);
    Vec2 TgtPos(OldDigitalSprt->getPositionX(), 1);
    OldDigitalSprt->setPosition(OldDigitalSprt->getPositionX(),-6);
    SpriteCanvas *OldCanvas = OldDigitalSprt->getSpriteCanvas();
    OldCanvas->canvasReset();
    OldCanvas->print(std::to_string(NewDigital).c_str());
    MoveBy* SwitchMove1 = MoveBy::create(0.5,Vec2(0,7));
    MoveTo* SwitchMove2 = MoveTo::create(0.5,TgtPos);
    Sequence *Seq = Sequence::createWithTwoActions(SwitchMove1, CallFunc::create(DT_CALLBACK_0(TimeLayer::CleanUpAnimationTmp,this)));
    //Serial.printf("TmpSprt %x \n",SprtTmp);
    SprtTmp->runAction(Seq);
    //SprtTmp->runAction(SwitchMove1);
    OldDigitalSprt->runAction(SwitchMove2);
}

bool boActiveAlarm(void)
{
    bool res = false;
    uint8_t AlarmNum = u8GetAlarmClkNum();
    tstAlarmClk AlarmClkTmp = {0,};
    if(AlarmNum)
    {
        for(uint8_t i = 0; i < AlarmNum; i++)
        {
            AlarmClkTmp = stGetAlarmClk(i);
            if(AlarmClkTmp.stAlarmSts == enAlarmSts_Alarming)
            {
                res = true;
                break;
            }  
        }
    }
    return res;    
}

void TimeLayer::StateTimeDisShow(void)
{
    static uint8_t WeatherCodeOld = 0;
    uint8_t WeatherCode = 0;
    WeatherCode = (uint8_t)GetCurWeatherCode();
    tst3078Time time = stGetCurTime();
    bool boReEnterflag = false;
    static bool boPreAlarmActive = false;
    bool boAlarmActive = boActiveAlarm();
    uint32_t gifsize = 0;
    const unsigned char *picon = pGetWeatherIcon(WeatherCode,&gifsize);
    if(WeatherCodeOld != WeatherCode)
    {
        Weather->setSpriteFrame(SpriteFrame::create(picon,gifsize,BMP_GIF));
    }
    if(0 != Week->getNumberOfRunningActions())
    {
        Week->stopAllActions();
        boReEnterflag = true;
    }
    if(0 == MinPt->getNumberOfRunningActions())
    {
        MinPt->runAction(RepeatForever::create(Blink::create(1,1)));
    }
    if(boReEnterflag == true)
    {
        std::string hour1 = std::to_string((time.u8Hour&0x70)>>4);
        std::string hour2 = std::to_string((time.u8Hour&0x0f));
        Hour1canvas->canvasReset();
        Hour1canvas->print(hour1.c_str());
        Hour2canvas->canvasReset();
        Hour2canvas->print(hour2.c_str());
        std::string min1 = std::to_string((time.u8Min&0xf0)>>4);
        std::string min2 = std::to_string((time.u8Min&0x0f));
        Min1canvas->canvasReset();
        Min1canvas->print(min1.c_str());
        Min2canvas->canvasReset();
        Min2canvas->print(min2.c_str());
        Weekcanvas->canvasReset();
        DrawWeek(time.u8Week);
        Hour_1->setVisible(true);
        Hour_2->setVisible(true);
        Min_1->setVisible(true);
        Min_2->setVisible(true);
        Week->setVisible(true);
        MinPt->setVisible(true);
    }
    if(boPreAlarmActive != boAlarmActive)
    {
        if(boAlarmActive)
        {
            AlarmIcon->runAction(RepeatForever::create(Blink::create(1,1)));
            AlarmIcon->setVisible(true);   
            Serial.printf("Alarming \n\r");        
        }
        else
        {
            AlarmIcon->stopAllActions();
            AlarmIcon->setVisible(false);   
            Serial.printf("Alarming disable \n\r");
        }
    }

    if(time.u8Hour != ClockTime.u8Hour)
    {
        // std::string hour1 = std::to_string((time.u8Hour&0x70)>>4);
        // std::string hour2 = std::to_string((time.u8Hour&0x0f));
        // Hour1canvas->canvasReset();
        // Hour1canvas->print(hour1.c_str());
        // Hour2canvas->canvasReset();
        // Hour2canvas->print(hour2.c_str());
        if(((time.u8Hour&0x70)>>4) != ((ClockTime.u8Hour&0x70)>>4))
        {
            DigitalSwitchAnimation(Hour_1,((ClockTime.u8Hour&0x70)>>4),((time.u8Hour&0x70)>>4),Hour1AniTmpTag);
        }
        if((time.u8Hour&0x0f) != (ClockTime.u8Hour&0x0f))
        {
            DigitalSwitchAnimation(Hour_2,(ClockTime.u8Hour&0x0f),(time.u8Hour&0x0f),Hour2AniTmpTag);
        }
    }
    if(time.u8Min != ClockTime.u8Min)
    {
        // std::string min1 = std::to_string((time.u8Min&0xf0)>>4);
        // std::string min2 = std::to_string((time.u8Min&0x0f));
        // Min1canvas->canvasReset();
        // Min1canvas->print(min1.c_str());
        // Min2canvas->canvasReset();
        // Min2canvas->print(min2.c_str());
        if(((time.u8Min&0xf0)>>4) != ((ClockTime.u8Min&0xf0)>>4))
        {
            DigitalSwitchAnimation(Min_1,((ClockTime.u8Min&0xf0)>>4),((time.u8Min&0xf0)>>4),Min1AniTmpTag);
        }
        if((time.u8Min&0x0f) != (ClockTime.u8Min&0x0f))
        {
            DigitalSwitchAnimation(Min_2,(ClockTime.u8Min&0x0f),(time.u8Min&0x0f),Min2AniTmpTag);
        }
    }
    if(time.u8Week != ClockTime.u8Week)
    {
        Weekcanvas->canvasReset();
        DrawWeek(time.u8Week);
    }
    ClockTime = time;
    boPreAlarmActive = boAlarmActive;
    WeatherCodeOld = WeatherCode;
}

void TimeLayer::StateSetMinShow(void)
{
    bool boFirstEnterflag = false;
    static tst3078Time OldSettingTime = {0,};
    if(0 != MinPt->getNumberOfRunningActions())
    {
        MinPt->stopAllActions();
        boFirstEnterflag = true;
    }
    if(0 == Min_1->getNumberOfRunningActions())
    {
        Min_1->runAction(RepeatForever::create(Blink::create(1,2)));
    }
    if(0 == Min_2->getNumberOfRunningActions())
    {
        Min_2->runAction(RepeatForever::create(Blink::create(1,2)));
    }
    if(true == boFirstEnterflag)
    {
        std::string hour1 = std::to_string((ClockTimeSetting.u8Hour&0x70)>>4);
        std::string hour2 = std::to_string((ClockTimeSetting.u8Hour&0x0f));
        Hour1canvas->canvasReset();
        Hour1canvas->print(hour1.c_str());
        Hour2canvas->canvasReset();
        Hour2canvas->print(hour2.c_str());
        std::string min1 = std::to_string((ClockTimeSetting.u8Min&0xf0)>>4);
        std::string min2 = std::to_string((ClockTimeSetting.u8Min&0x0f));
        Min1canvas->canvasReset();
        Min1canvas->print(min1.c_str());
        Min2canvas->canvasReset();
        Min2canvas->print(min2.c_str());
        Weekcanvas->canvasReset();
        DrawWeek(ClockTimeSetting.u8Week);
        Hour_1->setVisible(true);
        Hour_2->setVisible(true);
        Min_1->setVisible(true);
        Min_2->setVisible(true);
        Week->setVisible(true);
        MinPt->setVisible(true);
    }
    if(ClockTimeSetting.u8Min != OldSettingTime.u8Min)
    {
        std::string min1 = std::to_string((ClockTimeSetting.u8Min&0xf0)>>4);
        std::string min2 = std::to_string((ClockTimeSetting.u8Min&0x0f));
        Min1canvas->canvasReset();
        Min1canvas->print(min1.c_str());
        Min2canvas->canvasReset();
        Min2canvas->print(min2.c_str());
        Weekcanvas->canvasReset();
        DrawWeek(ClockTimeSetting.u8Week);            
    }
    OldSettingTime = ClockTimeSetting;
}

void TimeLayer::StateSetHourShow(void)
{
    bool boFirstEnterflag = false;
    static tst3078Time OldSettingTime = {0,};
    if(0 != Min_1->getNumberOfRunningActions())
    {
        Min_1->stopAllActions();
        boFirstEnterflag = true;
    }
    if(0 != Min_2->getNumberOfRunningActions())
    {
        Min_2->stopAllActions();
    }
    if(0 == Hour_1->getNumberOfRunningActions())
    {
        Hour_1->runAction(RepeatForever::create(Blink::create(1,2)));
    }
    if(0 == Hour_2->getNumberOfRunningActions())
    {
        Hour_2->runAction(RepeatForever::create(Blink::create(1,2)));
    }
    if(true == boFirstEnterflag)
    {
        std::string hour1 = std::to_string((ClockTimeSetting.u8Hour&0x70)>>4);
        std::string hour2 = std::to_string((ClockTimeSetting.u8Hour&0x0f));
        Hour1canvas->canvasReset();
        Hour1canvas->print(hour1.c_str());
        Hour2canvas->canvasReset();
        Hour2canvas->print(hour2.c_str());
        std::string min1 = std::to_string((ClockTimeSetting.u8Min&0xf0)>>4);
        std::string min2 = std::to_string((ClockTimeSetting.u8Min&0x0f));
        Min1canvas->canvasReset();
        Min1canvas->print(min1.c_str());
        Min2canvas->canvasReset();
        Min2canvas->print(min2.c_str());
        Weekcanvas->canvasReset();
        DrawWeek(ClockTimeSetting.u8Week);
        Hour_1->setVisible(true);
        Hour_2->setVisible(true);
        Min_1->setVisible(true);
        Min_2->setVisible(true);
        Week->setVisible(true);
        MinPt->setVisible(true);
    }
    if(ClockTimeSetting.u8Hour != OldSettingTime.u8Hour)
    {
        std::string hour1 = std::to_string((ClockTimeSetting.u8Hour&0x70)>>4);
        std::string hour2 = std::to_string((ClockTimeSetting.u8Hour&0x0f));
        Hour1canvas->canvasReset();
        Hour1canvas->print(hour1.c_str());
        Hour2canvas->canvasReset();
        Hour2canvas->print(hour2.c_str());            
    }
    OldSettingTime = ClockTimeSetting;
}

void TimeLayer::StateSetWeekShow(void)
{
    bool boFirstEnterflag = false;
    static tst3078Time OldSettingTime = {0,};
    if(0 != Hour_1->getNumberOfRunningActions())
    {
        Hour_1->stopAllActions();
        boFirstEnterflag = true;
    }
    if(0 != Hour_2->getNumberOfRunningActions())
    {
        Hour_2->stopAllActions();
        boFirstEnterflag = true;
    }
    if(0 == Week->getNumberOfRunningActions())
    {
        Week->runAction(RepeatForever::create(Blink::create(1,2)));
    }
    if(true == boFirstEnterflag)
    {
        std::string hour1 = std::to_string((ClockTimeSetting.u8Hour&0x70)>>4);
        std::string hour2 = std::to_string((ClockTimeSetting.u8Hour&0x0f));
        Hour1canvas->canvasReset();
        Hour1canvas->print(hour1.c_str());
        Hour2canvas->canvasReset();
        Hour2canvas->print(hour2.c_str());
        std::string min1 = std::to_string((ClockTimeSetting.u8Min&0xf0)>>4);
        std::string min2 = std::to_string((ClockTimeSetting.u8Min&0x0f));
        Min1canvas->canvasReset();
        Min1canvas->print(min1.c_str());
        Min2canvas->canvasReset();
        Min2canvas->print(min2.c_str());
        Weekcanvas->canvasReset();
        DrawWeek(ClockTimeSetting.u8Week);
        Hour_1->setVisible(true);
        Hour_2->setVisible(true);
        Min_1->setVisible(true);
        Min_2->setVisible(true);
        Week->setVisible(true);
        MinPt->setVisible(true);
    }
    if(ClockTimeSetting.u8Week != OldSettingTime.u8Week)
    {
        Weekcanvas->canvasReset();
        DrawWeek(ClockTimeSetting.u8Week);        
    }
    OldSettingTime = ClockTimeSetting;
}

void TimeLayer::TimeUpdate(float dt)
{   
    if(enTimests == State_TimeDis)
    {   
        StateTimeDisShow();
    }
    else if(enTimests == State_TimeSetMin)
    {
        StateSetMinShow();
    }
    else if(enTimests == State_TimeSetHour)
    {
        StateSetHourShow();
    }
    else if(enTimests == State_TimeSetWeek)
    {
        StateSetWeekShow();
    }    
    //Serial.printf("Time: %x:%x:%x:%x:%x:%x:%x\n",ClockTime.u8Year,ClockTime.u8Month,ClockTime.u8Day,ClockTime.u8Week,ClockTime.u8Hour,ClockTime.u8Min,ClockTime.u8Sec);
}
NS_DT_END