#ifndef __CLOCKSCENE_H__
#define __CLOCKSCENE_H__

#include "Dot2D/dot2d.h"
#include "SD3078.hpp"
#include "FastLED.h"
NS_DT_BEGIN

#define Hour1AniTmpTag (1)
#define Hour2AniTmpTag (2)
#define Min1AniTmpTag (3)
#define Min2AniTmpTag (4)
#define AlarmIconTag (5)

typedef enum
{
	State_TimeDis = 0,
	State_TimeSetMin = 1,
	State_TimeSetHour,
	State_TimeSetWeek,
} tenTimeState;

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
	TextSprite *hourText = nullptr;
	SpriteCanvas *hourCanvas = nullptr;
	TextSprite *minutePointText = nullptr;
	SpriteCanvas *minutePointCanvas = nullptr;
	TextSprite *minuteText = nullptr;
	SpriteCanvas *minuteCanvas = nullptr;
	TextSprite *SecPt = nullptr;
	SpriteCanvas *SecPtcanvas = nullptr;
	TextSprite *Sec_1 = nullptr;
	SpriteCanvas *Sec1canvas = nullptr;
	TextSprite *Sec_2 = nullptr;
	SpriteCanvas *Sec2canvas = nullptr;
	CanvasSprite *Week = nullptr;
	SpriteCanvas *Weekcanvas = nullptr;
	FrameSprite *Weather = nullptr;
	CanvasSprite *AlarmIcon = nullptr;
	SpriteCanvas *AlarmIconCanvas = nullptr;
	tm ClockTime;
	tm ClockTimeSetting;
	QueueHandle_t timeSettingQueue;
	float interval;
	bool ScrollUp = false;
	uint8_t ColorIndex = 0;
	uint8_t PaletteIndex = 0;
	CRGBPalette16 currentPalette;
	DTRGB timecolor;
	DTRGB weekcolor;
	tenTimeState enTimests = State_TimeDis;

public:
	STATIC_CREATE(TimeLayer);

	virtual bool initLayer();

	void TimeUpdate(float dt);

	void BtnClickHandler(int8_t, Event *);

	void BtnLongPressStartHandler(int8_t, Event *);

	void BtnDuringLongPressHandler(int8_t, Event *);

	void BtnDuringLongPressStopHandler(int8_t, Event *);

	void TimeStateMachine(int8_t, int8_t);

	void UpdateColor(int8_t, int8_t);

	void SendSettingTime(tm &);

	void updateHour(uint8_t hour);

	void updateMinute(uint8_t minute);

	void DrawWeek(uint8_t);

	void StateTimeDisShow(void);

	void StateSetMinShow(void);

	void StateSetHourShow(void);

	void StateSetWeekShow(void);

	void digitSwitchAnimation(TextSprite *OldDigitalSprt, uint8_t OldDigital, uint8_t NewDigital, int tmpTag);

	void CleanUpAnimationTmp(void);
};

NS_DT_END

#endif //__CLOCKSCENE_H__