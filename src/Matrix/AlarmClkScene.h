#ifndef __ALARM_CLOCK_H__
#define __ALARM_CLOCK_H__

#include "Dot2D/dot2d.h"
#include "SD3078.hpp"
#include "AlarmClk.h"
#include "main.h"

NS_DT_BEGIN
#define AnimationHourTag      (1)
#define AnimationMinTag       (2)
#define AnimationPtTag        (3)
#define AnimationWeekTag      (4)
#define AnimationSWTag        (5)

typedef enum
{
	State_AlarmDis = 0,
	State_AlarmSetMin = 1,
	State_AlarmSetHour,
    State_AlarmSetWeek,
}tenAlarmState;

class AlarmClkScene : public Scene
{

protected:
    bool init() override;

public:

    STATIC_CREATE(AlarmClkScene);

};


class AlarmClkLayer : public Layer
{
protected:
	uint8_t u8CurrentAlarm = 0;
	uint8_t u8TotalAlarmNum = 0;
	FrameSprite*ClkIcon = nullptr;
    TextSprite *Hour = nullptr;
    SpriteCanvas *Hourcanvas = nullptr;
    TextSprite *Min = nullptr;
    SpriteCanvas *Mincanvas = nullptr;
	TextSprite *TimePt = nullptr;
	SpriteCanvas *TimePtcanvas = nullptr;
    CanvasSprite *Week = nullptr;
	SpriteCanvas *Weekcanvas = nullptr;
	// FrameSprite *SwitchIcon = nullptr;
	// CanvasSprite *Switch = nullptr;
	// SpriteCanvas *Switchcanvas = nullptr;
	uint8_t SettingWeekIdx = 0;
    AlarmConfig AlarmTime;
	AlarmConfig AlarmTimeSetting;
	tenAlarmState enAlarmState = State_AlarmDis;
public:
	
    STATIC_CREATE(AlarmClkLayer);

    virtual bool initLayer();

    void AlarmUpdate(float dt);

	void BtnClickHandler(int8_t, Event*);

	void BtnDoubleClickHandler(int8_t, Event*);

	void BtnLongPressStartHandler(int8_t, Event*);

	void BtnDuringLongPressHandler(int8_t, Event*);

	void StateDisHandle(int8_t,int8_t);

	void StateSetMinHandle(int8_t,int8_t);

	void StateSetHourHandle(int8_t,int8_t);

	void StateSetWeekHandle(int8_t,int8_t);

	void SwitchAlarmAnimation(AlarmConfig OldAlarm,bool boUp);

	void ClearAnimationTmp(void);

	void StateDisShow(void);

	void StateSetMinShow(void);

	void StateSetHourShow(void);

	void StateSetWeekShow(void);

	void DrawWeek(uint8_t week, tenAlarmState AlarmState,SpriteCanvas *weekcanvas,uint8_t settingidx);

	void AlarmStateMachine(int8_t,int8_t);

};



NS_DT_END

#endif //__ALARM_CLOCK_H__