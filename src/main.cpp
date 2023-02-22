#include <Arduino.h>
#include "FastLED.h"
#include "Dot2D/dot2d.h"
#include "Matrix/MatrixMain.h"
#include "Key/ClockKey.h"
#include "RTC/SD3078.h"

#include "AudioFileSourceFunction.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"

// VIOLA sample taken from https://ccrma.stanford.edu/~jos/pasp/Sound_Examples.html
//#include "viola.h"

float hz = 440.f;

// pre-defined function can also be used to generate the wave
float sine_wave(const float time) {
  float v = sin(TWO_PI * hz * time);  // C
  v *= fmod(time, 1.f);               // change linear
  v *= 0.5;                           // scale
  return v;
};

AudioGeneratorWAV *wav;
AudioFileSourceFunction *file;
AudioOutputI2SNoDAC *out;


ClockKey* keyHandler = nullptr;
//SD3078* SD3078Time = nullptr;
tst3078Time ClockTime = {0x00,0x17,0x93,0x07,0x12,0x02,0x23,};
int8_t Tempture = 0;
void setup() {
  //----------------开启串口通信----------------
  Serial.begin(9600);
  Serial.printf("Setup function excuted!\n");
  keyHandler = new ClockKey();
  keyHandler->Start();
  vMatrixInit();
  pinMode(26, OUTPUT);//BAT ADC EN
  digitalWrite(26,HIGH);
  pinMode(16, OUTPUT);//MIC EN
  digitalWrite(16,HIGH);
  pinMode(21, OUTPUT);//Sound SD_Mode
  digitalWrite(21,HIGH);
  if(digitalPinCanOutput(26))
  {
      Serial.printf("Pin26 can output!\n");
  }
  if(digitalPinCanOutput(16))
  {
      Serial.printf("Pin16 can output!\n");
  }
  //SD3078Time = new SD3078();
  //SD3078Time->SetTime(&ClockTime);
    // ===== create instance with length of song in [sec] =====
  file = new AudioFileSourceFunction(8.);
  //
  // you can set (sec, channels, hz, bit/sample) but you should care about
  // the trade-off between performance and the audio quality
  //
  // file = new AudioFileSourceFunction(sec, channels, hz, bit/sample);
  // channels   : default = 1
  // hz         : default = 8000 (8000, 11025, 22050, 44100, 48000, etc.)
  // bit/sample : default = 16 (8, 16, 32)

  // ===== set your sound function =====
  file->addAudioGenerators([&](const float time) {
    float v = sin(TWO_PI * hz * time);  // generate sine wave
    v *= fmod(time, 1.f);               // change linear
    v *= 0.5;                           // scale
    return v;
  });
  out = new AudioOutputI2SNoDAC();
  wav = new AudioGeneratorWAV();
  out->SetPinout(18,5,19);
  wav->begin(file, out);
}

// void vPrintTaskInfo(void)
// {
//     TaskHandle_t xHandle;
//     TaskStatus_t xTaskDetails;
//     xHandle = xTaskGetCurrentTaskHandle();

//     if(xHandle)
//     {
//         /* Use the handle to obtain further information about the task. */
//         vTaskGetInfo( /* The handle of the task being queried. */
//                       xHandle,
//                       /* The TaskStatus_t structure to complete with information
//                       on xTask. */
//                       &xTaskDetails,
//                       /* Include the stack high water mark value in the
//                       TaskStatus_t structure. */
//                       pdTRUE,
//                       /* Include the task state in the TaskStatus_t structure. */
//                       eInvalid );
//         Serial.printf(xTaskDetails.pcTaskName);Serial.printf("\n\r");
//         Serial.printf("BasePriority:%d\n\r",xTaskDetails.uxBasePriority);
//         Serial.printf("StackHighWater:%d\n\r",xTaskDetails.usStackHighWaterMark);
//     }
// }

void loop() {
  // uint32_t BatADC;
  // uint32_t LDRADC;
  // uint32_t MicroPhoneADC;
  //vTaskDelay(10 / portTICK_PERIOD_MS);
  //BatADC = analogReadMilliVolts(39);
  //LDRADC = analogReadMilliVolts(36);
  //MicroPhoneADC = analogReadMilliVolts(4);
  //Serial.printf("Battary ADC:%d\n",BatADC);
  //Serial.printf("LDR ADC:%d\n",LDRADC);
  //Serial.printf("%d\n",MicroPhoneADC);
  //vPrintTaskInfo();
  // SD3078Time->ReadTime(&ClockTime);
  // Serial.printf("Time: %x:%x:%x:%x:%x:%x:%x\n",ClockTime.u8Year,ClockTime.u8Month,ClockTime.u8Day,ClockTime.u8Week,ClockTime.u8Hour,ClockTime.u8Min,ClockTime.u8Sec);
  // SD3078Time->ReadTemp(&Tempture);
  // Serial.printf("Temp:%d\n",Tempture);
  // Serial.printf("-----Free Heap Mem : %d [%.2f%%]-----\n",
  //         ESP.getFreeHeap(),
  //         ESP.getFreeHeap()/(double)ESP.getHeapSize()*100);
  if (wav->isRunning()) {
    if (!wav->loop()) wav->stop();
  } else {
    Serial.printf("WAV done\n");
    delay(1000);
  }
}



