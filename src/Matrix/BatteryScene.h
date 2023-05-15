
#ifndef __BATTSCENE_H__
#define __BATTSCENE_H__

#include "Dot2D/dot2d.h"
#include "../main.h"

NS_DT_BEGIN

class BattScene : public Scene
{

protected:

    bool init() override;

public:

    STATIC_CREATE(BattScene);

};

class BattLayer : public Layer
{
protected:
    CanvasSprite* canvasSprite = nullptr;
    FrameSprite* batticon = nullptr;
    TextSprite *BattLvltxt = nullptr;
    SpriteCanvas *BattLvltxtcanvas = nullptr;
    tstBattSts BattSts = {0,0};

protected:    

    SpriteFrame* pGetBattIconSprt(tstBattSts);

public:

    STATIC_CREATE(BattLayer);

    virtual bool initLayer();
    
    void update(float dt) override;

};




NS_DT_END

#endif //__BATTSCENE_H__