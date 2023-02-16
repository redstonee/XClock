#ifndef __CLOCKSCENE_H__
#define __CLOCKSCENE_H__

#include "Dot2D/dot2d.h"
#include "../RTC/SD3078.h"

NS_DT_BEGIN

class ClockScene : public Scene
{

protected:
    bool init() override;

public:

    STATIC_CREATE(ClockScene);

};


class TimeLayer : public Layer
{

protected:
    TextSprite *Hour_1 = nullptr;
    SpriteCanvas *Hour1canvas = nullptr;
    TextSprite *Hour_2 = nullptr;
    SpriteCanvas *Hour2canvas = nullptr;
    TextSprite *MinPt = nullptr;
    SpriteCanvas *MinPtcanvas = nullptr;    
    TextSprite *Min_1 = nullptr;
    SpriteCanvas *Min1canvas = nullptr;
    TextSprite *Min_2 = nullptr;
    SpriteCanvas *Min2canvas = nullptr;
    TextSprite *SecPt = nullptr;
    SpriteCanvas *SecPtcanvas = nullptr;
    TextSprite *Sec_1 = nullptr;
    SpriteCanvas *Sec1canvas = nullptr;
    TextSprite *Sec_2 = nullptr;
    SpriteCanvas *Sec2canvas = nullptr;
    SD3078* SD3078Time = nullptr;
    tst3078Time ClockTime;
    float interval;
    bool ScrollUp = false;
public:

    STATIC_CREATE(TimeLayer);

    virtual bool initLayer();

    void TimeUpdate(float dt);
};

NS_DT_END

#endif //__CLOCKSCENE_H__