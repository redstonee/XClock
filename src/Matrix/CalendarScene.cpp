#include "CalendarScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "main.h"
#include "FastLED.h"
#include "Icon.h"
#include "ClockKey.h"
#include "Palette.h"

NS_DT_BEGIN

bool CalYearScene::init()
{
    YearLayer *YearLayer = YearLayer::create();
    YearLayer->setContentSize(Size(32, 8));
    YearLayer->setPosition(0, 0);
    // ClockLayer->setOpacity
    this->addChild(YearLayer);
    YearLayer->initLayer();
    return true;
}

bool YearLayer::initLayer()
{

    ColorIndex = u8GetGlobalColorIdx();
    PaletteIndex = u8GetGlobalPaltIdx();

    auto currentTimeTime = time(nullptr);
    ClockTime = *localtime(&currentTimeTime);

    TimeSettingQ = pGetTimeSettingQ();
    CRGBPalette16 currentPalette = pGetPalette(PaletteIndex);
    CRGB ColorFromPat = ColorFromPalette(currentPalette, ColorIndex);
    DTRGB Color;
    Color.r = ColorFromPat.r;
    Color.g = ColorFromPat.g;
    Color.b = ColorFromPat.b;
    auto listener = EventListenerButton::create();
    listener->onBtnDuringLongPress = DT_CALLBACK_2(YearLayer::BtnDuringLongPressHandler, this);
    listener->onBtnLongPressStart = DT_CALLBACK_2(YearLayer::BtnLongPressStartHandler, this);
    listener->onBtnClick = DT_CALLBACK_2(YearLayer::BtnClickHandler, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    FrameSprite *CalendarIcon = FrameSprite::create(icon_calendar, sizeof(icon_calendar), BMP_GIF);
    CalendarIcon->setPosition(0, 0);
    CalendarIcon->setAutoSwitch(true);
    // YearH = TextSprite::create(Size(8, 5), Size(8, 5), Color, "20", TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    // YearHcanvas = YearH->getSpriteCanvas();
    YearL = TextSprite::create(Size(8, 5), Size(8, 5), Color, String(ClockTime.tm_year).c_str(), TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    YearLcanvas = YearL->getSpriteCanvas();
    // YearH->setTransparent(true);
    // YearH->setPosition(12, 1);
    YearL->setTransparent(true);
    YearL->setPosition(20, 1);
    this->addChild(CalendarIcon);
    // this->addChild(YearH);
    this->addChild(YearL);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(YearLayer::YearUpdate), 0.5);
    return true;
}

void YearLayer::BtnClickHandler(int8_t keyCode, Event *event)
{
    if (true == boYearSetting)
    {
        uint8_t year_temp = ClockTimeSetting.tm_year;
        if (enKey_OK == keyCode)
        {
            boYearSetting = false;
            SendSettingYear(&ClockTimeSetting);
        }
        else if (enKey_Left == keyCode)
        {
            if (--year_temp > 99)
            {
                year_temp = 99;
            }
        }
        else if (enKey_Right == keyCode)
        {
            if (++year_temp > 99)
            {
                year_temp = 0;
            }
        }
        ClockTimeSetting.tm_year = year_temp;
    }
    else
    {
    }
}

void YearLayer::BtnLongPressStartHandler(int8_t keyCode, Event *event)
{
    if (enKey_OK == keyCode)
    {
        boYearSetting = true;
        ClockTimeSetting = ClockTime;
    }
}

void YearLayer::BtnDuringLongPressHandler(int8_t keyCode, Event *event)
{
    if (true == boYearSetting)
    {
        uint8_t year_temp = ClockTimeSetting.tm_year;
        if (enKey_Left == keyCode)
        {
            if (--year_temp > 99)
            {
                year_temp = 99;
            }
        }
        else if (enKey_Right == keyCode)
        {
            if (++year_temp > 99)
            {
                year_temp = 0;
            }
        }
        ClockTimeSetting.tm_year = year_temp;
    }
    else
    {
    }
}

void YearLayer::SendSettingYear(tm *settingtime)
{
    if (TimeSettingQ != nullptr)
    {
        if (xQueueSend(TimeSettingQ,
                       (void *)settingtime,
                       (TickType_t)10) != pdPASS)
        {
            /* Failed to post the message, even after 10 ticks. */
        }
    }
}

void YearLayer::YearUpdate(float dt)
{
    auto currentTimeTime = time(nullptr);
    auto currentTime = *localtime(&currentTimeTime);

    static tm OldSettingTime = {
        0,
    };
    if (true == boYearSetting)
    {
        if (!YearL->getNumberOfRunningActions())
        {
            YearL->runAction(RepeatForever::create(Blink::create(1, 2)));
        }
        if (ClockTimeSetting.tm_year != OldSettingTime.tm_year)
        {
            YearLcanvas->canvasReset();
            YearLcanvas->print(String(ClockTimeSetting.tm_year).c_str());
        }
    }
    else
    {
        if (YearL->getNumberOfRunningActions())
        {
            ClockTime = currentTime;
            YearL->stopAllActions();
            YearLcanvas->canvasReset();
            YearLcanvas->print(String(currentTime.tm_year).c_str());
            YearL->setVisible(true);
        }
    }
}

/********************************Date sence and layer****************************************/

bool CalMonthScene::init()
{
    MonthLayer *Monthlayer = MonthLayer::create();
    Monthlayer->setContentSize(Size(32, 8));
    Monthlayer->setPosition(0, 0);
    // ClockLayer->setOpacity
    this->addChild(Monthlayer);
    Monthlayer->initLayer();
    return true;
}

bool MonthLayer::initLayer()
{
    auto currentTimeTime = time(nullptr);
    ClockTime = *localtime(&currentTimeTime);

    TimeSettingQ = pGetTimeSettingQ();
    ColorIndex = u8GetGlobalColorIdx();
    PaletteIndex = u8GetGlobalPaltIdx();
    CRGBPalette16 currentPalette = pGetPalette(PaletteIndex);
    CRGB ColorFromPat = ColorFromPalette(currentPalette, ColorIndex);
    DTRGB Color;
    Color.r = ColorFromPat.r;
    Color.g = ColorFromPat.g;
    Color.b = ColorFromPat.b;
    auto listener = EventListenerButton::create();
    listener->onBtnDuringLongPress = DT_CALLBACK_2(MonthLayer::BtnDuringLongPressHandler, this);
    listener->onBtnLongPressStart = DT_CALLBACK_2(MonthLayer::BtnLongPressStartHandler, this);
    listener->onBtnClick = DT_CALLBACK_2(MonthLayer::BtnClickHandler, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    FrameSprite *CalendarIcon = FrameSprite::create(icon_calendar, sizeof(icon_calendar), BMP_GIF);
    CalendarIcon->setPosition(0, 0);
    CalendarIcon->setAutoSwitch(true);
    Month = TextSprite::create(Size(8, 5), Size(8, 5), Color, String(ClockTime.tm_mon).c_str(), TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    Monthcanvas = Month->getSpriteCanvas();
    Split = TextSprite::create(Size(2, 5), Size(2, 5), Color, ".", TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    Splitcanvas = Split->getSpriteCanvas();
    Day = TextSprite::create(Size(8, 5), Size(8, 5), Color, String(ClockTime.tm_mday).c_str(), TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    Daycanvas = Day->getSpriteCanvas();
    Month->setTransparent(true);
    Month->setPosition(12, 1);
    Split->setTransparent(true);
    Split->setPosition(20, 1);
    Day->setTransparent(true);
    Day->setPosition(22, 1);
    this->addChild(CalendarIcon);
    this->addChild(Month);
    this->addChild(Split);
    this->addChild(Day);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(MonthLayer::MonthUpdate), 0.5);
    return true;
}

void MonthLayer::MonthUpdate(float dt)
{
    auto currentTimeTime = time(nullptr);
    auto currentTime = *localtime(&currentTimeTime);

    static tm OldSettingTime = {
        0,
    };
    static tenMonthState OldState = State_MonthDis;
    if (State_MonthDis == enMonthState)
    {
        if (0 != Month->getNumberOfRunningActions())
        {
            Month->stopAllActions();
            Month->setVisible(true);
        }
        if (0 != Day->getNumberOfRunningActions())
        {
            Day->stopAllActions();
            Day->setVisible(true);
        }
        if (currentTime.tm_mday != ClockTime.tm_mday)
        {
            Daycanvas->canvasReset();
            Daycanvas->print(String(currentTime.tm_mday).c_str());
        }
        if (currentTime.tm_mon != ClockTime.tm_mon)
        {
            Monthcanvas->canvasReset();
            Monthcanvas->print(String(currentTime.tm_mon).c_str());
        }
        ClockTime = currentTime;
    }
    else if (State_DaySet == enMonthState)
    {
        if (0 != Month->getNumberOfRunningActions())
        {
            Month->stopAllActions();
            Month->setVisible(true);
        }
        if (0 == Day->getNumberOfRunningActions())
        {
            Day->runAction(RepeatForever::create(Blink::create(1, 2)));
        }
        Daycanvas->canvasReset();
        Daycanvas->print(String(ClockTimeSetting.tm_mday).c_str());
    }
    else if (State_MonthSet == enMonthState)
    {
        if (0 != Day->getNumberOfRunningActions())
        {
            Day->stopAllActions();
            Daycanvas->canvasReset();
            Daycanvas->print(String(ClockTimeSetting.tm_mday).c_str());
            Day->setVisible(true);
        }
        if (0 == Month->getNumberOfRunningActions())
        {
            Month->runAction(RepeatForever::create(Blink::create(1, 2)));
        }
        Monthcanvas->canvasReset();
        Monthcanvas->print(String(ClockTimeSetting.tm_mon).c_str());
    }
}

void MonthLayer::BtnClickHandler(int8_t keyCode, Event *event)
{
    MonthStateMachine(keyCode, enKey_ShortPress);
}

void MonthLayer::BtnLongPressStartHandler(int8_t keyCode, Event *event)
{
    MonthStateMachine(keyCode, enKey_LongPressStart);
}

void MonthLayer::BtnDuringLongPressHandler(int8_t keyCode, Event *event)
{
    MonthStateMachine(keyCode, enKey_LongPress);
}

void MonthLayer::MonthStateMachine(int8_t key_type, int8_t key_event)
{
    uint8_t month_temp = ClockTimeSetting.tm_mon;
    uint8_t day_temp = ClockTimeSetting.tm_mday;
    switch (enMonthState)
    {
    case State_MonthDis:
        if (key_type == enKey_OK)
        {
            if (key_event == enKey_LongPressStart)
            {
                enMonthState = State_DaySet;
                ClockTimeSetting = ClockTime;
            }
        }
        break;
    case State_DaySet:
        if (key_type == enKey_OK)
        {
            if (key_event == enKey_ShortPress)
            {
                enMonthState = State_MonthSet;
            }
        }
        else if (key_type == enKey_Left)
        {

            if (--day_temp > 31)
            {
                day_temp = 31;
            }
        }
        else if (key_type == enKey_Right)
        {
            if (++day_temp > 31)
            {
                day_temp = 0;
            }
        }
        ClockTimeSetting.tm_mday = day_temp;
        break;
    case State_MonthSet:
        if (key_type == enKey_OK)
        {
            if (key_event == enKey_ShortPress)
            {
                enMonthState = State_MonthDis;
                SendSettingMonth(ClockTimeSetting);
            }
        }
        else if (key_type == enKey_Left)
        {

            if (--month_temp > 12)
            {
                month_temp = 12;
            }
        }
        else if (key_type == enKey_Right)
        {
            if (++month_temp > 12)
            {
                month_temp = 0;
            }
        }
        ClockTimeSetting.tm_mon = month_temp;
        break;
    default:
        break;
    }
}

void MonthLayer::SendSettingMonth(const tm &settingtime)
{
    if (TimeSettingQ != nullptr)
    {
        if (xQueueSend(TimeSettingQ,
                       reinterpret_cast<const void *>(&settingtime),
                       (TickType_t)10) != pdPASS)
        {
            /* Failed to post the message, even after 10 ticks. */
        }
    }
}

NS_DT_END