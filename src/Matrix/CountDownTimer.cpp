#include "CountDownTimer.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "FastLED.h"
#include "ClockKey.h"
#include "Sound.h"

NS_DT_BEGIN

bool CountDownLayer::isActive = false;

bool CountDownLayer::isCountDownTimerActive()
{
    return isActive;
}
/********************************Timer sence and layer****************************************/

void CountDownLayer::CounterTimerCb(TimerHandle_t xTimer)
{
    auto timerLayer = reinterpret_cast<CountDownLayer *>(pvTimerGetTimerID(xTimer));
    if (timerLayer->countTimerConfig.second-- <= 00)
    {
        if (timerLayer->countTimerConfig.minute-- <= 0)
        {
            timerLayer->countTimerConfig.minute = 0;
            timerLayer->countTimerConfig.second = 0;
            timerLayer->countTimerConfig.status = TimerStatus::FINISHED;
            xTimerStop(timerLayer->counterTimer, 10);
            xTimerStart(timerLayer->inactiveTimer, 10);
            if (enSndID_None == enGetCurSndID())
            {
                boReqSound(enSndID_Alarm2, 1);
            }
        }
        else
        {
            timerLayer->countTimerConfig.second = 59;
        }
    }
}

void CountDownLayer::ExitTimerCb(TimerHandle_t xTimer)
{
    isActive = false;
}

bool CountDownScene::init()
{
    CountDownLayer *Timerlayer = CountDownLayer::create();
    Timerlayer->setContentSize(Size(32, 8));
    Timerlayer->setPosition(0, 0);
    // ClockLayer->setOpacity
    this->addChild(Timerlayer);
    Timerlayer->initLayer();
    return true;
}

