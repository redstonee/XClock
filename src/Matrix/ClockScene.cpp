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

#include "shitTime.h"

NS_DT_BEGIN

#define WeekColorIdxOffset (80)

static const char *TAG = "ClockScene";

bool ClockScene::init()
{
    TimeLayer *ClockLayer = TimeLayer::create();
    ClockLayer->setContentSize(Size(32, 8));
    ClockLayer->setPosition(0, 0);
    // ClockLayer->setOpacity
    this->addChild(ClockLayer);
    ClockLayer->initLayer();
    return true;
}

const unsigned char *pGetWeatherIcon(uint8_t weathercode, uint32_t *Length)
{
    const unsigned char *point = nullptr;
    /**https://seniverse.yuque.com/hyper_data/api_v3/yev2c3#M1KBK*/
    switch (weathercode)
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

void TimeLayer::BtnClickHandler(int8_t keyCode, Event *event)
{
    TimeStateMachine(keyCode, enKey_ShortPress);
}

void TimeLayer::BtnLongPressStartHandler(int8_t keyCode, Event *event)
{
    TimeStateMachine(keyCode, enKey_LongPressStart);
}

void TimeLayer::BtnDuringLongPressHandler(int8_t keyCode, Event *event)
{
    TimeStateMachine(keyCode, enKey_LongPress);
}

void TimeLayer::BtnDuringLongPressStopHandler(int8_t keyCode, Event *event)
{
    TimeStateMachine(keyCode, enKey_LongPressStop);
}

void TimeLayer::TimeStateMachine(int8_t key_type, int8_t key_event)
{
    auto min_temp = ClockTimeSetting.tm_min;
    auto hour_temp = ClockTimeSetting.tm_hour;
    switch (enTimests)
    {
    case State_TimeDis:
        if (key_type == enKey_OK)
        {
            if (key_event == enKey_LongPressStart)
            {
                enTimests = State_TimeSetMin;
                ClockTimeSetting = ClockTime;
            }
            else if (key_event == enKey_ShortPress)
            {
                PaletteIndex++;
                ColorIndex = 0;                                // reset the color index when change the palette
                if (false == boSetGlobalPaltIdx(PaletteIndex)) // over the maxium palette size return false,restart from 0;
                {
                    ESP_LOGE(TAG, "Store Palette fail");
                    PaletteIndex = 0;
                    boSetGlobalPaltIdx(PaletteIndex);
                }
                boSetGlobalColorIdx(ColorIndex);
                PaletteIndex = u8GetGlobalPaltIdx();
                currentPalette = pGetPalette(PaletteIndex);
                UpdateColor(key_type, key_event);
            }
        }
        else if (key_type == enKey_Left || key_type == enKey_Right)
        {
            if (key_event == enKey_LongPress || key_event == enKey_ShortPress || key_event == enKey_LongPressStop)
            {
                UpdateColor(key_type, key_event);
            }
        }
        break;
    case State_TimeSetMin:
        if (key_type == enKey_OK)
        {
            if (key_event == enKey_ShortPress)
            {
                enTimests = State_TimeSetHour;
            }
        }
        else if (key_type == enKey_Left)
        {

            if (min_temp-- <= 0)
            {
                min_temp = 59;
            }
        }
        else if (key_type == enKey_Right)
        {
            if (min_temp++ >= 59)
            {
                min_temp = 0;
            }
        }
        ClockTimeSetting.tm_min = min_temp;
        break;
    case State_TimeSetHour:
        if (key_type == enKey_OK)
        {
            if (key_event == enKey_ShortPress)
            {
                enTimests = State_TimeSetWeek;
            }
        }
        else if (key_type == enKey_Left)
        {
            if (hour_temp-- <= 0)
            {
                hour_temp = 23;
            }
        }
        else if (key_type == enKey_Right)
        {
            if (hour_temp++ >= 23)
            {
                hour_temp = 0;
            }
        }
        ClockTimeSetting.tm_hour = hour_temp; // 24h format
        break;
    case State_TimeSetWeek:
        if (key_type == enKey_OK)
        {
            if (key_event == enKey_ShortPress)
            {
                enTimests = State_TimeDis;
                setSystemTime(ClockTimeSetting); // set the system time
                // SendSettingTime(ClockTimeSetting);
            }
        }
        else if (key_type == enKey_Left)
        {
            if (ClockTimeSetting.tm_wday-- <= 0)
            {
                ClockTimeSetting.tm_wday = 6;
            }
        }
        else if (key_type == enKey_Right)
        {
            if (ClockTimeSetting.tm_wday++ >= 6)
            {
                ClockTimeSetting.tm_wday = 0;
            }
        }
        break;
    default:
        break;
    }
}

void TimeLayer::UpdateColor(int8_t key_type, int8_t key_event)
{

    if (key_type == enKey_Left)
    {
        ColorIndex++;
    }
    else if (key_type == enKey_Right)
    {
        ColorIndex--;
    }
    if (key_event == enKey_LongPressStop || key_event == enKey_ShortPress)
    {
        boSetGlobalColorIdx(ColorIndex); // store the color index when long press stop or short press
    }

    ESP_LOGI(TAG, "PaletteIndex %d", PaletteIndex);
    ESP_LOGI(TAG, "ColorIndex %d", ColorIndex);

    CRGB ColorFromPat = ColorFromPalette(currentPalette, ColorIndex);
    timecolor.r = ColorFromPat.r;
    timecolor.g = ColorFromPat.g;
    timecolor.b = ColorFromPat.b;
    // ColorFromPat = ColorFromPalette( currentPalette, ColorIndex+WeekColorIdxOffset);
    // weekcolor.r = ColorFromPat.r;
    // weekcolor.g = ColorFromPat.g;
    // weekcolor.b = ColorFromPat.b;
    if ((timecolor.r > 235) && (timecolor.g > 235) && (timecolor.b > 235))
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
    ESP_LOGI(TAG, "timecolor r: %d, g: %d, b: %d", timecolor.r, timecolor.g, timecolor.b);
    hourCanvas->setTextColor(timecolor);
    minuteCanvas->setTextColor(timecolor);

    updateHour(ClockTime.tm_hour);
    updateMinute(ClockTime.tm_min);
    minutePointCanvas->setTextColor(timecolor);
    minutePointCanvas->canvasReset();
    minutePointCanvas->print(":");
    DrawWeek(ClockTime.tm_wday);
}


void TimeLayer::updateHour(uint8_t hour)
{
    ESP_LOGD(TAG, "hour %d", hour);
    if (hourText == nullptr)
    {
        ESP_LOGW(TAG, "hourText is null");
        return;
    }

    char hourStr[5];
    sprintf(hourStr, "%02d", hour);

    hourCanvas->canvasReset();
    hourCanvas->print(hourStr);
    hourText->setVisible(true);
}

void TimeLayer::updateMinute(uint8_t minute)
{
    ESP_LOGD(TAG, "minute %d", minute);
    if (minuteText == nullptr)
    {
        ESP_LOGW(TAG, "minuteText is null");
        return;
    }
    char minuteStr[5];
    sprintf(minuteStr, "%02d", minute);

    minuteCanvas->canvasReset();
    minuteCanvas->print(minuteStr);
    minuteText->setVisible(true);
}

void TimeLayer::DrawWeek(uint8_t week)
{
    ESP_LOGD(TAG, "Week %d", week);
    Weekcanvas->canvasReset();

    DTRGB inactivecol = timecolor;
    inactivecol.fadeToBlackBy(200);
    if (Weekcanvas != nullptr)
    {
        for (uint8_t i = 0; i < 7; i++)
        {
            if (week != 0) /*0 means sunday*/
            {
                if (i != (week - 1))
                {
                    Weekcanvas->drawLine(i * 3, 0, i * 3 + 1, 0, inactivecol);
                }
                else
                {
                    Weekcanvas->drawLine(i * 3, 0, i * 3 + 1, 0, timecolor);
                }
            }
            else
            {
                if (i != 6)
                {
                    Weekcanvas->drawLine(i * 3, 0, i * 3 + 1, 0, inactivecol);
                }
                else
                {
                    Weekcanvas->drawLine(6 * 3, 0, 6 * 3 + 1, 0, timecolor);
                }
            }
        }
    }
}

bool TimeLayer::initLayer()
{
    auto currentTimeTime = time(nullptr);
    ClockTime = *localtime(&currentTimeTime);

    ColorIndex = u8GetGlobalColorIdx();
    PaletteIndex = u8GetGlobalPaltIdx();

    ESP_LOGI(TAG, "PaletteIndex %d", PaletteIndex);

    currentPalette = pGetPalette(PaletteIndex);
    CRGB ColorFromPat = ColorFromPalette(currentPalette, ColorIndex);
    auto listener = EventListenerButton::create();
    listener->onBtnDuringLongPress = DT_CALLBACK_2(TimeLayer::BtnDuringLongPressHandler, this);
    listener->onBtnLongPressStop = DT_CALLBACK_2(TimeLayer::BtnDuringLongPressStopHandler, this);
    listener->onBtnLongPressStart = DT_CALLBACK_2(TimeLayer::BtnLongPressStartHandler, this);
    listener->onBtnClick = DT_CALLBACK_2(TimeLayer::BtnClickHandler, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    char hourStr[5];
    char minuteSr[5];
    sprintf(hourStr, "%02d", ClockTime.tm_hour);
    sprintf(minuteSr, "%02d", ClockTime.tm_min);

    auto WeatherCode = GetCurWeatherCode();
    ESP_LOGD(TAG, "Weather code:%d", WeatherCode);

    uint32_t gifsize = 0;
    const unsigned char *picon = pGetWeatherIcon(WeatherCode, &gifsize);
    ESP_LOGD(TAG, "Weather icon:%x", picon);

    Weather = FrameSprite::create(picon, gifsize, BMP_GIF);
    Weather->setPosition(0, 0);
    Weather->setAutoSwitch(true);
    timecolor.r = ColorFromPat.r;
    timecolor.g = ColorFromPat.g;
    timecolor.b = ColorFromPat.b;
    // ColorFromPat = ColorFromPalette( currentPalette, ColorIndex+WeekColorIdxOffset);
    if ((timecolor.r > 235) && (timecolor.g > 235) && (timecolor.b > 235))
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

    hourText = TextSprite::create(Size(8, 5), Size(8, 5), timecolor, hourStr, TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    hourCanvas = hourText->getSpriteCanvas();
    minutePointText = TextSprite::create(Size(2, 5), Size(2, 5), timecolor, ":", TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    minutePointCanvas = minutePointText->getSpriteCanvas();
    minuteText = TextSprite::create(Size(8, 5), Size(8, 5), timecolor, minuteSr, TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    minuteCanvas = minuteText->getSpriteCanvas();
    Week = CanvasSprite::create(21, 1);
    Weekcanvas = Week->getSpriteCanvas();
    AlarmIcon = CanvasSprite::create(1, 8);
    AlarmIconCanvas = AlarmIcon->getSpriteCanvas();
    AlarmIcon->setPosition(31, 0);
    AlarmIconCanvas->drawLine(0, 0, 0, 7, DTRGB(0, 255, 0));
    AlarmIcon->setVisible(false);
    hourText->setTransparent(true);
    hourText->setPosition(11, 1);
    minutePointText->setTransparent(true);
    minutePointText->setPosition(19, 1);
    minutePointText->runAction(RepeatForever::create(Blink::create(1, 1)));
    minuteText->setTransparent(true);
    minuteText->setPosition(21, 1);
    Week->setPosition(10, 7);
    DrawWeek(ClockTime.tm_wday);
    this->addChild(Weather);
    this->addChild(hourText);
    this->addChild(minutePointText);
    this->addChild(minuteText);
    this->addChild(Week, 3);
    this->addChild(AlarmIcon);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(TimeLayer::TimeUpdate), 0.1);
    // MoveBy *moveBy1 = MoveBy::create(0.5,Vec2(0,5));
    // MoveBy *moveBy2 = MoveBy::create(0.5,Vec2(0,5));
    // SpriteCanvas *TestSprite = Min_2->getSpriteCanvas();
    // Min_2->runAction(moveBy1);
    // Min_3->runAction(moveBy2);
    return true;
}

void TimeLayer::CleanUpAnimationTmp(void)
{
    auto Hour1SprtTmp = getChildByTag(Hour1AniTmpTag);
    if (Hour1SprtTmp)
    {
        if (0 == Hour1SprtTmp->getNumberOfRunningActions())
        {
            removeChild(Hour1SprtTmp);
        }
    }
    auto Hour2SprtTmp = getChildByTag(Hour2AniTmpTag);
    if (Hour2SprtTmp)
    {
        if (0 == Hour2SprtTmp->getNumberOfRunningActions())
        {
            removeChild(Hour2SprtTmp);
        }
    }
    auto Min1SprtTmp = getChildByTag(Min1AniTmpTag);
    if (Min1SprtTmp)
    {
        if (0 == Min1SprtTmp->getNumberOfRunningActions())
        {
            removeChild(Min1SprtTmp);
        }
    }
    auto Min2SprtTmp = getChildByTag(Min2AniTmpTag);
    if (Min2SprtTmp)
    {
        if (0 == Min2SprtTmp->getNumberOfRunningActions())
        {
            removeChild(Min2SprtTmp);
        }
    }
}

void TimeLayer::digitSwitchAnimation(TextSprite *digitSprt, uint8_t oldDigit, uint8_t newDigit, int tmpTag)
{
    char oldDigitStr[5];
    char newDigitStr[5];
    sprintf(oldDigitStr, "%02d", oldDigit);
    sprintf(newDigitStr, "%02d", newDigit);

    TextSprite *SprtTmp = TextSprite::create(Size(10, 5), Size(10, 5), timecolor, oldDigitStr, TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    SprtTmp->setPosition(digitSprt->getPositionX(), digitSprt->getPositionY());
    this->addChild(SprtTmp, 0, tmpTag);

    digitSprt->setPosition(digitSprt->getPositionX(), -6);
    SpriteCanvas *digitCanvas = digitSprt->getSpriteCanvas();
    digitCanvas->canvasReset();
    digitCanvas->print(newDigitStr);

    MoveBy *SwitchMove1 = MoveBy::create(0.5, Vec2(0, 7));
    Vec2 TgtPos(digitSprt->getPositionX(), 1);
    MoveTo *SwitchMove2 = MoveTo::create(0.5, TgtPos);
    Sequence *Seq = Sequence::createWithTwoActions(SwitchMove1, CallFunc::create(DT_CALLBACK_0(TimeLayer::CleanUpAnimationTmp, this)));
    SprtTmp->runAction(Seq);
    digitSprt->runAction(SwitchMove2);
}

bool boActiveAlarm(void)
{
    auto nAlarms = getAlarmClockCount();
    if (nAlarms)
    {
        for (uint8_t i = 0; i < nAlarms; i++)
        {
            if (getAlarmClock(i).alarmStatus == Alarm_GoOff)
                return true;
        }
    }
    return false;
}

void TimeLayer::StateTimeDisShow(void)
{
    static uint8_t WeatherCodeOld = 0;
    uint8_t WeatherCode = 0;
    WeatherCode = (uint8_t)GetCurWeatherCode();

    auto currentTimeTime = time(nullptr);
    auto currentTime = *localtime(&currentTimeTime);
    char hourStr[5];
    char minuteStr[5];
    sprintf(hourStr, "%02d", currentTime.tm_hour);
    sprintf(minuteStr, "%02d", currentTime.tm_min);

    bool boReEnterflag = false;
    static bool boPreAlarmActive = false;
    bool boAlarmActive = boActiveAlarm();
    uint32_t gifsize = 0;
    const unsigned char *picon = pGetWeatherIcon(WeatherCode, &gifsize);
    if (WeatherCodeOld != WeatherCode)
    {
        Weather->setSpriteFrame(SpriteFrame::create(picon, gifsize, BMP_GIF));
    }
    if (0 != Week->getNumberOfRunningActions())
    {
        Week->stopAllActions();
        boReEnterflag = true;
    }
    if (0 == minutePointText->getNumberOfRunningActions())
    {
        minutePointText->runAction(RepeatForever::create(Blink::create(1, 1)));
    }
    if (boReEnterflag == true)
    {
        updateHour(currentTime.tm_hour);
        updateMinute(currentTime.tm_min);
        DrawWeek(currentTime.tm_wday);
        hourText->setVisible(true);
        minuteText->setVisible(true);
        Week->setVisible(true);
        minutePointText->setVisible(true);
    }
    if (boPreAlarmActive != boAlarmActive)
    {
        if (boAlarmActive)
        {
            AlarmIcon->runAction(RepeatForever::create(Blink::create(1, 1)));
            AlarmIcon->setVisible(true);
            ESP_LOGI(TAG, "Alarming");
        }
        else
        {
            AlarmIcon->stopAllActions();
            AlarmIcon->setVisible(false);
            ESP_LOGI(TAG, "Alarming disable");
        }
    }

    if (currentTime.tm_hour != ClockTime.tm_hour)
    {
        digitSwitchAnimation(hourText, ClockTime.tm_hour, currentTime.tm_hour, Hour1AniTmpTag);
    }
    if (currentTime.tm_min != ClockTime.tm_min)
    {
        digitSwitchAnimation(minuteText, ClockTime.tm_min, currentTime.tm_min, Min1AniTmpTag);
    }
    if (currentTime.tm_wday != ClockTime.tm_wday)
    {
        DrawWeek(currentTime.tm_wday);
    }
    ClockTime = currentTime;
    boPreAlarmActive = boAlarmActive;
    WeatherCodeOld = WeatherCode;
}

void TimeLayer::StateSetMinShow(void)
{
    bool isFirstEnter = false;
    static tm oldSettingTime = {
        0,
    };
    if (0 != minutePointText->getNumberOfRunningActions())
    {
        minutePointText->stopAllActions();
        isFirstEnter = true;
    }
    if (0 == minuteText->getNumberOfRunningActions())
    {
        minuteText->runAction(RepeatForever::create(Blink::create(1, 2)));
    }
    if (isFirstEnter)
    {
        updateHour(ClockTimeSetting.tm_hour);
        updateMinute(ClockTimeSetting.tm_min);
        DrawWeek(ClockTimeSetting.tm_wday);
        Week->setVisible(true);
        minutePointText->setVisible(true);
    }
    if (ClockTimeSetting.tm_min != oldSettingTime.tm_min)
    {
        updateMinute(ClockTimeSetting.tm_min);
        DrawWeek(ClockTimeSetting.tm_wday);
    }
    oldSettingTime = ClockTimeSetting;
}

void TimeLayer::StateSetHourShow(void)
{
    bool isFirstEnter = false;
    static tm oldSettingTime = {
        0,
    };
    if (0 != minuteText->getNumberOfRunningActions())
    {
        minuteText->stopAllActions();
        isFirstEnter = true;
    }
    if (0 == hourText->getNumberOfRunningActions())
    {
        hourText->runAction(RepeatForever::create(Blink::create(1, 2)));
    }
    if (isFirstEnter)
    {
        updateHour(ClockTimeSetting.tm_hour);
        updateMinute(ClockTimeSetting.tm_min);
        DrawWeek(ClockTimeSetting.tm_wday);
        Week->setVisible(true);
        minutePointText->setVisible(true);
    }
    if (ClockTimeSetting.tm_hour != oldSettingTime.tm_hour)
    {
        updateHour(ClockTimeSetting.tm_hour);
    }
    oldSettingTime = ClockTimeSetting;
}

void TimeLayer::StateSetWeekShow(void)
{
    bool isFirstEnter = false;
    static tm oldSettingTime = {
        0,
    };
    if (hourText->getNumberOfRunningActions())
    {
        hourText->stopAllActions();
        isFirstEnter = true;
    }
    if (!Week->getNumberOfRunningActions())
    {
        Week->runAction(RepeatForever::create(Blink::create(1, 2)));
    }
    if (isFirstEnter)
    {
        updateHour(ClockTimeSetting.tm_hour);
        updateMinute(ClockTimeSetting.tm_min);
        DrawWeek(ClockTimeSetting.tm_wday);

        Week->setVisible(true);
        minutePointText->setVisible(true);
    }
    if (ClockTimeSetting.tm_wday != oldSettingTime.tm_wday)
    {
        DrawWeek(ClockTimeSetting.tm_wday);
    }
    oldSettingTime = ClockTimeSetting;
}

void TimeLayer::TimeUpdate(float dt)
{
    if (enTimests == State_TimeDis)
    {
        StateTimeDisShow();
    }
    else if (enTimests == State_TimeSetMin)
    {
        StateSetMinShow();
    }
    else if (enTimests == State_TimeSetHour)
    {
        StateSetHourShow();
    }
    else if (enTimests == State_TimeSetWeek)
    {
        StateSetWeekShow();
    }
    // Serial.printf("Time: %x:%x:%x:%x:%x:%x:%x\n",ClockTime.year,ClockTime.month,ClockTime.day,ClockTime.week,ClockTime.hour,ClockTime.minute,ClockTime.second);
}
NS_DT_END