#ifndef __MATRIX_MAIN__
#define __MATRIX_MAIN__
#include "../Key/ClockKey.h"
//LED矩阵宽
#define MATRIX_WIDTH          32
//LED矩阵高
#define MATRIX_HEIGHT         8
//点阵屏驱动引脚
#define MATRIX_LED_PIN        17
//屏幕最大亮度
#define MATRIX_MAX_BRIGHTNESS 5
//换场时间
#define SCENE_TRANSITION_DURATION  (0.5f)

#define FeatureTimeout  (portTICK_PERIOD_MS*1000*30)

#define SleepTimeout    (portTICK_PERIOD_MS*1000*60*3)//5 minutes

typedef enum
{
    Feature_None = 0,
    Feature_First,
    Feature_Timer = Feature_First,
    Feature_CountDown,
    Feature_AlarmClk,
    Feature_Clock,
    Feature_CalYear,
    Feature_CalMonth,
    Feature_Battery,
    Feature_Music,
    Feature_Wifi,
    Feature_Last = Feature_Wifi,
}tenFeatureID;



typedef struct
{
    uint8_t enMainSceneIdx;
    uint8_t enEnteredFeature;
}tstMainSts;


void vMatrixInit(QueueHandle_t);
void vResetSleepTimer();
#endif