#include <Arduino.h>
#include <arduinoFFT.h>
#include "main.h"
#include "FastLED.h"
#include "Dot2D/dot2d.h"
#include "Matrix/MatrixMain.h"
#include "Key/ClockKey.h"
#include "web/web.h"
#include "RTC/SD3078.h"
#include "RTC/AlarmClk.h"
#include "Sound/Sound.h"

ClockKey* keyHandler = nullptr;
QueueHandle_t KeyQueue = nullptr;
QueueHandle_t TimeSettingQ = nullptr;
SemaphoreHandle_t xWakeReqCntSemp = nullptr;
SD3078* SD3078Time = nullptr;
tst3078Time ClockTime = {0x00,0x17,0x93,0x07,0x12,0x02,0x23,};
tst3078Time stCurTime = {0x00,};
tstBattSts stBattsts = {0x00,};
int8_t Tempture = 0;
int8_t i8SleepReqCnt = 0;
double realComponent[64];
double imagComponent[64];
arduinoFFT* fft = new arduinoFFT(realComponent,imagComponent,64,8000);

void vCreateKeyQueue(void)
{
    KeyQueue = xQueueCreate(
                         /* The number of items the queue can hold. */
                         5,
                         /* Size of each item is big enough to hold the
                         whole structure. */
                         sizeof( tstKeyEvent ) );
}

void vCreateTimeSettingQ(void)
{
    TimeSettingQ = xQueueCreate(
                         /* The number of items the queue can hold. */
                         3,
                         /* Size of each item is big enough to hold the
                         whole structure. */
                         sizeof( tst3078Time ) );
}

void vCreateSleepSemp(void)
{
    xWakeReqCntSemp = xSemaphoreCreateMutex();
    if(xWakeReqCntSemp == nullptr)
    {
        Serial.printf("create sem failed\n");
    }
}

bool RequestWakeup(void)
{
    bool res = true;
    if( xWakeReqCntSemp != NULL )
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if( xSemaphoreTake( xWakeReqCntSemp, ( TickType_t ) 10 ) == pdTRUE )
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */

            /* ... */
            i8SleepReqCnt++;
            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive( xWakeReqCntSemp );
        }
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
            Serial.printf("get sem failed\n");
            res = false;
        }
    }
    else
    {
        res = false;
    }
    return res;
}

bool boNeedWakeup(void)
{
    bool res = true;
    if( xWakeReqCntSemp != NULL )
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if( xSemaphoreTake( xWakeReqCntSemp, ( TickType_t ) 10 ) == pdTRUE )
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */
            /* ... */
            if(i8SleepReqCnt > 0)
            {
                res = true;
            }
            else
            {
                res = false;
            }
            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive( xWakeReqCntSemp );
        }
        else
        {
            Serial.printf("get sem failed\n");
        }
        
    }
    //Serial.printf("sleep cnt %d res %d\n",i8SleepReqCnt,res);
    return res;
}

bool ClearWakeupRequest(void)
{
    bool res = true;
    if( xWakeReqCntSemp != NULL )
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if( xSemaphoreTake( xWakeReqCntSemp, ( TickType_t ) 10 ) == pdTRUE )
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */

            /* ... */
            if(i8SleepReqCnt)
            {
                i8SleepReqCnt--;
            }    
            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive( xWakeReqCntSemp );
        }
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
            res = false;
            Serial.printf("get sem failed\n");
        }
    }
    else
    {
        res = false;
    }
    return res;
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
    // tst3078Time stTime = {0,0x00,0x01,0x01,0x05,0x05,0x23};
    // static uint16_t cnt = 0;
    // static uint8_t min_temp = 0;
    // static uint8_t hour_temp = 0;
    // if(++cnt>=5)
    // {
    //     cnt = 0;
    //     if(++min_temp >= 60)
    //     {
    //         min_temp = 0;
    //         if(++hour_temp >= 24)
    //         {
    //             hour_temp = 0;
    //         }            
    //     }
        
    // }
    // stTime.u8Min = ((min_temp/10)<<4) + (min_temp%10);
    // stTime.u8Hour = (((hour_temp/10)<<4) + (hour_temp%10))|0x80;
    // return stTime;
    return stCurTime;
}

QueueHandle_t pGetTimeSettingQ(void)
{
    return TimeSettingQ;
}

void vRcvTimeSettingReq(void)
{
    tst3078Time RcvTime = {0xff,};
    xQueueReceive( TimeSettingQ,&( RcvTime ),( TickType_t ) 0 );
    if(RcvTime.u8Sec != 0xff)/*new time*/
    {
        //Serial.printf("Receive new setting time!\n");
        SD3078Time->SetTime(&RcvTime);
    }
}

tstBattSts stGetBattSts(void)
{
    return stBattsts;
}

