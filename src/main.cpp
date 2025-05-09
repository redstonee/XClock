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
SemaphoreHandle_t xWakeReqCntSemp = nullptr;
static SD3078 RTC(RTC_SDA_PIN, RTC_SCL_PIN, RTC_ADDR);

static const char *TAG = "Main";

int8_t Tempture = 0;
int8_t i8SleepReqCnt = 0;
int8_t i8SleepReqHMICnt = 0;
double realComponent[64];
double imagComponent[64]{0};
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
        sizeof(tm));
}

void vCreateSleepSemp(void)
{
    xWakeReqCntSemp = xSemaphoreCreateMutex();
    if (xWakeReqCntSemp == nullptr)
    {
        ESP_LOGE(TAG, "create sem failed\n\r");
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
            ESP_LOGE(TAG, "get sem failed\n\r");
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
        if (xSemaphoreTake(xWakeReqCntSemp, 10))
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
            ESP_LOGE(TAG, "get sem failed\n\r");
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
            ESP_LOGE(TAG, "get sem failed\n\r");
        }
    }
    else
    {
        res = false;
    }
    return res;
}

QueueHandle_t pGetTimeSettingQ(void)
{
    return TimeSettingQ;
}

void vRcvTimeSettingReq(void)
{
    tm RcvTime = {
        .tm_sec = 0xff,
    };

    xQueueReceive(TimeSettingQ, &(RcvTime), 0);
    if (RcvTime.tm_sec != 0xff) /*new time*/
    {
        ESP_LOGE(TAG, "Receive new setting time!\n\r");
        RTC.setTime(RcvTime);
    }
}

void vCheckAlarms(tm currentTime)
{
    uint8_t AlarmNum = u8GetAlarmClkNum();
    AlarmConfig AlarmClkTmp = {
        0,
    };
    if (AlarmNum)
    {
        for (uint8_t i = 0; i < AlarmNum; i++)
        {
            AlarmClkTmp = stGetAlarmClk(i);
            if (AlarmClkTmp.isActive)
            {
                if ((AlarmClkTmp.alarmStatus == Alarm_Idle))
                {
                    if ((AlarmClkTmp.hour == currentTime.tm_hour) && (AlarmClkTmp.minute == currentTime.tm_min) &&
                        (AlarmClkTmp.week & (1 << (currentTime.tm_wday == 0 ? 6 : (currentTime.tm_wday - 1)))))
                    {
                        ESP_LOGI(TAG, "Alarming!\n\r");
                        vSetAlarmClkSts(i, Alarm_GoOff);
                        RequestWakeup(true);
                    }
                }
                else if (AlarmClkTmp.alarmStatus == Alarm_Clicked || AlarmClkTmp.alarmStatus == Alarm_GoOff)
                {
                    if ((AlarmClkTmp.hour != currentTime.tm_hour) || (AlarmClkTmp.minute != currentTime.tm_min) ||
                        ((AlarmClkTmp.week & (1 << (currentTime.tm_wday == 0 ? 6 : (currentTime.tm_wday - 1)))) == 0))
                    {
                        ESP_LOGI(TAG, "Alarming disabled!\n\r");
                        vSetAlarmClkSts(i, Alarm_Idle);
                        ClearWakeupRequest(true);
                    }
                }
            }
        }
    }
}

void vGotoSleep(void)
{
    ESP_LOGI(TAG, "Go to sleep\n\r");
    digitalWrite(MIC_EN_PIN, LOW);
    digitalWrite(LED_EN_PIN, LOW);
    esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(BUTTON_OK_PIN), 0);
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
        ESP_LOGI(TAG, "nosiy %g\n\r", sum);
        return true;
    }
    ESP_LOGI(TAG, "%g\n\r", sum);
    return false;
}

void setup()
{
    analogSetPinAttenuation(MIC_SIG_PIN, ADC_2_5db);
    pinMode(MIC_EN_PIN, OUTPUT);
    digitalWrite(MIC_EN_PIN, HIGH);

    BattMon::begin();
    if (!RTC.begin())
    {
        ESP_LOGE(TAG, "Failed to initialize RTC\n\r");
        while (1)
        {
            delay(1000);
        }
    }
    ESP_LOGI(TAG, "RTC initialized\n\r");

    vCreateSleepSemp();
    if (!vSoundInit())
    {
        ESP_LOGE(TAG, "Sound Init failed\n\r");
        while (1)
        {
            delay(1000);
        }
    }
    ESP_LOGI(TAG, "Sound Init success\n\r");

    boInitAlarmClkList();

    setenv("TZ", "CST-8", 1);
    tzset();

    tm currentTime;
    if (!RTC.getTime(&currentTime))
    {
        ESP_LOGE(TAG, "Failed to read RTC time\n\r");
        while (1)
        {
            delay(1000);
        }
    }
    // Set the system time to RTC time
    // auto t = mktime(&currentTime);
    // struct timeval now = {.tv_sec = t, .tv_usec = 0};
    // settimeofday(&now, NULL);

    // TODO: Check battery level
    vCheckAlarms(currentTime);

    ESP_LOGD(TAG, "Fuck Me");

    delay(50); // delay for ADC stable
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

void loop()
{
    delay(1000);

    // Set the system time to RTC time
    tm currentTime;
    RTC.getTime(&currentTime);

    // char timeStr[32];
    // strftime(timeStr, 32, "%c", &currentTime);
    // ESP_LOGD(TAG, "Current time: %s", timeStr);
    
    // auto t = mktime(&currentTime);
    // struct timeval now = {.tv_sec = t, .tv_usec = 0};
    // settimeofday(&now, NULL);


    vCheckAlarms(currentTime);
    vRcvTimeSettingReq();

    if ((false == boNeedWakeup(true)) && (false == boNeedWakeup(false)))
    {
        vGotoSleep();
    }

    if (boNoisy())
    {
        vResetSleepTimer();
    }
}
