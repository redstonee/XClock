#ifndef __CLOCKSCENE_H__
#define __CLOCKSCENE_H__

#include "Dot2D/dot2d.h"
#include "../RTC/SD3078.h"
#include "FastLED.h"
NS_DT_BEGIN

#define Hour1AniTmpTag  (1)
#define Hour2AniTmpTag  (2)
#define Min1AniTmpTag   (3)
#define Min2AniTmpTag   (4)
#define AlarmIconTag    (5)

typedef enum
{
	State_TimeDis = 0,
	State_TimeSetMin = 1,
	State_TimeSetHour,
	State_TimeSetWeek,
}tenTimeState;

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
	CanvasSprite *Week = nullptr;
	SpriteCanvas *Weekcanvas = nullptr;
	FrameSprite *Weather = nullptr;
	CanvasSprite *AlarmIcon = nullptr;
	SpriteCanvas *AlarmIconCanvas = nullptr;
    tst3078Time ClockTime;
	tst3078Time ClockTimeSetting;
	QueueHandle_t TimeSettingQ;
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

	void BtnClickHandler(int8_t, Event*);

	void BtnLongPressStartHandler(int8_t, Event*);

	void BtnDuringLongPressHandler(int8_t, Event*);

	void BtnDuringLongPressStopHandler(int8_t, Event*);

	void TimeStateMachine(int8_t,int8_t);

	void UpdateColor(int8_t,int8_t);

	void SendSettingTime(tst3078Time*);

	void DrawWeek(uint8_t);

	void StateTimeDisShow(void);

	void StateSetMinShow(void);

	void StateSetHourShow(void);

	void StateSetWeekShow(void);

	void DigitalSwitchAnimation(TextSprite* OldDigitalSprt,uint8_t OldDigital, uint8_t NewDigital, int tmpTag);

	void CleanUpAnimationTmp(void);
};

NS_DT_END

#endif //__CLOCKSCENE_H__