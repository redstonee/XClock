#include <Arduino.h>
#include "FastLED.h"
#include "Dot2D/dot2d.h"
#include "Matrix/MatrixMain.h"
#include "Key/ClockKey.h"
#include "RTC/SD3078.h"
#include "Sound/Sound.h"


ClockKey* keyHandler = nullptr;
QueueHandle_t KeyQueue = NULL;
//SD3078* SD3078Time = nullptr;
tst3078Time ClockTime = {0x00,0x17,0x93,0x07,0x12,0x02,0x23,};
int8_t Tempture = 0;

void vCreateKeyQueue(void)
{
    KeyQueue = xQueueCreate(
                         /* The number of items the queue can hold. */
                         5,
                         /* Size of each item is big enough to hold the
                         whole structure. */
                         sizeof( tstKeyEvent ) );
}

void setup() {
  //----------------开启串口通信----------------
  Serial.begin(9600);
  Serial.printf("Setup function excuted!\n");
  vCreateKeyQueue();
  keyHandler = new ClockKey();
  keyHandler->SetSendQueue(KeyQueue);
  keyHandler->Start();
  vMatrixInit();
  pinMode(26, OUTPUT);//BAT ADC EN
  digitalWrite(26,HIGH);
  pinMode(16, OUTPUT);//MIC EN
  digitalWrite(16,HIGH);
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
  vSoundInit();
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
}



