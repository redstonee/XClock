#include <Arduino.h>
#include "FastLED.h"
#include "Dot2D/dot2d.h"
#include "Matrix.h"
#include "MatrixMain.h"
#include "ClockScene.h"
#include "CalendarScene.h"
#include "AlarmClkScene.h"
#include "BatteryScene.h"
#include "TimerScene.h"
#include "CountDownTimer.h"
#include "FFT.h"
#include "WifiInfo.h"
#include "../Sound/Sound.h"
//#include "Dot2D/math/dtMath.h"

dot2d::Director* director = nullptr;
QueueHandle_t pKeyRcvQueue = nullptr;
dot2d::Matrix *  sence0 = nullptr;
dot2d::ClockScene *  sence1 = nullptr;
uint8_t SceneIndex = Feature_Clock;
tstMainSts stMainSts = {Feature_Clock,Feature_None};
bool boAlarming = false;
TimerHandle_t FeatureEnterTO = nullptr;
TimerHandle_t SleepTO = nullptr;
//必须要实现的dot2d导演对象代理方法
class MainDelegate : public dot2d::DirectorDelegate
{

    void render()
    {
        FastLED.show();
        // 输出ESP32内存占用情况
        // Serial.printf("-----Free Heap Mem : %d [%.2f%%]-----\n",
        //         ESP.getFreeHeap(),
        //         ESP.getFreeHeap()/(double)ESP.getHeapSize()*100);
        // Serial.printf("-----Free PSRAM Mem: %d [%.2f%%]-----\n",
        //         ESP.getFreePsram(),
        //         ESP.getFreePsram()/(double)ESP.getPsramSize()*100);
    }

    size_t write(uint8_t c)
    {
      return Serial.write(c);
    }

    //返回一个RGB对象的顺序表指针，用于初始化硬件屏幕
    void initMatrix(dot2d::DTRGB *data)
    {
      FastLED.addLeds<WS2812Controller800Khz,MATRIX_LED_PIN, GRB>((CRGB* )data,MATRIX_WIDTH*MATRIX_HEIGHT);
    }

    //用于计算坐标为(x,y)的灯珠在RGB对象顺序表中的具体位置，适配不同排列方式的屏幕，在此处修改
    uint32_t dotOrder(uint16_t x,uint16_t y)
    {
      uint16_t order = x*MATRIX_HEIGHT;
      if (x%2==0)
      {
          order+=(MATRIX_HEIGHT-1-y);
      }else
      {
          order+=y;
      }
      return order;
    }
};

dot2d::Scene* GetSceneByIdx(uint8_t idx)
{
    dot2d::Scene* TgtScene = nullptr;
    switch(idx)
    {
        case Feature_Timer:
            TgtScene = dot2d::TimerScene::create();
            break;
        case Feature_CountDown:
            TgtScene = dot2d::CountDownScene::create();
            break;
        case Feature_AlarmClk:
            TgtScene = dot2d::AlarmClkScene::create();
            break;
        case Feature_Clock:
            TgtScene = dot2d::ClockScene::create();
            break;
        case Feature_CalYear:
            TgtScene = dot2d::CalYearScene::create();
            break;            
        case Feature_CalMonth:
            TgtScene = dot2d::CalMonthScene::create();
            break;
        case Feature_Battery:
            TgtScene = dot2d::BattScene::create();
            break;
        case Feature_Music:
            TgtScene = dot2d::FFT::create();
            break;
        case Feature_Wifi:
            TgtScene = dot2d::WifiInfo::create();
            break;
        default:
            TgtScene = dot2d::ClockScene::create();
            break;
    }
    return TgtScene;
}


dot2d::TransitionSlideInL* MainSceneTrans(tstKeyEvent rcvkey)
{
    dot2d::TransitionSlideInL* transition = nullptr;
    if(rcvkey.Key == enKey_Left)
    {
        if(rcvkey.Type == enKey_ShortPress)
        {
            if(stMainSts.enMainSceneIdx++ >= Feature_Last)
            {
                stMainSts.enMainSceneIdx = Feature_First;
            }
            transition = dot2d::TransitionSlideInR::create(SCENE_TRANSITION_DURATION,GetSceneByIdx(stMainSts.enMainSceneIdx));
        }
        
        
    }
    else if(rcvkey.Key == enKey_Right)
    {
        if(rcvkey.Type == enKey_ShortPress)
        {
            if(stMainSts.enMainSceneIdx-- <= Feature_First)
            {
                stMainSts.enMainSceneIdx = Feature_Last;
            }
            transition = dot2d::TransitionSlideInL::create(SCENE_TRANSITION_DURATION,GetSceneByIdx(stMainSts.enMainSceneIdx));
        }
        
    }
    return transition;
}

