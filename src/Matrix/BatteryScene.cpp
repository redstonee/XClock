#include "BatteryScene.h"
#include "Dot2D/third/gfxfont.h"
#include "Dot2D/third/Picopixel.h"
#include "Dot2D/third/TomThumb.h"
#include "Icon.h"

#include <Arduino.h>
#include "BattMon.h"

NS_DT_BEGIN

static const char *TAG = "BattScene";

bool BattScene::init()
{
    BattLayer *BatteryLayer = BattLayer::create();
    BatteryLayer->setContentSize(Size(32, 8));
    BatteryLayer->setPosition(0, 0);
    // ClockLayer->setOpacity
    this->addChild(BatteryLayer);
    BatteryLayer->initLayer();
    return true;
}

SpriteFrame *BattLayer::pGetBattIconSprt(bool charging, uint8_t batteryLevel)
{
    if (BattMon::isCharging())
    {
        return SpriteFrame::create(icon_charging, sizeof(icon_charging), BMP_GIF);
    }

    switch (batteryLevel / 20)
    {
    case 0:
        return SpriteFrame::create(icon_battlvl_0, sizeof(icon_battlvl_0), BMP_BMP);

    case 1:
        return SpriteFrame::create(icon_battlvl_1, sizeof(icon_battlvl_1), BMP_BMP);

    case 2:
        return SpriteFrame::create(icon_battlvl_2, sizeof(icon_battlvl_2), BMP_BMP);

    case 3:
        return SpriteFrame::create(icon_battlvl_3, sizeof(icon_battlvl_3), BMP_BMP);

    case 4:
        return SpriteFrame::create(icon_battlvl_4, sizeof(icon_battlvl_4), BMP_BMP);

    case 5:
    default:
        return SpriteFrame::create(icon_battlvl_5, sizeof(icon_battlvl_5), BMP_BMP);
    }
}

std::pair<String, DTRGB> genBattLevelStr(uint8_t batteryLevel)
{
    auto BattLvlstr = String(batteryLevel) + " %";
    ESP_LOGI(TAG, "Battlvl %s", BattLvlstr.c_str());

    DTRGB color;
    if (batteryLevel < 20)
    {
        color = DTRGB(0xff, 0x2b, 0x00); // red
    }
    else if (batteryLevel < 60)
    {
        color = DTRGB(0xff, 0xff, 0x00); // yellow
    }
    else
    {
        color = DTRGB(0x0d, 0xff, 0x00); // green
    }
    return {BattLvlstr, color};
}

bool BattLayer::initLayer()
{
    batteryLevel = BattMon::getBatteryLevel();
    isCharging = BattMon::isCharging();
    batticon = FrameSprite::create(pGetBattIconSprt(isCharging, batteryLevel));

    if (batticon != nullptr)
    {
        batticon->setPosition(1, 0);
        batticon->setAutoSwitch(true);
        this->addChild(batticon);
    }

    auto [batteryLevelStr, strcolor] = genBattLevelStr(batteryLevel);
    BattLvltxt = TextSprite::create(Size(18, 5), Size(18, 5), strcolor, batteryLevelStr.c_str(), TextSprite::TextAlign::TextAlignCenter, &TomThumb);
    BattLvltxtcanvas = BattLvltxt->getSpriteCanvas();
    BattLvltxt->setTransparent(true);
    BattLvltxt->setPosition(13, 1);
    this->addChild(BattLvltxt);
    this->scheduleUpdate();
    return true;
}

void BattLayer::update(float dt)
{
    auto batteryLevelNew = BattMon::getBatteryLevel();
    auto isChargingNew = BattMon::isCharging();

    if (isChargingNew != isCharging)
    {
        batticon->setSpriteFrame(pGetBattIconSprt(isChargingNew, batteryLevelNew));
        isCharging = isChargingNew;
    }

    if (batteryLevelNew != batteryLevel)
    {
        auto [batteryLevelStr, strcolor] = genBattLevelStr(batteryLevelNew);
        BattLvltxtcanvas->canvasReset();
        BattLvltxtcanvas->setTextColor(strcolor);
        BattLvltxtcanvas->print(batteryLevelStr.c_str());

        batteryLevel = batteryLevelNew;
    }
}
NS_DT_END