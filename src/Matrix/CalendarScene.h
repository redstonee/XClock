#ifndef __CALENDAR_H__
#define __CALENDAR_H__

#include "Dot2D/dot2d.h"
#include "SD3078.hpp"

NS_DT_BEGIN

typedef enum
{
	State_MonthDis = 0,
	State_MonthSet = 1,
	State_DaySet,
}tenMonthState;

class CalYearScene : public Scene
{

protected:
    bool init() override;

public:

    STATIC_CREATE(CalYearScene);

};


class YearLayer : public Layer
{
protected:
    TextSprite *YearH = nullptr;
    SpriteCanvas *YearHcanvas = nullptr;
    TextSprite *YearL = nullptr;
    SpriteCanvas *YearLcanvas = nullptr;
    tst3078Time ClockTime;
	tst3078Time ClockTimeSetting;
	QueueHandle_t TimeSettingQ;
	uint8_t ColorIndex = 0;
	uint8_t PaletteIndex = 0;
	bool boYearSetting = false;
public:
	
    STATIC_CREATE(YearLayer);

    virtual bool initLayer();

    void YearUpdate(float dt);

	void BtnClickHandler(int8_t, Event*);

	void BtnLongPressStartHandler(int8_t, Event*);

	void BtnDuringLongPressHandler(int8_t, Event*);

	void SendSettingYear(tst3078Time*);

};

class CalMonthScene : public Scene
{

protected:
    bool init() override;

public:

    STATIC_CREATE(CalMonthScene);

};


class MonthLayer : public Layer
{
protected:
    TextSprite *Month = nullptr;
    SpriteCanvas *Monthcanvas = nullptr;
    TextSprite *Day = nullptr;
    SpriteCanvas *Daycanvas = nullptr;
	TextSprite *Split = nullptr;
	SpriteCanvas *Splitcanvas = nullptr;
    tst3078Time ClockTime;
	tst3078Time ClockTimeSetting;
	QueueHandle_t TimeSettingQ;
	uint8_t ColorIndex = 0;
	uint8_t PaletteIndex = 0;
	tenMonthState enMonthState = State_MonthDis;
public:
	
    STATIC_CREATE(MonthLayer);

    virtual bool initLayer();

    void MonthUpdate(float dt);

	void BtnClickHandler(int8_t, Event*);

	void BtnLongPressStartHandler(int8_t, Event*);

	void BtnDuringLongPressHandler(int8_t, Event*);

	void MonthStateMachine(int8_t,int8_t);

	void SendSettingMonth(tst3078Time*);

};

NS_DT_END

#endif //__CALENDAR_H__