uint32_t u32ADCDataFilter(uint32_t volt)
{
    static uint32_t buffer[ADC_FILTER_CNT] = {0,};
    static uint8_t newdataindex = 0;
    static uint8_t datacnt = 0;
    uint32_t filteredvolt = 0;
    buffer[newdataindex] = volt;
    if(datacnt < ADC_FILTER_CNT)
    {
        datacnt++;
    }
    if(++newdataindex >= ADC_FILTER_CNT)
    {
        newdataindex = 0;
    }
    for(uint8_t i = 0; i < ADC_FILTER_CNT; i++)
    {
        filteredvolt += buffer[i];
    }
    filteredvolt = filteredvolt/datacnt;
    //Serial.printf("in:%d out:%d\n",volt,filteredvolt);
    return filteredvolt;
}

void vBrightessTask(void)
{
    uint32_t LDRADC;
    static uint8_t u8OldBrightness = MATRIX_BRIGHTNESS_BASE;
    uint8_t u8NewBrightness = MATRIX_BRIGHTNESS_BASE;
    LDRADC = analogReadMilliVolts(MATRIX_LDR_ADC_CH);
    LDRADC = u32ADCDataFilter(LDRADC);
    u8NewBrightness = (MATRIX_LDR_ADC_MAX - LDRADC) / MATRIX_LDR2LIGHT_STEP + MATRIX_BRIGHTNESS_BASE;
    if(u8OldBrightness !=u8NewBrightness)
    {
        FastLED.setBrightness(u8NewBrightness);
        Serial.printf("new brightness:%d\n",u8NewBrightness);
    }
    u8OldBrightness = u8NewBrightness;
}

void vAlarmTask(void)
{
    uint8_t AlarmNum = u8GetAlarmClkNum();
    tstAlarmClk AlarmClkTmp = {0,};
    if(AlarmNum)
    {
        for(uint8_t i = 0; i < AlarmNum; i++)
        {
            AlarmClkTmp = stGetAlarmClk(i);
            if(AlarmClkTmp.stAlarmSts == enAlarmSts_Alarming)
            {
                boAlarming = true;
                if(enSndID_None == enGetCurSndID())
                {
                    boReqSound(enSndID_Alarm1,1);
                    if(Feature_Clock != stMainSts.enMainSceneIdx)
                    {
                        dot2d::TransitionSlideInL* transition = dot2d::TransitionSlideInR::create(SCENE_TRANSITION_DURATION,GetSceneByIdx(Feature_Clock));
                        if(nullptr != transition)
                        {
                            director->replaceScene(transition);
                        } 
                    }                    
                }
            }         
        }
    }    
}

void vAlarmClick(void)
{
    uint8_t AlarmNum = u8GetAlarmClkNum();
    tstAlarmClk AlarmClkTmp = {0,};
    if(AlarmNum)
    {
        for(uint8_t i = 0; i < AlarmNum; i++)
        {
            AlarmClkTmp = stGetAlarmClk(i);
            if(AlarmClkTmp.stAlarmSts == enAlarmSts_Alarming)
            {
                boAlarming = false;
                vStopSound();
                vSetAlarmClkSts(i,enAlarmSts_AlarmClicked);
            }         
        }
    } 
}

