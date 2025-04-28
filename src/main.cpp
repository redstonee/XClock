#include <Arduino.h>
#include <arduinoFFT.h>
#include <WiFi.h>
#include "config.h"
#include "FastLED.h"
#include "Dot2D/dot2d.h"
#include "Matrix/MatrixMain.h"
#include "ClockKey.h"
#include "web.h"
#include "SD3078.hpp"
#include "AlarmClk.h"
#include "Sound.h"
#include "BattMon.h"

ClockKey *keyHandler = nullptr;
QueueHandle_t KeyQueue = nullptr;
QueueHandle_t TimeSettingQ = nullptr;
SemaphoreHandle_t xTimeSemp = nullptr;
SemaphoreHandle_t xWakeReqCntSemp = nullptr;
static SD3078 RTC(RTC_SDA_PIN, RTC_SCL_PIN, RTC_ADDR);

tst3078Time ClockTime = {
    0x00,
    0x17,
    0x93,
    0x07,
    0x12,
    0x02,
    0x23,
};
tst3078Time stCurTime = {
    0x00,
};
int8_t Tempture = 0;
int8_t i8SleepReqCnt = 0;
int8_t i8SleepReqHMICnt = 0;
double realComponent[64];
double imagComponent[64];
arduinoFFT *fft = new arduinoFFT(realComponent, imagComponent, 64, 8000);

void vCreateKeyQueue(void)
{
    KeyQueue = xQueueCreate(
        /* The number of items the queue can hold. */
        5,
        /* Size of each item is big enough to hold the
        whole structure. */
        sizeof(tstKeyEvent));
}

void vCreateTimeSettingQ(void)
{
    TimeSettingQ = xQueueCreate(
        /* The number of items the queue can hold. */
        3,
        /* Size of each item is big enough to hold the
        whole structure. */
        sizeof(tst3078Time));
}

void vCreateTimeSemp(void)
{
    xTimeSemp = xSemaphoreCreateMutex();
    if (xTimeSemp == nullptr)
    {
        Serial.printf("create time sem failed\n\r");
    }
}

void vCreateSleepSemp(void)
{
    xWakeReqCntSemp = xSemaphoreCreateMutex();
    if (xWakeReqCntSemp == nullptr)
    {
        Serial.printf("create sem failed\n\r");
    }
}

bool RequestWakeup(bool boHMIDis)
{
    bool res = true;
    if (xWakeReqCntSemp != NULL)
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if (xSemaphoreTake(xWakeReqCntSemp, (TickType_t)10) == pdTRUE)
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */

            /* ... */
            if (boHMIDis)
            {
                i8SleepReqHMICnt++;
            }
            else
            {
                i8SleepReqCnt++;
            }
            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive(xWakeReqCntSemp);
        }
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
            Serial.printf("get sem failed\n\r");
            res = false;
        }
    }
    else
    {
        res = false;
    }
    return res;
}

bool boNeedWakeup(bool boHMIDis)
{
    bool res = true;
    if (xWakeReqCntSemp != NULL)
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if (xSemaphoreTake(xWakeReqCntSemp, (TickType_t)10) == pdTRUE)
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */
            /* ... */
            if (boHMIDis)
            {
                if (i8SleepReqHMICnt > 0)
                {
                    res = true;
                }
                else
                {
                    res = false;
                }
            }
            else
            {
                if (i8SleepReqCnt > 0)
                {
                    res = true;
                }
                else
                {
                    res = false;
                }
            }

            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive(xWakeReqCntSemp);
        }
        else
        {
            Serial.printf("get sem failed\n\r");
        }
    }
    // Serial.printf("sleep cnt %d res %d\n",i8SleepReqCnt,res);
    return res;
}

bool ClearWakeupRequest(bool boHMIDis)
{
    bool res = true;
    if (xWakeReqCntSemp != NULL)
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if (xSemaphoreTake(xWakeReqCntSemp, (TickType_t)10) == pdTRUE)
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */

            /* ... */
            if (boHMIDis)
            {
                if (i8SleepReqHMICnt)
                {
                    i8SleepReqHMICnt--;
                }
            }
            else
            {
                if (i8SleepReqCnt)
                {
                    i8SleepReqCnt--;
                }
            }
            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive(xWakeReqCntSemp);
        }
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
            res = false;
            Serial.printf("get sem failed\n\r");
        }
    }
    else
    {
        res = false;
    }
    return res;
}