bool CountDownLayer::initLayer()
{
    isActive = true;
    auto listener = EventListenerButton::create();
    listener->onBtnDuringLongPress = DT_CALLBACK_2(CountDownLayer::BtnDuringLongPressHandler, this);
    listener->onBtnLongPressStart = DT_CALLBACK_2(CountDownLayer::BtnLongPressStartHandler, this);
    listener->onBtnClick = DT_CALLBACK_2(CountDownLayer::BtnClickHandler, this);
    listener->onBtnDoubleClick = DT_CALLBACK_2(CountDownLayer::BtnDoubleClickHandler, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    DTRGB TextColor = {255, 255, 255};
    enTimerSts = Timer_Dis;
    if (countTimerConfig.status == TimerStatus::COUNTING)
    {
        TimerIcon = FrameSprite::create(icon_TimerActive, sizeof(icon_TimerActive), BMP_GIF);
    }
    else
    {
        TimerIcon = FrameSprite::create(icon_TimerInactive, sizeof(icon_TimerInactive), BMP_BMP);
        countTimerConfig.minute = 25;
        countTimerConfig.second = 0;
    }

    TimerIcon->setPosition(-1, -1);
    TimerIcon->setAutoSwitch(true);
    TimePt = TextSprite::create(Size(2, 5), Size(2, 5), TextColor, ":", TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    TimePtcanvas = TimePt->getSpriteCanvas();
    Min = TextSprite::create(Size(8, 5), Size(8, 5), TextColor, (std::to_string(countTimerConfig.minute / 10) + std::to_string(countTimerConfig.minute % 10)), TextSprite::TextAlign::TextAlignRight, &TomThumb);
    Mincanvas = Min->getSpriteCanvas();
    Sec = TextSprite::create(Size(8, 5), Size(8, 5), TextColor, (std::to_string(countTimerConfig.second / 10) + std::to_string(countTimerConfig.second % 10)), TextSprite::TextAlign::TextAlignRight, &TomThumb);
    Seccanvas = Sec->getSpriteCanvas();
    Min->setPosition(10, 1);
    TimePt->setPosition(18, 1);
    Sec->setPosition(20, 1);
    this->addChild(TimePt);
    this->addChild(Min);
    this->addChild(Sec);
    this->addChild(TimerIcon);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(CountDownLayer::TimerUpdate), 0.1);
    if (!counterTimer)
    {
        counterTimer = xTimerCreate("FeatureTimer", pdMS_TO_TICKS(1000), true, this, CounterTimerCb);
    }
    if (!inactiveTimer)
    {
        inactiveTimer = xTimerCreate("FeatureTimer", pdMS_TO_TICKS(CountDownTimerExit_TO),
                                     true, this, ExitTimerCb);
    }

    return true;
}

void CountDownLayer::TimerUpdate(float dt)
{
    static CountTimer TimerPre = countTimerConfig;
    if (countTimerConfig.status == TimerStatus::COUNTING)
    {
        if (0 == TimePt->getNumberOfRunningActions())
        {
            TimePt->runAction(RepeatForever::create(Blink::create(1, 1)));
            TimerIcon->setSpriteFrame(SpriteFrame::create(icon_TimerActive, sizeof(icon_TimerActive), BMP_GIF));
        }
    }
    else
    {
        if (0 != TimePt->getNumberOfRunningActions())
        {
            TimePt->stopAllActions();
            TimePt->setVisible(true);
            TimerIcon->setSpriteFrame(SpriteFrame::create(icon_TimerInactive, sizeof(icon_TimerInactive), BMP_BMP));
        }
    }
    if (TimerPre.minute != countTimerConfig.minute)
    {
        char minuteStr[5];
        sprintf(minuteStr, "%02d", countTimerConfig.minute);
        Mincanvas->canvasReset();
        Mincanvas->print(minuteStr);
    }
    if (TimerPre.second != countTimerConfig.second)
    {
        char secondStr[5];
        sprintf(secondStr, "%02d", countTimerConfig.second);
        Seccanvas->canvasReset();
        Seccanvas->print(secondStr);
    }
    if (enTimerSts == Timer_Dis)
    {
        if (0 != Min->getNumberOfRunningActions())
        {
            Min->stopAllActions();
            Min->setVisible(true);
        }
        if (0 != Sec->getNumberOfRunningActions())
        {
            Sec->stopAllActions();
            Sec->setVisible(true);
        }
    }
    else if (enTimerSts == Timer_MinSetting)
    {
        if (0 == Min->getNumberOfRunningActions())
        {
            Min->runAction(RepeatForever::create(Blink::create(1, 1)));
        }
        if (0 != Sec->getNumberOfRunningActions())
        {
            Sec->stopAllActions();
            Sec->setVisible(true);
        }
    }
    else
    {
        if (0 != Min->getNumberOfRunningActions())
        {
            Min->stopAllActions();
            Min->setVisible(true);
        }
        if (0 == Sec->getNumberOfRunningActions())
        {
            Sec->runAction(RepeatForever::create(Blink::create(1, 1)));
        }
    }
    TimerPre = countTimerConfig;
}

void CountDownLayer::TimerMachine(int8_t keyCode, int8_t event)
{
    if (enKey_OK == keyCode)
    {
        if (enKey_ShortPress == event)
        {
            if (enTimerSts == Timer_Dis)
            {
                if (xTimerIsTimerActive(counterTimer))
                {
                    xTimerStop(counterTimer, 10);
                    xTimerStart(inactiveTimer, 10);
                    countTimerConfig.status = TimerStatus::READY;
                }
                else
                {
                    if (countTimerConfig.status == TimerStatus::READY)
                    {
                        initialMinute = countTimerConfig.minute;
                        initialSecond = countTimerConfig.second;
                        xTimerStart(counterTimer, 10);
                        xTimerStop(inactiveTimer, 10);
                        countTimerConfig.status = TimerStatus::COUNTING;
                    }
                    else
                    {
                        countTimerConfig.minute = initialMinute;
                        countTimerConfig.second = initialSecond;
                        countTimerConfig.status = TimerStatus::READY;
                    }
                }
            }
            else if (enTimerSts == Timer_MinSetting)
            {
                enTimerSts = Timer_Dis;
            }
            else
            {
                enTimerSts = Timer_MinSetting;
            }
        }
        else if (enKey_LongPressStart == event)
        {
            if (enTimerSts == Timer_Dis)
            {
                enTimerSts = Timer_SecSetting;
                if (xTimerIsTimerActive(counterTimer))
                {
                    xTimerStop(counterTimer, 10);
                    xTimerStart(inactiveTimer, 10);
                }
                countTimerConfig.status = TimerStatus::READY;
            }
        }
    }
    else if (enKey_Right == keyCode)
    {
        if (enTimerSts == Timer_MinSetting)
        {
            if (++countTimerConfig.minute > 99)
            {
                countTimerConfig.minute = 0;
            }
        }
        else if (enTimerSts == Timer_SecSetting)
        {
            if (++countTimerConfig.second >= 60)
            {
                countTimerConfig.second = 0;
            }
        }
    }
    else if (enKey_Left == keyCode)
    {
        if (enTimerSts == Timer_MinSetting)
        {
            if (countTimerConfig.minute-- <= 0)
            {
                countTimerConfig.minute = 99;
            }
        }
        else if (enTimerSts == Timer_SecSetting)
        {
            if (countTimerConfig.second-- <= 0)
            {
                countTimerConfig.second = 59;
            }
        }
    }
}

void CountDownLayer::BtnClickHandler(int8_t keyCode, Event *event)
{
    TimerMachine(keyCode, enKey_ShortPress);
}

void CountDownLayer::BtnDoubleClickHandler(int8_t keyCode, Event *event)
{
    TimerMachine(keyCode, enKey_DoubleClick);
}

void CountDownLayer::BtnLongPressStartHandler(int8_t keyCode, Event *event)
{
    TimerMachine(keyCode, enKey_LongPressStart);
}

void CountDownLayer::BtnDuringLongPressHandler(int8_t keyCode, Event *event)
{
    TimerMachine(keyCode, enKey_LongPress);
}

NS_DT_END