void vCheckAlarms(tst3078Time time)
{
    uint8_t AlarmNum = u8GetAlarmClkNum();
    tstAlarmClk AlarmClkTmp = {0,};
    if(AlarmNum)
    {
        for(uint8_t i = 0; i < AlarmNum; i++)
        {
            AlarmClkTmp = stGetAlarmClk(i);
            if(AlarmClkTmp.boActive)
            {
                if((AlarmClkTmp.stAlarmSts== enAlarmSts_AlarmIdle))
                {
                    if((AlarmClkTmp.u8Hour == (((time.u8Hour&0x70)>>4)*10 + (time.u8Hour&0x0f)))\
                    && (AlarmClkTmp.u8Min == (((time.u8Min&0xf0)>>4)*10 + (time.u8Min&0x0f)))\
                    && (AlarmClkTmp.u8Week & (1<<(time.u8Week==0?6:(time.u8Week-1)))))
                    {
                        Serial.printf("Alarming!\n");
                        vSetAlarmClkSts(i,enAlarmSts_Alarming);
                        RequestWakeup();
                    }
                }
                else if(AlarmClkTmp.stAlarmSts == enAlarmSts_AlarmClicked || AlarmClkTmp.stAlarmSts== enAlarmSts_Alarming)
                {
                    if((AlarmClkTmp.u8Hour != (((time.u8Hour&0x70)>>4)*10 + (time.u8Hour&0x0f)))\
                    || (AlarmClkTmp.u8Min != (((time.u8Min&0xf0)>>4)*10 + (time.u8Min&0x0f)))\
                    || ((AlarmClkTmp.u8Week & (1<<(time.u8Week==0?6:(time.u8Week-1)))) == 0))
                    {
                        Serial.printf("Alarming disabled!\n");
                        vSetAlarmClkSts(i,enAlarmSts_AlarmIdle);
                        ClearWakeupRequest();
                    }
                }
            }              
        }
    }
}



void vGotoSleep(void)
{
    Serial.printf("Go to sleep\n");
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,0);
    esp_sleep_enable_timer_wakeup(1000000);
    esp_deep_sleep_start();
}

bool boNoisy()
{
    for (int i=0; i<64; i++)
    {
        realComponent[i] = analogReadMilliVolts(4);
        imagComponent[i] = 0;
        //Serial.printf("%g ",realComponent[i]);
    }
    //Serial.printf("\n");
    fft->DCRemoval();
    fft->Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    fft->Compute(FFT_FORWARD);
    fft->ComplexToMagnitude();
    double sum = 0;
    for (int i = 0; i < 64 ; i++) /*计算每个频率段的中值和最大值*/
    {
        sum += realComponent[i];
    }
    if(sum > 12000.0)
    {
        Serial.printf("nosiy %g \n",sum);
        return true;
    }
    Serial.printf("%g \n",sum);
    return false;
}

void setup() {
  //----------------开启串口通信----------------
    Serial.begin(115200);
    pinMode(BAT_ADC_EN_PORT, OUTPUT);//BAT ADC EN
    pinMode(BAT_CHARGE_STS_PORT, INPUT);//battery charging status
    pinMode(MIC_ADC_EN_PORT, OUTPUT);//MIC EN
    digitalWrite(MIC_ADC_EN_PORT,HIGH);    
    vCreateSleepSemp();    
    SD3078Time = new SD3078();
    //SD3078Time->SetTime(&ClockTime);
    vSoundInit();
    boInitAlarmClkList();
    stCurTime = stUpdateTime();
    stBattsts = stUpdateBattSts();
    vCheckAlarms(stCurTime);
    
    if(stCurTime.u8Min == 0x00 && stCurTime.u8Sec == 0x00)
    {
        Serial.printf("SetUp wifi\n");
        SetupWifi();
    }
    vTaskDelay(50);//delay for ADC stable
    if(boNoisy() || boNeedWakeup() || ESP_SLEEP_WAKEUP_EXT0 == esp_sleep_get_wakeup_cause())
    {

        vCreateKeyQueue();
        vCreateTimeSettingQ();
        keyHandler = new ClockKey();
        keyHandler->SetSendQueue(KeyQueue);
        keyHandler->Start();
        vMatrixInit(KeyQueue);
    }
    else
    {
        vGotoSleep();
    }
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
    //boNoisy();
    vTaskDelay(500);
    stBattsts = stUpdateBattSts();
    stCurTime = stUpdateTime();
    vCheckAlarms(stCurTime);
    vRcvTimeSettingReq();
    
    if(false == boNeedWakeup())
    {
        vGotoSleep();
    }
    //uint32_t ADC1 = analogReadMilliVolts(MIC_ADC_PORT);
    // uint32_t ADC2 = analogReadMilliVolts(MIC_ADC_PORT);
    //Serial.printf("ADC %d \n", ADC1);
}



