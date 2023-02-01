#include <Arduino.h>
#include "FastLED.h"
#include "Dot2D/dot2d.h"
#include "Matrix.h"

//LED矩阵宽
#define MATRIX_WIDTH          32
//LED矩阵高
#define MATRIX_HEIGHT         8
//点阵屏驱动引脚
#define MATRIX_LED_PIN        18
//屏幕最大亮度
#define MATRIX_MAX_BRIGHTNESS 48

dot2d::Director* director = nullptr;


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


void setup() {
  //----------------开启串口通信----------------
  Serial.begin(115200);

  //设置WS2812屏幕亮度
  FastLED.setBrightness(MATRIX_MAX_BRIGHTNESS);


  //----------------初始化Dot2d引擎及渲染画布----------------
  director = dot2d::Director::getInstance();                      //获取导演对象
  director->setDelegate(new MainDelegate());                      //设置导演代理
  director->setFramesPerSecond(30);                               //设置帧速率
  director->initDotCanvas(MATRIX_WIDTH,MATRIX_HEIGHT);            //初始化导演画布


  director->runWithScene(dot2d::Matrix::create());

  
}

void loop() {
  vTaskDelay(10);
  director->mainLoop();
}