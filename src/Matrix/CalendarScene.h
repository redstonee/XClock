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
} tenMonthState;

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
	TextSprite *yearText = nullptr;
	SpriteCanvas *yearCanvas = nullptr;
	tm ClockTime;
	tm ClockTimeSetting;
	uint8_t ColorIndex = 0;
	uint8_t PaletteIndex = 0;
	bool boYearSetting = false;

public:
	STATIC_CREATE(YearLayer);

	virtual bool initLayer();

	void YearUpdate(float dt);

	void BtnClickHandler(int8_t, Event *);

	void BtnLongPressStartHandler(int8_t, Event *);

	void BtnDuringLongPressHandler(int8_t, Event *);

	void SendSettingYear(tm *);
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
	TextSprite *monthText = nullptr;
	SpriteCanvas *monthCanvas = nullptr;
	TextSprite *dayText = nullptr;
	SpriteCanvas *dayCanvas = nullptr;
	TextSprite *Split = nullptr;
	SpriteCanvas *Splitcanvas = nullptr;
	tm ClockTime;
	tm ClockTimeSetting;
	uint8_t ColorIndex = 0;
	uint8_t PaletteIndex = 0;
	tenMonthState enMonthState = State_MonthDis;

public:
	STATIC_CREATE(MonthLayer);

	virtual bool initLayer();

	void MonthUpdate(float dt);

	void BtnClickHandler(int8_t, Event *);

	void BtnLongPressStartHandler(int8_t, Event *);

	void BtnDuringLongPressHandler(int8_t, Event *);

	void MonthStateMachine(int8_t, int8_t);

	void SendSettingMonth(const tm &settingtime);
};

NS_DT_END

#endif //__CALENDAR_H__