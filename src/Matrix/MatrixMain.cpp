#include <Arduino.h>
#include "FastLED.h"
#include "Dot2D/dot2d.h"
#include "Matrix.h"
#include "MatrixMain.h"
#include "ClockScene.h"
#include "CalendarScene.h"
#include "BatteryScene.h"
#include "FFT.h"
//#include "Dot2D/math/dtMath.h"

dot2d::Director* director = nullptr;
QueueHandle_t pKeyRcvQueue = nullptr;
dot2d::Matrix *  sence0 = nullptr;
dot2d::ClockScene *  sence1 = nullptr;
uint8_t SceneIndex = Feature_Clock;
tstMainSts stMainSts = {Feature_Clock,Feature_None};
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
            TgtScene = dot2d::ClockScene::create();
            break;
        case Feature_CountDown:
            TgtScene = dot2d::BattScene::create();
            break;
        case Feature_AlarmClk:
            TgtScene = dot2d::FFT::create();
            break;
        case Feature_Clock:
            TgtScene = dot2d::ClockScene::create();
            break;
        case Feature_CalYear:
            TgtScene = dot2d::CalYearScene::create();
            break;
        case Feature_Battery:
            TgtScene = dot2d::BattScene::create();
            break;
        case Feature_Music:
            TgtScene = dot2d::FFT::create();
            break;
        case Feature_ScoreBoard:
            TgtScene = dot2d::ClockScene::create();
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
        }
        
        transition = dot2d::TransitionSlideInR::create(SCENE_TRANSITION_DURATION,GetSceneByIdx(stMainSts.enMainSceneIdx));
    }
    else if(rcvkey.Key == enKey_Right)
    {
        if(rcvkey.Type == enKey_ShortPress)
        {
            if(stMainSts.enMainSceneIdx-- <= Feature_First)
            {
                stMainSts.enMainSceneIdx = Feature_Last;
            }
        }
        transition = dot2d::TransitionSlideInL::create(SCENE_TRANSITION_DURATION,GetSceneByIdx(stMainSts.enMainSceneIdx));
    }
    return transition;
}

void vMatrixMain(void *param)
{
    tstKeyEvent RcvKey = {enKey_Nokey,enKey_NoAct};
    
    //sence0 = dot2d::Matrix::create();
    //sence1 = dot2d::ClockScene::create();
    //director->runWithScene(sence1);
    for(;;)
    {
        vTaskDelay(25);
        if(pKeyRcvQueue != nullptr)
        {
            xQueueReceive( pKeyRcvQueue,&( RcvKey ),( TickType_t ) 0 );
            if(RcvKey.Key != enKey_Nokey)
            {
                
                if(stMainSts.enEnteredFeature == Feature_None)
                {
                    if(RcvKey.Key != enKey_OK)
                    {
                        director->replaceScene(MainSceneTrans(RcvKey));
                    }
                    else
                    {
                        stMainSts.enEnteredFeature = stMainSts.enMainSceneIdx;    //enter subfunctions
                    }
                }
                else
                {
                    if(RcvKey.Key == enKey_OK && RcvKey.Type == enKey_DoubleClick)
                    {
                        stMainSts.enEnteredFeature = Feature_None;
                    }
                    else
                    {
                        dot2d::EventButton event(RcvKey.Key,(dot2d::EventButton::ButtonEventCode)RcvKey.Type);
                        auto dispatcher = dot2d::Director::getInstance()->getEventDispatcher();
                        dispatcher->dispatchEvent(&event);
                    }
                }
                RcvKey.Key = enKey_Nokey;
                RcvKey.Type = enKey_NoAct;
                // if(sceneindex == 0)
                // {         
                //     director->replaceScene(dot2d::TransitionSlideInL::create(0.5,dot2d::BattScene::create()));
                //     sceneindex = 1;
                // }
                // else
                // {           
                //     director->replaceScene(dot2d::TransitionSlideInR::create(0.5,dot2d::ClockScene::create()));
                //     sceneindex = 0;
                // }
            }
        }
        
        director->mainLoop();
    }
}

void vMatrixInit(QueueHandle_t rcvQ)
{
    //设置WS2812屏幕亮度
  FastLED.setBrightness(MATRIX_MAX_BRIGHTNESS);
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
  director = dot2d::Director::getInstance();                      //获取导演对象
  director->setDelegate(new MainDelegate());                      //设置导演代理
  director->setFramesPerSecond(20);                               //设置帧速率
  director->initDotCanvas(MATRIX_WIDTH,MATRIX_HEIGHT);            //初始化导演画布
  //director->runWithScene(dot2d::Matrix::create());
  director->runWithScene(dot2d::ClockScene::create());
  xTaskCreate(
    vMatrixMain,    // Function that should be called
    "Matrix main task",   // Name of the task (for debugging)
    4000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
}