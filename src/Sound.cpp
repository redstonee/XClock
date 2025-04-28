#include <Arduino.h>
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "Sound.h"
#include "SoundData.h"

AudioGeneratorWAV *wav = NULL;
AudioFileSourcePROGMEM *file = NULL;
AudioOutputI2S *out = NULL;
tenSoundID RequestedSnd = enSndID_None;
uint16_t u16ReqTime = 0;

SemaphoreHandle_t xMutex_ReqSnd = NULL;
static const char *TAG = "Sound";

typedef struct
{
    const unsigned char *pSndData;
    uint32_t u32DataLen;
} tstSndListItem;

const tstSndListItem SndList[] PROGMEM = {
    {NULL, 0},
    {DingDong, sizeof(DingDong)},
    {Alarm1, sizeof(Alarm1)},
    {Alarm2, sizeof(Alarm2)},
    {Ding, sizeof(Ding)},
    {Boo, sizeof(Boo)},
};

bool boReqSound(tenSoundID SndID, uint16_t times)
{
    bool result = false;
    if (SndID < enSndID_TotalNum)
    {
        if (xSemaphoreTake(xMutex_ReqSnd, (TickType_t)100) == pdTRUE)
        {
            if (enSndID_None != RequestedSnd)
            {
                result = false;
            }
            else
            {

                RequestedSnd = SndID;
                u16ReqTime = times;

                result = true;
            }
            xSemaphoreGive(xMutex_ReqSnd);
        }
        else
        {
            ESP_LOGE(TAG, "Can not get metux4!\n\r");
        }
    }

    return result;
}

tenSoundID enGetCurSndID(void)
{
    return RequestedSnd;
}

void vStopSound(void)
{
    if (wav->isRunning())
    {

        wav->stop();
        digitalWrite(21, LOW);
        if (xSemaphoreTake(xMutex_ReqSnd, (TickType_t)100) == pdTRUE)
        {
            u16ReqTime = 0;
            RequestedSnd = enSndID_None;
            // digitalWrite(21,LOW);
            xSemaphoreGive(xMutex_ReqSnd);
        }
        else
        {
            ESP_LOGE(TAG, "Can not get metux3!\n\r");
        }
    }
}

void vSoundLoop(void *param)
{
    for (;;)
    {
        if (wav->isRunning())
        {
            if (!wav->loop())
            {
                wav->stop();
                digitalWrite(21, LOW);
                if (xSemaphoreTake(xMutex_ReqSnd, (TickType_t)100) == pdTRUE)
                {

                    if (0 < --u16ReqTime)
                    {
                        file->open(SndList[RequestedSnd].pSndData, SndList[RequestedSnd].u32DataLen);
                        digitalWrite(21, HIGH);
                        wav->begin(file, out);
                    }
                    else
                    {
                        RequestedSnd = enSndID_None;
                        // digitalWrite(21,LOW);
                    }
                    xSemaphoreGive(xMutex_ReqSnd);
                }
                else
                {
                    ESP_LOGE(TAG, "Can not get metux2!\n\r");
                }
            }
        }
        else
        {
            if (xSemaphoreTake(xMutex_ReqSnd, (TickType_t)100) == pdTRUE)
            {
                if (enSndID_None != RequestedSnd)
                {
                    // Serial.printf("Request Snd:%d!\n",RequestedSnd);
                    // digitalWrite(21,HIGH);
                    // Serial.printf("SoundData:%d len:%d!\n",SndList[RequestedSnd].pSndData[64],SndList[RequestedSnd].u32DataLen);
                    file->open(SndList[RequestedSnd].pSndData, SndList[RequestedSnd].u32DataLen);
                    digitalWrite(21, HIGH);
                    wav->begin(file, out);
                    // Serial.printf("begin!\n");
                }
                xSemaphoreGive(xMutex_ReqSnd);
            }
            else
            {
                ESP_LOGE(TAG, "Can not get metux1!\n\r");
            }
        }
        vTaskDelay(10);
    }
}

bool vSoundInit(void)
{
    pinMode(21, OUTPUT); // Sound SD_Mode
    // digitalWrite(21,HIGH);
    digitalWrite(21, LOW);
    audioLogger = &Serial;
    file = new AudioFileSourcePROGMEM();
    if (file == NULL)
    {
        ESP_LOGE(TAG, "Create file failed!\n\r");
        return false;
    }
    out = new AudioOutputI2S();
    if (out == NULL)
    {
        ESP_LOGE(TAG, "Create out failed!\n\r");
        return false;
    }
    wav = new AudioGeneratorWAV();
    if (wav == NULL)
    {
        ESP_LOGE(TAG, "Create wav failed!\n\r");
        return false;
    }
    out->SetPinout(18, 5, 19);
    xMutex_ReqSnd = xSemaphoreCreateMutex();
    if (!xMutex_ReqSnd)
    {
        ESP_LOGE(TAG, "Create request sound mutex failed!\n\r");
        return false;
    }
    xTaskCreate(
        vSoundLoop,        // Function that should be called
        "Sound main task", // Name of the task (for debugging)
        4000,              // Stack size (bytes)
        NULL,              // Parameter to pass
        3,                 // Task priority
        NULL               // Task handle
    );
    return true;
}