void vMatrixMain(void *param)
{
    tstKeyEvent RcvKey = {enKey_Nokey,enKey_NoAct};
    for(;;)
    {
        vTaskDelay(5);
        vAlarmTask();
        vBrightessTask();
        if(pKeyRcvQueue != nullptr)
        {
            xQueueReceive( pKeyRcvQueue,&( RcvKey ),( TickType_t ) 0 );
            if(RcvKey.Key != enKey_Nokey)
            {
                if( xTimerIsTimerActive(SleepTO))
                {
                    xTimerReset(SleepTO,10);
                } 
                if(8 ==director->getRunningScene()->getPositionY())
                {
                    dot2d::MoveTo* MoveUp = dot2d::MoveTo::create(0.5,dot2d::Vec2(0,0));
                    director->getRunningScene()->runAction(MoveUp);    
                }
                xTimerReset(SleepTO,10);
                if(boAlarming)
                {
                    vAlarmClick();
                }
                else
                {
                    if(stMainSts.enEnteredFeature == Feature_None)
                    {
                        if(RcvKey.Key != enKey_OK)
                        {
                            dot2d::TransitionSlideInL* transition = MainSceneTrans(RcvKey);
                            if(nullptr != transition)
                            {
                                director->replaceScene(transition);
                                if(Feature_Clock != stMainSts.enMainSceneIdx \
                                && Feature_Music != stMainSts.enMainSceneIdx \
                                && Feature_Wifi != stMainSts.enMainSceneIdx \
                                && false == dot2d::boIsTimerCounterActive() \
                                && false == dot2d::boIsCountDownTimerActive())
                                {                                    
                                    if(nullptr != FeatureEnterTO)   /*Start a timeout timer to exit the feature*/
                                    {
                                        if(xTimerIsTimerActive(FeatureEnterTO))
                                        {
                                            xTimerReset(FeatureEnterTO,10);
                                        }
                                    }
                                }
                                else
                                {
                                    if(nullptr != FeatureEnterTO)   /*Start a timeout timer to exit the feature*/
                                    {
                                        if(xTimerIsTimerActive(FeatureEnterTO))
                                        {
                                            xTimerStop(FeatureEnterTO,10);
                                        }                                        
                                    }
                                }
                            }                        
                        }
                        else
                        {
                            stMainSts.enEnteredFeature = stMainSts.enMainSceneIdx;    //enter subfunctions
                            //if(Feature_Timer != stMainSts.enEnteredFeature && Feature_CountDown != stMainSts.enEnteredFeature)
                            {
                                if(nullptr != FeatureEnterTO)   /*Start a timeout timer to exit the feature*/
                                {
                                    xTimerStart(FeatureEnterTO,10);
                                }
                            }
                            dot2d::EventButton event(RcvKey.Key,(dot2d::EventButton::ButtonEventCode)RcvKey.Type);
                            auto dispatcher = dot2d::Director::getInstance()->getEventDispatcher();
                            dispatcher->dispatchEvent(&event);                        
                        }
                    }
                    else
                    {
                        if(RcvKey.Key == enKey_OK && RcvKey.Type == enKey_DoubleClick)
                        {
                            stMainSts.enEnteredFeature = Feature_None;
                            if(nullptr != FeatureEnterTO) /*Stop the timeout timer*/
                            {
                                if( xTimerIsTimerActive(FeatureEnterTO))
                                {
                                    xTimerStop(FeatureEnterTO,10);
                                }
                            }
                        }
                        else
                        {
                            if(nullptr != FeatureEnterTO)/*Restart the timeout timer if any key pressed*/
                            {
                                if( xTimerIsTimerActive(FeatureEnterTO))
                                {
                                    xTimerReset(FeatureEnterTO,10);
                                }                            
                            }
                            dot2d::EventButton event(RcvKey.Key,(dot2d::EventButton::ButtonEventCode)RcvKey.Type);
                            auto dispatcher = dot2d::Director::getInstance()->getEventDispatcher();
                            dispatcher->dispatchEvent(&event);
                        }
                    }
                }
                
                RcvKey.Key = enKey_Nokey;
                RcvKey.Type = enKey_NoAct;
            }
        }
        
        director->mainLoop();
    }
}

