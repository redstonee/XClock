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
#include "shitTime.h"

QueueHandle_t KeyQueue = nullptr;
static SD3078 RTC(RTC_SDA_PIN, RTC_SCL_PIN, RTC_ADDR);

static const char *TAG = "Main";

uint8_t i8SleepReqCnt = 0;
uint8_t i8SleepReqHMICnt = 0;
static double realComponent[64];
static double imagComponent[64];
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



void RequestWakeup(bool boHMIDis)
{
    if (boHMIDis)
        i8SleepReqHMICnt++;
    else
        i8SleepReqCnt++;
}

inline bool isWakeupNeeded(bool boHMIDis)
{
    return boHMIDis ? (i8SleepReqHMICnt > 0) : (i8SleepReqCnt > 0);
}

void ClearWakeupRequest(bool boHMIDis)
{
    if (boHMIDis)
    {
        if (i8SleepReqHMICnt)
            i8SleepReqHMICnt--;
    }
    else
    {
        if (i8SleepReqCnt)
            i8SleepReqCnt--;
    }
}



void vCheckAlarms(tm currentTime)
{
    uint8_t AlarmNum = getAlarmClockCount();
    AlarmConfig AlarmClkTmp = {
        0,
    };
    if (AlarmNum)
    {
        for (uint8_t i = 0; i < AlarmNum; i++)
        {
            AlarmClkTmp = getAlarmClock(i);
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

void vGotoSleep(bool enableTimerWake = true)
{
    ESP_LOGI(TAG, "Go to sleep\n\r");
    digitalWrite(MIC_EN_PIN, LOW);
    digitalWrite(LED_EN_PIN, LOW);
    esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(BUTTON_OK_PIN), 0);
    if (enableTimerWake)
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
        ESP_LOGD(TAG, "nosiy %g\n\r", sum);
        return true;
    }
    ESP_LOGD(TAG, "%g\n\r", sum);
    return false;
}

void setup()
{
    BattMon::begin();
    delay(100);
    if (BattMon::getBatteryLevel() < 10)
    {
        ESP_LOGE(TAG, "Battery level is low\n\r");
        vGotoSleep(false);
    }
    pinMode(MIC_EN_PIN, OUTPUT);
    digitalWrite(MIC_EN_PIN, HIGH);
    analogSetPinAttenuation(MIC_SIG_PIN, ADC_6db);

    if (!RTC.begin())
    {
        ESP_LOGE(TAG, "Failed to initialize RTC\n\r");
        while (1)
        {
            delay(1000);
        }
    }
    ESP_LOGI(TAG, "RTC initialized\n\r");

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

    // Set the system time to RTC time
    tm currentTime;
    if (!RTC.getTime(&currentTime))
    {
        ESP_LOGE(TAG, "Failed to read RTC time\n\r");
        while (1)
        {
            delay(1000);
        }
    }
    setSystemTime(currentTime);

    vCheckAlarms(currentTime);

    ESP_LOGD(TAG, "Fuck Me");

    delay(50); // delay for ADC stable
    if (boNoisy() || isWakeupNeeded(true) || ESP_SLEEP_WAKEUP_EXT0 == esp_sleep_get_wakeup_cause())
    {
        vCreateKeyQueue();
        static ClockKey keyHandler;
        keyHandler.SetSendQueue(KeyQueue);
        keyHandler.Start();
        pinMode(LED_EN_PIN, OUTPUT); // MIC EN
        digitalWrite(LED_EN_PIN, HIGH);
        vMatrixInit(KeyQueue);
    }
    if (!(isWakeupNeeded(true) || isWakeupNeeded(false)))
    {
        vGotoSleep();
    }

    initWiFi();
}

void loop()
{
    delay(5000);

    if (BattMon::getBatteryLevel() < 10)
    {
        ESP_LOGE(TAG, "Battery level is low\n\r");
        vGotoSleep(false);
    }

    tm currentTime;
    getLocalTime(&currentTime);
    vCheckAlarms(currentTime);
    RTC.setTime(currentTime);


    if (!(isWakeupNeeded(true) || isWakeupNeeded(false)))
    {
        vGotoSleep();
    }

    if (boNoisy())
    {
        vResetSleepTimer();
    }
}
