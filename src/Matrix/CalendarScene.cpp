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

    timeSettingQueue = pGetTimeSettingQ();
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

    yearText = TextSprite::create(Size(16, 5), Size(16, 5), Color, String(ClockTime.tm_year + 1900).c_str(), TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    yearCanvas = yearText->getSpriteCanvas();
    yearText->setTransparent(true);
    yearText->setPosition(12, 1);

    this->addChild(CalendarIcon);
    this->addChild(yearText);
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
    if (timeSettingQueue != nullptr)
    {
        if (xQueueSend(timeSettingQueue,
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
        if (!yearText->getNumberOfRunningActions())
        {
            yearText->runAction(RepeatForever::create(Blink::create(1, 2)));
        }
        if (ClockTimeSetting.tm_year != OldSettingTime.tm_year)
        {
            yearCanvas->canvasReset();
            yearCanvas->print(String(ClockTimeSetting.tm_year + 1900).c_str());
        }
    }
    else
    {
        if (yearText->getNumberOfRunningActions())
        {
            ClockTime = currentTime;
            yearText->stopAllActions();
            yearCanvas->canvasReset();
            yearCanvas->print(String(currentTime.tm_year + 1900).c_str());
            yearText->setVisible(true);
        }
    }
}

/********************************Date sence and layer****************************************/

bool CalMonthScene::init()
{
    MonthLayer *Monthlayer = MonthLayer::create();
    Monthlayer->setContentSize(Size(32, 8));
    Monthlayer->setPosition(0, 0);
    this->addChild(Monthlayer);
    Monthlayer->initLayer();
    return true;
}

bool MonthLayer::initLayer()
{
    auto currentTimeTime = time(nullptr);
    ClockTime = *localtime(&currentTimeTime);
    char monthStr[5];
    char dayStr[5];
    sprintf(monthStr, "%02d", ClockTime.tm_mon + 1); // Month is 0-11 in tm struct
    sprintf(dayStr, "%02d", ClockTime.tm_mday);

    timeSettingQueue = pGetTimeSettingQ();
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

    monthText = TextSprite::create(Size(8, 5), Size(8, 5), Color, monthStr, TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    monthCanvas = monthText->getSpriteCanvas();

    Split = TextSprite::create(Size(2, 5), Size(2, 5), Color, ".", TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    Splitcanvas = Split->getSpriteCanvas();

    dayText = TextSprite::create(Size(8, 5), Size(8, 5), Color, dayStr, TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    dayCanvas = dayText->getSpriteCanvas();

    monthText->setTransparent(true);
    monthText->setPosition(12, 1);
    Split->setTransparent(true);
    Split->setPosition(20, 1);
    dayText->setTransparent(true);
    dayText->setPosition(22, 1);

    this->addChild(CalendarIcon);
    this->addChild(monthText);
    this->addChild(Split);
    this->addChild(dayText);
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
        if (0 != monthText->getNumberOfRunningActions())
        {
            monthText->stopAllActions();
            monthText->setVisible(true);
        }
        if (0 != dayText->getNumberOfRunningActions())
        {
            dayText->stopAllActions();
            dayText->setVisible(true);
        }
        if (currentTime.tm_mday != ClockTime.tm_mday)
        {
            char dayStr[5];
            sprintf(dayStr, "%02d", ClockTime.tm_mday);
            dayCanvas->canvasReset();
            dayCanvas->print(dayStr);
        }
        if (currentTime.tm_mon != ClockTime.tm_mon)
        {
            char monthStr[5];
            sprintf(monthStr, "%02d", ClockTime.tm_mon + 1); // Month is 0-11 in tm struct
            monthCanvas->canvasReset();
            monthCanvas->print(monthStr);
        }
        ClockTime = currentTime;
    }
    else if (State_DaySet == enMonthState)
    {
        if (0 != monthText->getNumberOfRunningActions())
        {
            monthText->stopAllActions();
            monthText->setVisible(true);
        }
        if (0 == dayText->getNumberOfRunningActions())
        {
            dayText->runAction(RepeatForever::create(Blink::create(1, 2)));
        }
        dayCanvas->canvasReset();
        dayCanvas->print(String(ClockTimeSetting.tm_mday).c_str());
    }
    else if (State_MonthSet == enMonthState)
    {
        if (0 != dayText->getNumberOfRunningActions())
        {
            char dayStr[5];
            sprintf(dayStr, "%02d", ClockTime.tm_mday);

            dayText->stopAllActions();
            dayCanvas->canvasReset();
            dayCanvas->print(dayStr);
            dayText->setVisible(true);
        }
        if (0 == monthText->getNumberOfRunningActions())
        {
            monthText->runAction(RepeatForever::create(Blink::create(1, 2)));
        }
        char monthStr[5];
        sprintf(monthStr, "%02d", ClockTime.tm_mon + 1); // Month is 0-11 in tm struct
        monthCanvas->canvasReset();
        monthCanvas->print(monthStr);
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
    if (timeSettingQueue != nullptr)
    {
        if (xQueueSend(timeSettingQueue,
                       reinterpret_cast<const void *>(&settingtime),
                       (TickType_t)10) != pdPASS)
        {
            /* Failed to post the message, even after 10 ticks. */
        }
    }
}

NS_DT_END