void vFeatureTOCb(TimerHandle_t xTimer)
{
    
    if((Feature_Timer != stMainSts.enEnteredFeature || false == dot2d::boIsTimerCounterActive())\
        && (Feature_CountDown != stMainSts.enEnteredFeature || false == dot2d::boIsCountDownTimerActive()) \
        && Feature_Music != stMainSts.enEnteredFeature \
        && Feature_Wifi!= stMainSts.enMainSceneIdx)
    {
        stMainSts.enEnteredFeature = Feature_None;
        stMainSts.enMainSceneIdx = Feature_Clock;
        dot2d::TransitionSlideInL* transition = dot2d::TransitionSlideInR::create(SCENE_TRANSITION_DURATION,GetSceneByIdx(Feature_Clock));
        if(nullptr != transition)
        {
            director->replaceScene(transition);
        } 
    }
    else
    {
        stMainSts.enEnteredFeature = Feature_None;
    }
    
}

void vOffSeqFinishCb()
{
    ClearWakeupRequest(true);
}

void vSleepTOCb(TimerHandle_t xTimer)
{
    Serial.printf("Sleep timeout\n");
    if(stMainSts.enEnteredFeature == Feature_None \
        && stMainSts.enMainSceneIdx != Feature_CountDown \
        && stMainSts.enMainSceneIdx != Feature_Timer \
        && stMainSts.enMainSceneIdx != Feature_Music \
        && Feature_Wifi!= stMainSts.enMainSceneIdx)
    {
        dot2d::MoveTo* MoveOff = dot2d::MoveTo::create(0.5,dot2d::Vec2(0,8));
        dot2d::Sequence *OffSeq = dot2d::Sequence::createWithTwoActions(MoveOff, dot2d::CallFunc::create(vOffSeqFinishCb));
        director->getRunningScene()->runAction(OffSeq);        
    }
    else
    {
        xTimerReset(SleepTO,10);
    }    
}

void vResetSleepTimer()
{
    if( xTimerIsTimerActive(SleepTO))
    {
        xTimerReset(SleepTO,10);
    } 
}

void vMatrixInit(QueueHandle_t rcvQ)
{
    //设置WS2812屏幕亮度
  FastLED.setBrightness(MATRIX_BRIGHTNESS_BASE);
  pKeyRcvQueue = rcvQ;
  uint8_t sceneindex = Feature_Clock;
  stMainSts.enMainSceneIdx = Feature_Clock;
  stMainSts.enEnteredFeature = Feature_None;
  //----------------Init scene list------------------------
  //sence0 = dot2d::Matrix::create();
  //Serial.printf("SceneMtri %x\n",&sence0);
  //sence1 = dot2d::ClockScene::create();
  //Serial.printf("SceneClk %x\n",&sence1);
  //----------------初始化Dot2d引擎及渲染画布----------------
  RequestWakeup(true);
  director = dot2d::Director::getInstance();                      //获取导演对象
  director->setDelegate(new MainDelegate());                      //设置导演代理
  director->setFramesPerSecond(30);                               //设置帧速率
  director->initDotCanvas(MATRIX_WIDTH,MATRIX_HEIGHT);            //初始化导演画布
  //director->runWithScene(dot2d::Matrix::create());
  director->runWithScene(dot2d::ClockScene::create());
  FeatureEnterTO = xTimerCreate
                   ( /* Just a text name, not used by the RTOS
                     kernel. */
                     "FeatureTimer",
                     /* The timer period in ticks, must be
                     greater than 0. */
                     FeatureTimeout,
                     /* The timers will auto-reload themselves
                     when they expire. */
                     pdFALSE,
                     /* The ID is used to store a count of the
                     number of times the timer has expired, which
                     is initialised to 0. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when
                     it expires. */
                     vFeatureTOCb
                   );
    SleepTO = xTimerCreate
                   ( /* Just a text name, not used by the RTOS
                     kernel. */
                     "FeatureTimer",
                     /* The timer period in ticks, must be
                     greater than 0. */
                     SleepTimeout,
                     /* The timers will auto-reload themselves
                     when they expire. */
                     pdFALSE,
                     /* The ID is used to store a count of the
                     number of times the timer has expired, which
                     is initialised to 0. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when
                     it expires. */
                     vSleepTOCb
                   );
  xTimerStart(SleepTO,10);
  xTaskCreatePinnedToCore(
    vMatrixMain,    // Function that should be called
    "Matrix main task",   // Name of the task (for debugging)
    4000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    4,               // Task priority
    NULL,             // Task handle
    1                 //core, fastled seems must pin to core 1 and wifi pin to core0, otherwise the fastled will have flicker
  );
}