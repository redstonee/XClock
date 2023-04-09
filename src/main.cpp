#include <Arduino.h>
#include "main.h"
#include "FastLED.h"
#include "Dot2D/dot2d.h"
#include "Matrix/MatrixMain.h"
#include "Key/ClockKey.h"
#include "RTC/SD3078.h"
#include "Sound/Sound.h"

ClockKey* keyHandler = nullptr;
QueueHandle_t KeyQueue = nullptr;
SD3078* SD3078Time = nullptr;
tst3078Time ClockTime = {0x00,0x17,0x93,0x07,0x12,0x02,0x23,};
tst3078Time stCurTime = {0x00,};
tstBattSts stBattsts = {0x00,};
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


uint32_t u32BattFilter(uint32_t volt)
{
    static uint32_t buffer[BAT_FILTER_CNT] = {0,};
    static uint8_t newdataindex = 0;
    static uint8_t datacnt = 0;
    uint32_t filteredvolt = 0;
    buffer[newdataindex] = volt;
    if(datacnt < BAT_FILTER_CNT)
    {
        datacnt++;
    }
    if(++newdataindex >= BAT_FILTER_CNT)
    {
        newdataindex = 0;
    }
    for(uint8_t i = 0; i < BAT_FILTER_CNT; i++)
    {
        filteredvolt += buffer[i];
    }
    filteredvolt = filteredvolt/datacnt;
    return filteredvolt;
}

tstBattSts stUpdateBattSts(void)
{
    uint8_t BatLvl = 0;
    uint32_t Volt = 0;
    tstBattSts batt_sts = {0,0};
    digitalWrite(BAT_ADC_EN_PORT,HIGH);
    vTaskDelay(10);
    Volt = u32BattFilter(analogReadMilliVolts(BAT_ADC_CH));
    if(Volt > BAT_EMPTY_VOLT)
    {
        batt_sts.BattLvl = (Volt - BAT_EMPTY_VOLT)/((BAT_FULL_VOLT-BAT_EMPTY_VOLT)/BAT_FULL_LVL);
    }
    else
    {
        batt_sts.BattLvl = 0;
    }
    digitalWrite(BAT_ADC_EN_PORT,LOW);
    batt_sts.boCharging = (LOW == digitalRead(BAT_CHARGE_STS_PORT));
    return batt_sts;
}

tst3078Time stUpdateTime(void)
{
    tst3078Time curtime = {0,};
    SD3078Time->ReadTime(&curtime);
    //Serial.printf("year %x %x %x Week %d\n",curtime.u8Year,curtime.u8Month,curtime.u8Day,curtime.u8Week);
    return curtime;
}

tst3078Time stGetCurTime(void)
{
    return stCurTime;
}

tstBattSts stGetBattSts(void)
{
    return stBattsts;
}

void setup() {
  //----------------开启串口通信----------------
  Serial.begin(115200);
  vCreateKeyQueue();
  keyHandler = new ClockKey();
  keyHandler->SetSendQueue(KeyQueue);
  keyHandler->Start();
  
  pinMode(BAT_ADC_EN_PORT, OUTPUT);//BAT ADC EN
  pinMode(BAT_CHARGE_STS_PORT, INPUT);//battery charging status
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
  SD3078Time = new SD3078();
  //SD3078Time->SetTime(&ClockTime);
  vSoundInit();
  stCurTime = stUpdateTime();
  vMatrixInit(KeyQueue);
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
  vTaskDelay(500);
  stBattsts = stUpdateBattSts();
  stCurTime = stUpdateTime();
}



