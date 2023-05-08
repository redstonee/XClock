#include "AlarmClkScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "FastLED.h"
#include "../Key/ClockKey.h"

NS_DT_BEGIN


/********************************Date sence and layer****************************************/

bool AlarmClkScene::init()
{
    AlarmClkLayer *Alarmlayer = AlarmClkLayer::create();
    Alarmlayer->setContentSize(Size(32,8));
    Alarmlayer->setPosition(0,0);
    //ClockLayer->setOpacity
    this->addChild(Alarmlayer);
    Alarmlayer->initLayer();
    return true;
}

bool AlarmClkLayer::initLayer()
{
    CRGBPalette16 currentPalette( RainbowColors_p );
    u8TotalAlarmNum = u8GetAlarmClkNum();
    if(u8TotalAlarmNum)
    {
        u8CurrentAlarm = 0;
        AlarmTime = stGetAlarmClk(u8CurrentAlarm);
    }
    else
    {
        AlarmTime.u8Hour = 0;
        AlarmTime.u8Min = 0;
        AlarmTime.u8Week = 0;
        AlarmTime.boActive = false;
        boAddAlarmClk(&AlarmTime);
    }
    auto listener = EventListenerButton::create();
    listener ->onBtnDuringLongPress = DT_CALLBACK_2(AlarmClkLayer::BtnDuringLongPressHandler,this);
    listener ->onBtnLongPressStart = DT_CALLBACK_2(AlarmClkLayer::BtnLongPressStartHandler,this);
    listener ->onBtnClick = DT_CALLBACK_2(AlarmClkLayer::BtnClickHandler,this);   
    listener ->onBtnDoubleClick = DT_CALLBACK_2(AlarmClkLayer::BtnDoubleClickHandler,this);  
    _eventDispatcher->addEventListenerWithSceneGraphPriority ( listener, this );
    if(AlarmTime.boActive)
    {
        ClkIcon = FrameSprite::create(icon_ClockActive,sizeof(icon_ClockActive),BMP_GIF);
    }
    else
    {
        ClkIcon = FrameSprite::create(icon_ClockInactive,sizeof(icon_ClockInactive),BMP_BMP);
    }
    
    ClkIcon->setPosition(0,0);
    ClkIcon->setAutoSwitch(true);
    DTRGB clkcolor(255,255,255);
    if(AlarmTime.boActive)
    {
        clkcolor.r = 0;
        clkcolor.g = 255;
        clkcolor.b = 0;
    }
    else
    {
        clkcolor.r = 255;
        clkcolor.g = 255;
        clkcolor.b = 255;
    }    
    std::string hour = std::to_string(AlarmTime.u8Hour/10) + std::to_string(AlarmTime.u8Hour%10);
    Hour = TextSprite::create(Size(8,5),Size(8,5),clkcolor,hour,TextSprite::TextAlign::TextAlignRight,&TomThumb);
    Hourcanvas = Hour->getSpriteCanvas();
    TimePt = TextSprite::create(Size(2,5),Size(2,5),clkcolor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    TimePtcanvas = TimePt->getSpriteCanvas();
    std::string min = std::to_string(AlarmTime.u8Min/10) + std::to_string(AlarmTime.u8Min%10);
    Min = TextSprite::create(Size(8,5),Size(8,5),clkcolor,min,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    Mincanvas = Min->getSpriteCanvas();
    Week = CanvasSprite::create(21,2);
    Weekcanvas = Week->getSpriteCanvas();
    DrawWeek(AlarmTime.u8Week,enAlarmState,Weekcanvas,SettingWeekIdx);
    // Switch = CanvasSprite::create(6,2);
    // Switchcanvas = Switch->getSpriteCanvas();
    // Switchcanvas->writeFillRect(0,0,2,2,DTRGB(255,0,0));
    // Switchcanvas->writeFillRect(2,0,2,2,DTRGB(255,255,255));
    // Switchcanvas->writeFillRect(4,0,2,2,DTRGB(0,255,0));
    Week->setPosition(10,6);
    Hour->setTransparent(false);
    Hour->setPosition(10,1);
    TimePt->setTransparent(false);
    TimePt->setPosition(18,1);
    Min->setTransparent(false);    
    Min->setPosition(20,1);
    // if(AlarmTime.boActive)
    // {
    //     Switch->setPosition(26,3);
    // }
    // else
    // {
    //     Switch->setPosition(28,3);
    // }    
    // this->addChild(Switch);
    this->addChild(ClkIcon);
    this->addChild(Hour);
    this->addChild(TimePt);
    this->addChild(Min);
    this->addChild(Week);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(AlarmClkLayer::AlarmUpdate),0.1);
    return true;
}

void AlarmClkLayer::AlarmUpdate(float dt)
{
    switch(enAlarmState)
    {
        case State_AlarmDis:
            StateDisShow();
            break;
        case State_AlarmSetMin:
            StateSetMinShow();
            break;
        case State_AlarmSetHour:
            StateSetHourShow();
            break;
        case State_AlarmSetWeek:
            StateSetWeekShow();
            break;
        default:break;
    }
    //Serial.printf("Alarm Clk sts:%d alarm idx:%d week idx:%d time: %d:%d:%x \n",enAlarmState,u8CurrentAlarm,SettingWeekIdx,AlarmTime.u8Hour,AlarmTime.u8Min,AlarmTime.u8Week);
}

void AlarmClkLayer::BtnClickHandler(int8_t keyCode, Event* event)
{
    AlarmStateMachine(keyCode,enKey_ShortPress);
}

void AlarmClkLayer::BtnDoubleClickHandler(int8_t keyCode, Event* event)
{
    AlarmStateMachine(keyCode,enKey_DoubleClick);
}

void AlarmClkLayer::BtnLongPressStartHandler(int8_t keyCode, Event* event)
{
    AlarmStateMachine(keyCode,enKey_LongPressStart);
}

void AlarmClkLayer::BtnDuringLongPressHandler(int8_t keyCode, Event* event)
{
    AlarmStateMachine(keyCode,enKey_LongPress);
}

void AlarmClkLayer::DrawWeek(uint8_t week, tenAlarmState AlarmState,SpriteCanvas *weekcanvas,uint8_t settingidx)
{
    weekcanvas->canvasReset();
    if(State_AlarmSetWeek != AlarmState)
    {
        for(uint8_t i = 0;i < 7;i++)
        {
            if(week & (1<<i))
            {
                weekcanvas->drawLine(i*3,1,i*3 + 1,1,DTRGB(0,255,0));
            }
            else
            {
                weekcanvas->drawLine(i*3,1,i*3 + 1,1,DTRGB(255,255,255));
            }
        }
    }
    else
    {
        for(uint8_t i = 0;i < 7;i++)
        {
            if(week & (1<<i))
            {
                if(settingidx == i)
                {
                    weekcanvas->drawLine(i*3,0,i*3 + 1,0,DTRGB(0,255,0));
                }
                else
                {
                    weekcanvas->drawLine(i*3,1,i*3 + 1,1,DTRGB(0,255,0));
                }
                
            }
            else
            {
                if(settingidx == i)
                {
                    weekcanvas->drawLine(i*3,0,i*3 + 1,0,DTRGB(255,255,255));
                }
                else
                {
                    weekcanvas->drawLine(i*3,1,i*3 + 1,1,DTRGB(255,255,255));
                }
                
            }
        }
    }
}

void AlarmClkLayer::ClearAnimationTmp(void)
{
    auto HourSprtTmp = getChildByTag(AnimationHourTag);
    if(HourSprtTmp)
    {
        if(0 == HourSprtTmp->getNumberOfRunningActions())
        {
            removeChild(HourSprtTmp);
        }
    }
    auto PtSprtTmp = getChildByTag(AnimationPtTag);
    if(PtSprtTmp)
    {
        if(0 == PtSprtTmp->getNumberOfRunningActions())
        {
            removeChild(PtSprtTmp);
        }
    }

    auto MinSprtTmp = getChildByTag(AnimationMinTag);
    if(MinSprtTmp)
    {
        if(0 == MinSprtTmp->getNumberOfRunningActions())
        {
            removeChild(MinSprtTmp);
        }
    }

    auto WeekSprtTmp = getChildByTag(AnimationWeekTag);
    if(WeekSprtTmp)
    {
        if(0 == WeekSprtTmp->getNumberOfRunningActions())
        {
            removeChild(WeekSprtTmp);
        }
    }
    
}


void AlarmClkLayer::SwitchAlarmAnimation(tstAlarmClk OldAlarm,bool boUp)
{
    DTRGB clkcolor;
    DTRGB newclkcolor;
    if(OldAlarm.boActive)
    {
        clkcolor.r = 0;
        clkcolor.g = 255;
        clkcolor.b = 0;
    }
    else
    {
        clkcolor.r = 255;
        clkcolor.g = 255;
        clkcolor.b = 255;
    } 
    if(AlarmTime.boActive)
    {
        newclkcolor.r = 0;
        newclkcolor.g = 255;
        newclkcolor.b = 0;
    }
    else
    {
        newclkcolor.r = 255;
        newclkcolor.g = 255;
        newclkcolor.b = 255;
    } 
    TextSprite* HourTmp = TextSprite::create(Size(8,5),Size(8,5),clkcolor,(std::to_string(OldAlarm.u8Hour/10) + std::to_string(OldAlarm.u8Hour%10)),TextSprite::TextAlign::TextAlignRight,&TomThumb);
    HourTmp->setPosition(10,1);
    HourTmp->setTransparent(false);   
    TextSprite* TimePtTmp = TextSprite::create(Size(2,5),Size(2,5),clkcolor,":",TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    TimePtTmp->setPosition(18,1);
    TimePtTmp->setTransparent(false);    
    TextSprite* MinTmp = TextSprite::create(Size(8,5),Size(8,5),clkcolor,(std::to_string(OldAlarm.u8Min/10) + std::to_string(OldAlarm.u8Min%10)),TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    MinTmp->setPosition(20,1);
    MinTmp->setTransparent(false);    
    CanvasSprite* WeekTmp = CanvasSprite::create(21,2);
    WeekTmp->setPosition(10,6);
    WeekTmp->setTransparent(false);
    DrawWeek(OldAlarm.u8Week,enAlarmState,WeekTmp->getSpriteCanvas(),SettingWeekIdx); 
    
    this->addChild(HourTmp,0,AnimationHourTag);
    this->addChild(TimePtTmp,0,AnimationPtTag);
    this->addChild(MinTmp,0,AnimationMinTag);
    this->addChild(WeekTmp,0,AnimationWeekTag);

    std::string hour = std::to_string(AlarmTime.u8Hour/10) + std::to_string(AlarmTime.u8Hour%10);        
    Hourcanvas->setTextColor(newclkcolor);
    Hourcanvas->canvasReset();
    Hourcanvas->print(hour.c_str());
    
    TimePtcanvas->setTextColor(newclkcolor);
    TimePtcanvas->canvasReset();
    TimePtcanvas->print(":");    
    
    std::string min = std::to_string(AlarmTime.u8Min/10) + std::to_string(AlarmTime.u8Min%10);
    Mincanvas->setTextColor(newclkcolor);    
    Mincanvas->canvasReset();
    Mincanvas->print(min.c_str());    
    
    DrawWeek(AlarmTime.u8Week,enAlarmState,Week->getSpriteCanvas(),SettingWeekIdx);
    
    if(boUp)
    {
        Sequence *Seq = Sequence::createWithTwoActions(MoveBy::create(0.5,Vec2(0,-8)), CallFunc::create(DT_CALLBACK_0(AlarmClkLayer::ClearAnimationTmp,this)));   
        HourTmp->runAction(MoveBy::create(0.5,Vec2(0,-8)));
        TimePtTmp->runAction(MoveBy::create(0.5,Vec2(0,-8)));
        MinTmp->runAction(MoveBy::create(0.5,Vec2(0,-8)));
        WeekTmp->runAction(Seq); 
        Hour->setPosition(10,9);
        Hour->runAction(MoveBy::create(0.5,Vec2(0,-8)));
        TimePt->setPosition(18,9);
        TimePt->runAction(MoveBy::create(0.5,Vec2(0,-8)));  
        Min->setPosition(20,9);
        Min->runAction(MoveBy::create(0.5,Vec2(0,-8)));
        Week->setPosition(10,14);
        Week->runAction(MoveBy::create(0.5,Vec2(0,-8)));
    }
    else
    {
        Sequence *Seq = Sequence::createWithTwoActions(MoveBy::create(0.5,Vec2(0,8)), CallFunc::create(DT_CALLBACK_0(AlarmClkLayer::ClearAnimationTmp,this)));   
        HourTmp->runAction(MoveBy::create(0.5,Vec2(0,8)));
        TimePtTmp->runAction(MoveBy::create(0.5,Vec2(0,8)));
        MinTmp->runAction(MoveBy::create(0.5,Vec2(0,8)));
        WeekTmp->runAction(Seq); 
        Hour->setPosition(10,-7);
        Hour->runAction(MoveBy::create(0.5,Vec2(0,8)));
        TimePt->setPosition(18,-7);
        TimePt->runAction(MoveBy::create(0.5,Vec2(0,8)));  
        Min->setPosition(20,-7);
        Min->runAction(MoveBy::create(0.5,Vec2(0,8)));
        Week->setPosition(10,-2);
        Week->runAction(MoveBy::create(0.5,Vec2(0,8)));
    } 
    
}

void AlarmClkLayer::StateDisShow(void)
{
    static tstAlarmClk oldAlarmClk= AlarmTime;
    static uint8_t oldAlarmidx = 0;
    DTRGB textcolor = {255,255,255};
    if(oldAlarmClk.boActive != AlarmTime.boActive)
    {
        if(AlarmTime.boActive)
        {
            textcolor.r = 0;
            textcolor.g = 255;
            textcolor.b = 0;
            ClkIcon->setSpriteFrame(SpriteFrame::create(icon_ClockActive,sizeof(icon_ClockActive),BMP_GIF));
            // MoveBy *Move = MoveBy::create(0.1,Vec2(-2,0));
            // Switch->runAction(Move);        
        }
        else
        {
            ClkIcon->setSpriteFrame(SpriteFrame::create(icon_ClockInactive,sizeof(icon_ClockInactive),BMP_BMP));
            // MoveBy *Move = MoveBy::create(0.1,Vec2(2,0));
            // Switch->runAction(Move);
        }
    } 
    
    if(oldAlarmidx != u8CurrentAlarm)
    {
        if(oldAlarmidx < u8CurrentAlarm)
        {
            SwitchAlarmAnimation(oldAlarmClk,true);
        }
        else
        {
            SwitchAlarmAnimation(oldAlarmClk,false);
        }
    }
    else
    {
        if(AlarmTime.u8Hour != oldAlarmClk.u8Hour || AlarmTime.boActive != oldAlarmClk.boActive)
        {
            std::string hour = std::to_string(AlarmTime.u8Hour/10) + std::to_string(AlarmTime.u8Hour%10);        
            Hourcanvas->setTextColor(textcolor);
            Hourcanvas->canvasReset();
            Hourcanvas->print(hour.c_str());
            TimePtcanvas->setTextColor(textcolor);
            TimePtcanvas->canvasReset();
            TimePtcanvas->print(":");
        }
        if(AlarmTime.u8Min != oldAlarmClk.u8Min || AlarmTime.boActive != oldAlarmClk.boActive)
        {
            Mincanvas->setTextColor(textcolor);
            std::string min = std::to_string(AlarmTime.u8Min/10) + std::to_string(AlarmTime.u8Min%10);
            Mincanvas->canvasReset();
            Mincanvas->print(min.c_str());
        }

        DrawWeek(AlarmTime.u8Week,enAlarmState,Weekcanvas,SettingWeekIdx);
    }
    

    oldAlarmClk = AlarmTime;
    oldAlarmidx = u8CurrentAlarm;
}

void AlarmClkLayer::StateSetMinShow(void)
{
    static tstAlarmClk oldAlarmClk= {0,0,0,false};
    if(0 != Hour->getNumberOfRunningActions())
    {
        Hour->stopAllActions();
        Hour->setVisible(true);
    }
    if(0 == Min->getNumberOfRunningActions())
    {
        Min->runAction(RepeatForever::create(Blink::create(1,2)));
    }
    // if(AlarmTime.u8Hour != oldAlarmClk.u8Hour)
    // {
    //     std::string hour = std::to_string(AlarmTime.u8Hour/10) + std::to_string(AlarmTime.u8Hour%10);        
    //     Hourcanvas->canvasReset();
    //     Hourcanvas->print(hour.c_str());
    // }
    if(AlarmTime.u8Min != oldAlarmClk.u8Min)
    {
        std::string min = std::to_string(AlarmTime.u8Min/10) + std::to_string(AlarmTime.u8Min%10);
        Mincanvas->canvasReset();
        Mincanvas->print(min.c_str());
    }
    if(AlarmTime.u8Week != oldAlarmClk.u8Week)
    {
        DrawWeek(AlarmTime.u8Week,enAlarmState,Weekcanvas,SettingWeekIdx);
    }
    oldAlarmClk = AlarmTime;
}

void AlarmClkLayer::StateSetHourShow(void)
{
    static tstAlarmClk oldAlarmClk= {0,0,0,false};
    if(0 == Hour->getNumberOfRunningActions())
    {
        Hour->runAction(RepeatForever::create(Blink::create(1,2)));
    }
    if(0 != Min->getNumberOfRunningActions())
    {
        Min->stopAllActions();
        Min->setVisible(true);
    }
    if(AlarmTime.u8Hour != oldAlarmClk.u8Hour)
    {
        std::string hour = std::to_string(AlarmTime.u8Hour/10) + std::to_string(AlarmTime.u8Hour%10);        
        Hourcanvas->canvasReset();
        Hourcanvas->print(hour.c_str());
    }
    // if(AlarmTime.u8Min != oldAlarmClk.u8Min)
    // {
    //     std::string min = std::to_string(AlarmTime.u8Min/10) + std::to_string(AlarmTime.u8Min%10);
    //     Mincanvas->canvasReset();
    //     Mincanvas->print(min.c_str());
    // }
    if(AlarmTime.u8Week != oldAlarmClk.u8Week)
    {
        DrawWeek(AlarmTime.u8Week,enAlarmState,Weekcanvas,SettingWeekIdx);
    }
    oldAlarmClk = AlarmTime;
}

void AlarmClkLayer::StateSetWeekShow(void)
{
    static tstAlarmClk oldAlarmClk= {0,0,0,false};
    static uint8_t oldSettingWeek = 0;
    if(0 != Hour->getNumberOfRunningActions())
    {
        Hour->stopAllActions();
        Hour->setVisible(true);
    }
    if(0 != Min->getNumberOfRunningActions())
    {
        Min->stopAllActions();
        Min->setVisible(true);
    }
    // if(AlarmTime.u8Hour != oldAlarmClk.u8Hour)
    // {
    //     std::string hour = std::to_string(AlarmTime.u8Hour/10) + std::to_string(AlarmTime.u8Hour%10);        
    //     Hourcanvas->canvasReset();
    //     Hourcanvas->print(hour.c_str());
    // }
    // if(AlarmTime.u8Min != oldAlarmClk.u8Min)
    // {
    //     std::string min = std::to_string(AlarmTime.u8Min/10) + std::to_string(AlarmTime.u8Min%10);
    //     Mincanvas->canvasReset();
    //     Mincanvas->print(min.c_str());
    // }
    // if((AlarmTime.u8Week != oldAlarmClk.u8Week) || (oldSettingWeek != SettingWeekIdx))
    // {
    //     DrawWeek(AlarmTime.u8Week,enAlarmState,Weekcanvas,SettingWeekIdx);
    // }
    DrawWeek(AlarmTime.u8Week,enAlarmState,Weekcanvas,SettingWeekIdx);
    oldAlarmClk = AlarmTime;
    oldSettingWeek = SettingWeekIdx;
}

void AlarmClkLayer::StateDisHandle(int8_t key_type, int8_t key_event)
{
    if(enKey_OK == key_type)
    {
        if(enKey_ShortPress == key_event)//next alarm clock
        {
            if(++u8CurrentAlarm >= u8TotalAlarmNum)
            {
                u8CurrentAlarm = 0;
            }
            AlarmTime = stGetAlarmClk(u8CurrentAlarm);
        }
        else if(enKey_LongPressStart == key_event)//enter setting state
        {
            enAlarmState = State_AlarmSetMin;
        }
    }
    else if(enKey_Left == key_type)
    {
        if(enKey_ShortPress == key_event) //previous alarm clock
        {
            if(--u8CurrentAlarm >= u8TotalAlarmNum)
            {
                u8CurrentAlarm = u8TotalAlarmNum-1;
            }
            AlarmTime = stGetAlarmClk(u8CurrentAlarm);
        }
        else if(enKey_DoubleClick == key_event)//enable the alarm clock
        {
            if(false == AlarmTime.boActive)
            {
                AlarmTime.boActive = true;
            }
            
            boSetAlarmClk(u8CurrentAlarm,&AlarmTime);
        }
        else if(enKey_LongPressStart == key_event)//add new alarm clock
        {
            tstAlarmClk newAlarmClk = {0,0,0,false};
            if(boAddAlarmClk(&newAlarmClk))
            {
                u8CurrentAlarm++;
                u8TotalAlarmNum = u8GetAlarmClkNum();
                AlarmTime = stGetAlarmClk(u8CurrentAlarm);
            }
        }
    }
    else if(enKey_Right == key_type)
    {
        if(enKey_ShortPress == key_event) //next alarm clock
        {
            if(++u8CurrentAlarm >= u8TotalAlarmNum)
            {
                u8CurrentAlarm = 0;
            }
            AlarmTime = stGetAlarmClk(u8CurrentAlarm);
        }
        else if(enKey_DoubleClick == key_event)//disable the alarm clock
        {
                if(true == AlarmTime.boActive)
                {
                    AlarmTime.boActive = false;
                }
                boSetAlarmClk(u8CurrentAlarm,&AlarmTime);
        }
        else if(enKey_LongPressStart == key_event)//delete the alarm clock
        {
            if(u8TotalAlarmNum > 1)
            {
                if(boDelAlarmClk(u8CurrentAlarm))
                {
                    u8CurrentAlarm--;
                    u8TotalAlarmNum = u8GetAlarmClkNum();
                    AlarmTime = stGetAlarmClk(u8CurrentAlarm);
                }
            }
            
        }
    }
}

void AlarmClkLayer::StateSetMinHandle(int8_t key_type, int8_t key_event)
{
    if(enKey_OK == key_type)
    {
        if(enKey_ShortPress == key_event)//enter hour setting state
        {
            enAlarmState = State_AlarmSetHour;
        }
    }
    else if(enKey_Left == key_type)
    {
        /*minutes - 1*/
        if(--AlarmTime.u8Min >= 60)
        {
            AlarmTime.u8Min = 59;
        }
    }
    else if(enKey_Right == key_type)
    {
        /*minutes + 1*/
        if(++AlarmTime.u8Min >= 60)
        {
            AlarmTime.u8Min = 0;
        }
    }
}

void AlarmClkLayer::StateSetHourHandle(int8_t key_type, int8_t key_event)
{
    if(enKey_OK == key_type)
    {
        if(enKey_ShortPress == key_event)//enter week setting state
        {
            enAlarmState = State_AlarmSetWeek;
            SettingWeekIdx = 0;
        }
    }
    else if(enKey_Left == key_type)
    {
        /*hour - 1*/
        if(--AlarmTime.u8Hour >= 24)
        {
            AlarmTime.u8Hour = 23;
        }
    }
    else if(enKey_Right == key_type)
    {
        /*hour + 1*/
        if(++AlarmTime.u8Hour >= 24)
        {
            AlarmTime.u8Hour = 0;
        }
    }
}

void AlarmClkLayer::StateSetWeekHandle(int8_t key_type, int8_t key_event)
{
    if(enKey_OK == key_type)
    {
        if(enKey_ShortPress == key_event)//current day need enable
        {
            if(AlarmTime.u8Week & (1<<SettingWeekIdx))
            {
                AlarmTime.u8Week &= ~(1<<SettingWeekIdx);
            }
            else
            {
                AlarmTime.u8Week |= (1<<SettingWeekIdx);
            }
            
        }
        else if(enKey_LongPressStart == key_event)//finish this alarm setting go back to display state
        {
            boSetAlarmClk(u8CurrentAlarm,&AlarmTime);
            SettingWeekIdx = 0;
            enAlarmState = State_AlarmDis;
        }
    }
    else if(enKey_Left == key_type)
    {
        /*week index -1*/
        if(--SettingWeekIdx >=6)
        {
            SettingWeekIdx = 6;
        }
    }
    else if(enKey_Right == key_type)
    {
        /*minutes + 1*/
        if(++SettingWeekIdx >=7)
        {
            SettingWeekIdx = 0;
        }
    }
}

void AlarmClkLayer::AlarmStateMachine(int8_t key_type, int8_t key_event)
{  
    switch (enAlarmState)
    {
        case State_AlarmDis:
            StateDisHandle(key_type,key_event);
            break;
        case State_AlarmSetMin:
            StateSetMinHandle(key_type,key_event);
            break;
        case State_AlarmSetHour:
            StateSetHourHandle(key_type,key_event);
            break;
        case State_AlarmSetWeek:
            StateSetWeekHandle(key_type,key_event);
            break;
        default:
            break;
    }
}


NS_DT_END