tst3078Time stUpdateTime(void)
{
    tst3078Time curtime = {
        0,
    };
    RTC.ReadTime(&curtime);
    // Serial.printf("%x:%x:%x Week %d\n",curtime.u8Hour,curtime.u8Min,curtime.u8Sec,curtime.u8Week);
    return curtime;
}

void vSetCurTime(tst3078Time CurTime)
{
    if (xTimeSemp != NULL)
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if (xSemaphoreTake(xTimeSemp, (TickType_t)10) == pdTRUE)
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */

            /* ... */
            stCurTime = CurTime;
            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive(xTimeSemp);
        }
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
            Serial.printf("get time sem failed\n\r");
        }
    }
}

tst3078Time stGetCurTime(void)
{
    tst3078Time time_tmp;
    if (xTimeSemp != NULL)
    {
        /* See if we can obtain the semaphore.  If the semaphore is not
        available wait 10 ticks to see if it becomes free. */
        if (xSemaphoreTake(xTimeSemp, (TickType_t)10) == pdTRUE)
        {
            /* We were able to obtain the semaphore and can now access the
            shared resource. */

            /* ... */
            time_tmp = stCurTime;
            /* We have finished accessing the shared resource.  Release the
            semaphore. */
            xSemaphoreGive(xTimeSemp);
        }
        else
        {
            /* We could not obtain the semaphore and can therefore not access
            the shared resource safely. */
            Serial.printf("get time sem failed\n\r");
        }
    }
    return time_tmp;
}

QueueHandle_t pGetTimeSettingQ(void)
{
    return TimeSettingQ;
}

void vRcvTimeSettingReq(void)
{
    tst3078Time RcvTime = {
        0xff,
    };
    xQueueReceive(TimeSettingQ, &(RcvTime), (TickType_t)0);
    if (RcvTime.u8Sec != 0xff) /*new time*/
    {
        Serial.printf("Receive new setting time!\n\r");
        RTC.SetTime(&RcvTime);
    }
}

void vSetTimeDirect(tst3078Time *time)
{
    RTC.SetTime(time);
}

void vCheckAlarms(tst3078Time time)
{
    uint8_t AlarmNum = u8GetAlarmClkNum();
    tstAlarmClk AlarmClkTmp = {
        0,
    };
    if (AlarmNum)
    {
        for (uint8_t i = 0; i < AlarmNum; i++)
        {
            AlarmClkTmp = stGetAlarmClk(i);
            if (AlarmClkTmp.boActive)
            {
                if ((AlarmClkTmp.stAlarmSts == enAlarmSts_AlarmIdle))
                {
                    if ((AlarmClkTmp.u8Hour == (((time.u8Hour & 0x70) >> 4) * 10 + (time.u8Hour & 0x0f))) && (AlarmClkTmp.u8Min == (((time.u8Min & 0xf0) >> 4) * 10 + (time.u8Min & 0x0f))) && (AlarmClkTmp.u8Week & (1 << (time.u8Week == 0 ? 6 : (time.u8Week - 1)))))
                    {
                        Serial.printf("Alarming!\n\r");
                        vSetAlarmClkSts(i, enAlarmSts_Alarming);
                        RequestWakeup(true);
                    }
                }
                else if (AlarmClkTmp.stAlarmSts == enAlarmSts_AlarmClicked || AlarmClkTmp.stAlarmSts == enAlarmSts_Alarming)
                {
                    if ((AlarmClkTmp.u8Hour != (((time.u8Hour & 0x70) >> 4) * 10 + (time.u8Hour & 0x0f))) || (AlarmClkTmp.u8Min != (((time.u8Min & 0xf0) >> 4) * 10 + (time.u8Min & 0x0f))) || ((AlarmClkTmp.u8Week & (1 << (time.u8Week == 0 ? 6 : (time.u8Week - 1)))) == 0))
                    {
                        Serial.printf("Alarming disabled!\n\r");
                        vSetAlarmClkSts(i, enAlarmSts_AlarmIdle);
                        ClearWakeupRequest(true);
                    }
                }
            }
        }
    }
}

void vGotoSleep(void)
{
    Serial.printf("Go to sleep\n\r");
    digitalWrite(MIC_EN_PIN, LOW);
    digitalWrite(LED_EN_PIN, LOW);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0);
    esp_sleep_enable_timer_wakeup(SLEEP_TIME);
    esp_deep_sleep_start();
}

