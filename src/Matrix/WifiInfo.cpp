/****************************************************************************
Copyright (c) 2021      Yinbaiyuan

https://www.yinbaiyuan.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "FastLED.h"
#include <Arduino.h>
#include "WifiInfo.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "../web/web.h"
#include "../Key/ClockKey.h"
NS_DT_BEGIN

bool WifiInfo::init()
{
    WifiInfoLayer *rootLayer = WifiInfoLayer::create();
    rootLayer->setContentSize(Size(32,8));
    rootLayer->setPosition(0,1);
    this->addChild(rootLayer);
    rootLayer->initLayer();
    return true;
}

WifiInfoLayer::~WifiInfoLayer()
{
    
}

bool WifiInfoLayer::initLayer()
{
    DTRGB TextColor = {255,255,255};
    if(IsWifiConfig())
    {
        WifiInfoTxt = TextSprite::create(Size(32,5),Size(128,5),TextColor,GetWifiSSID().c_str(),TextSprite::TextAlign::TextAlignScroll,&TomThumb);
        WifiInfoTxt->setAutoScroll(TextSprite::ScrollType::Translate,32,32,0.2);
        boWifiConfiged = true;
    }
    else
    {
        vWifiInit();
        WifiInfoTxt = TextSprite::create(Size(32,5),Size(128,5),TextColor,"Please connect XClock_AP",TextSprite::TextAlign::TextAlignScroll,&TomThumb);
        WifiInfoTxt->setAutoScroll(TextSprite::ScrollType::Translate,32,32,0.2);
        boWifiConfiged = false;
    }
    WifiTxtcanvas = WifiInfoTxt->getSpriteCanvas();
    auto listener = EventListenerButton::create();
    listener ->onBtnLongPressStart = DT_CALLBACK_2(WifiInfoLayer::BtnLongPressStartHandler,this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority ( listener, this );
    this->addChild(WifiInfoTxt);
    this->scheduleUpdate();
    this->schedule(DT_SCHEDULE_SELECTOR(WifiInfoLayer::update),1);
    return true;
}

void WifiInfoLayer::update(float dt)
{    
    if(boWifiConfiged == false)
    {
        if(IsWifiConfig())
        {
            WifiTxtcanvas->canvasReset();
            WifiTxtcanvas->print(GetWifiSSID().c_str());
            boWifiConfiged = true;
        } 
    }
}

void WifiInfoLayer::BtnLongPressStartHandler(int8_t keyCode, Event* event)
{
    if(enKey_OK == keyCode)
    {
        boWifiConfiged = false;
        restoreWiFi();
        vWifiInit();
        WifiTxtcanvas->print("Please connect XClock_AP");
    }
}

NS_DT_END