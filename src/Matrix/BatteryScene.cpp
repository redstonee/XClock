#include "BatteryScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "../main.h"
#include <Arduino.h>
NS_DT_BEGIN

bool BattScene::init()
{
    BattLayer *BatteryLayer = BattLayer::create();
    BatteryLayer->setContentSize(Size(32,8));
    BatteryLayer->setPosition(0,0);
    //ClockLayer->setOpacity
    this->addChild(BatteryLayer);
    BatteryLayer->initLayer();
    return true;
}

bool BattLayer::initLayer()
{
    DTRGB strcolor;
    BattSts = stGetBattSts();
    //auto listener = EventListenerButton::create();
    // listener ->onBtnClick = [&](int8_t keyCode , Event * event )
    // {
    //     Serial.printf("clicked %d \n",keyCode);
    // };
    // _eventDispatcher->addEventListenerWithSceneGraphPriority ( listener, this );
    batticon = FrameSprite::create(pGetBattIconSprt(BattSts));
    if(batticon != nullptr)
    {
            batticon->setPosition(1,0);
            batticon->setAutoSwitch(true);
            this->addChild(batticon);
    }   
    std::string BattLvlstr = std::to_string(BattSts.BattLvl)+"%";
    Serial.printf("Battlvl %s \n",BattLvlstr.c_str());
    if(BattSts.BattLvl < 20)
    {
        strcolor = DTRGB(0xff,0x2b,0x00); //red
    }
    else if(BattSts.BattLvl < 60)
    {
        strcolor = DTRGB(0xff,0xff,0x00); //yellow
    }
    else
    {
        strcolor = DTRGB(0x0d,0xff,0x00); //green
    }  

    BattLvltxt = TextSprite::create(Size(18,5),Size(18,5),strcolor,BattLvlstr,TextSprite::TextAlign::TextAlignCenter,&TomThumb);
    BattLvltxtcanvas = BattLvltxt->getSpriteCanvas();
    BattLvltxt->setTransparent(true);
    BattLvltxt->setPosition(13,1);
    this->addChild(BattLvltxt);
    this->scheduleUpdate();
    return true;
}


SpriteFrame* BattLayer::pGetBattIconSprt(tstBattSts battsts)
{
    SpriteFrame* batticon = nullptr;
    if(battsts.boCharging)
    {
        batticon = SpriteFrame::create(icon_charging,sizeof(icon_charging),BMP_GIF);
    }
    else
    {
        uint8_t battlvl = battsts.BattLvl/(BAT_FULL_LVL/5);
        switch(battlvl)
        {
            case 0:
                batticon = SpriteFrame::create(icon_battlvl_0,sizeof(icon_battlvl_0),BMP_BMP);
                break;
            case 1:
                batticon = SpriteFrame::create(icon_battlvl_1,sizeof(icon_battlvl_1),BMP_BMP);
                break;
            case 2:
                batticon = SpriteFrame::create(icon_battlvl_2,sizeof(icon_battlvl_2),BMP_BMP);
                break;
            case 3:
                batticon = SpriteFrame::create(icon_battlvl_3,sizeof(icon_battlvl_3),BMP_BMP);
                break;
            case 4:
                batticon = SpriteFrame::create(icon_battlvl_4,sizeof(icon_battlvl_4),BMP_BMP);
                break;
            case 5:
            default:
                batticon = SpriteFrame::create(icon_battlvl_5,sizeof(icon_battlvl_5),BMP_BMP);
                break;
        } 
    }
    return batticon;
}


void BattLayer::update(float dt)
{
    tstBattSts BattStsTmp = stGetBattSts();

    if(BattStsTmp.boCharging != BattSts.boCharging)
    {
        batticon->setSpriteFrame(pGetBattIconSprt(BattSts));   
    }

    if(BattStsTmp.BattLvl != BattSts.BattLvl)
    {       
        DTRGB strcolor;
        std::string BattLvlstr = std::to_string(BattStsTmp.BattLvl)+"%";
        Serial.printf("Battlvl %s \n",BattLvlstr.c_str());
        if(BattStsTmp.BattLvl < 20)
        {
            strcolor = DTRGB(0xff,0x2b,0x00); //red
        }
        else if(BattSts.BattLvl < 60)
        {
            strcolor = DTRGB(0xff,0xff,0x00); //yellow
        }
        else
        {
            strcolor = DTRGB(0x0d,0xff,0x00); //green
        }  
        BattLvltxtcanvas->canvasReset();
        BattLvltxtcanvas->setTextColor(strcolor);
        BattLvltxtcanvas->print(BattLvlstr.c_str());
    }
    BattSts = BattStsTmp;
}
NS_DT_END