bool boNoisy()
{
    digitalWrite(MIC_EN_PIN, HIGH);
    for (int i = 0; i < 64; i++)
    {
        realComponent[i] = analogReadMilliVolts(MIC_SIG_PIN);
        imagComponent[i] = 0;
        // Serial.printf("%g ",realComponent[i]);
    }
    // Serial.printf("\n");
    fft->DCRemoval();
    fft->Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    fft->Compute(FFT_FORWARD);
    fft->ComplexToMagnitude();
    double sum = 0;
    for (int i = 0; i < 64; i++) /*计算每个频率段的中值和最大值*/
    {
        sum += realComponent[i];
    }
    if (sum > NOISE_THRESH)
    {
        Serial.printf("nosiy %g\n\r", sum);
        return true;
    }
    Serial.printf("%g\n\r", sum);
    return false;
}

void setup()
{
    //----------------开启串口通信----------------
    Serial.begin(115200);
    BattMon::begin();
    pinMode(MIC_EN_PIN, OUTPUT); // MIC EN
    digitalWrite(MIC_EN_PIN, HIGH);
    vCreateTimeSemp();
    vCreateSleepSemp();
    // RTC->SetTime(&ClockTime);
    vSoundInit();
    boInitAlarmClkList();
    vSetCurTime(stUpdateTime());
    vCheckAlarms(stCurTime);

    if ((stCurTime.u8Min == 0x00 || stCurTime.u8Min == 0x30) && (stCurTime.u8Sec == 0x00 || stCurTime.u8Sec == 0x0A || stCurTime.u8Sec == 0x17))
    {
        Serial.printf("SetUp wifi1\n\r");
        SetupWifi();
    }
    vTaskDelay(50); // delay for ADC stable
    if (boNoisy() || boNeedWakeup(true) || ESP_SLEEP_WAKEUP_EXT0 == esp_sleep_get_wakeup_cause())
    {
        // vWifiInit();
        vCreateKeyQueue();
        vCreateTimeSettingQ();
        keyHandler = new ClockKey();
        keyHandler->SetSendQueue(KeyQueue);
        keyHandler->Start();
        pinMode(LED_EN_PIN, OUTPUT); // MIC EN
        digitalWrite(LED_EN_PIN, HIGH);
        vMatrixInit(KeyQueue);
    }
    if ((false == boNeedWakeup(true)) && (false == boNeedWakeup(false)))
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

void loop()
{
    // uint32_t BatADC;
    // uint32_t LDRADC;
    // uint32_t MicroPhoneADC;
    // vTaskDelay(10 / portTICK_PERIOD_MS);
    // BatADC = analogReadMilliVolts(39);
    // LDRADC = analogReadMilliVolts(36);
    // MicroPhoneADC = analogReadMilliVolts(4);
    // Serial.printf("Battary ADC:%d\n",BatADC);
    // Serial.printf("LDR ADC:%d\n",LDRADC);
    // Serial.printf("%d\n",MicroPhoneADC);
    // vPrintTaskInfo();
    // RTC->ReadTime(&ClockTime);
    // Serial.printf("Time: %x:%x:%x:%x:%x:%x:%x\n",ClockTime.u8Year,ClockTime.u8Month,ClockTime.u8Day,ClockTime.u8Week,ClockTime.u8Hour,ClockTime.u8Min,ClockTime.u8Sec);
    // RTC->ReadTemp(&Tempture);
    // Serial.printf("Temp:%d\n",Tempture);
    // Serial.printf("-----Free Heap Mem : %d [%.2f%%]-----\n",
    //         ESP.getFreeHeap(),
    //         ESP.getFreeHeap()/(double)ESP.getHeapSize()*100);
    // boNoisy();
    vTaskDelay(1000);
    vSetCurTime(stUpdateTime());
    vCheckAlarms(stCurTime);
    vRcvTimeSettingReq();

    if ((false == boNeedWakeup(true)) && (false == boNeedWakeup(false)))
    {
        vGotoSleep();
    }
    if ((stCurTime.u8Min == 0x00 || stCurTime.u8Min == 0x30) && (stCurTime.u8Sec == 0x00 || stCurTime.u8Sec == 0x0A || stCurTime.u8Sec == 0x17))
    {
        Serial.printf("SetUp wifi2\n\r");
        SetupWifi();
    }
    if (boNoisy())
    {
        vResetSleepTimer();
    }
    // uint32_t ADC1 = analogReadMilliVolts(MIC_ADC_PORT);
    //  uint32_t ADC2 = analogReadMilliVolts(MIC_ADC_PORT);
    // Serial.printf("ADC %d \n", ADC1);
}
