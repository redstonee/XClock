#ifndef __TIMER_H__
#define __TIMER_H__
#include <Arduino.h>
#include "Dot2D/dot2d.h"

NS_DT_BEGIN
 
    bool boIsTimerCounterActive(void);          


class TimerScene : public Scene
{

protected:
    bool init() override;
public:

    STATIC_CREATE(TimerScene);
};


class TimerLayer : public Layer
{
protected:
    TextSprite *MSec = nullptr;
    SpriteCanvas *MSeccanvas = nullptr;
	TextSprite *TimePt1 = nullptr;
	SpriteCanvas *TimePt1canvas = nullptr;
    TextSprite *Min = nullptr;
    SpriteCanvas *Mincanvas = nullptr;
	TextSprite *TimePt2 = nullptr;
	SpriteCanvas *TimePt2canvas = nullptr;
	TextSprite *Sec = nullptr;
    SpriteCanvas *Seccanvas = nullptr;

public:
	
    STATIC_CREATE(TimerLayer);

    virtual bool initLayer();

    void TimerUpdate(float dt);

	void BtnClickHandler(int8_t, Event*);

	void BtnDoubleClickHandler(int8_t, Event*);

	void BtnLongPressStartHandler(int8_t, Event*);

	void BtnDuringLongPressHandler(int8_t, Event*);

};


NS_DT_END

#endif //__TIMER_H__