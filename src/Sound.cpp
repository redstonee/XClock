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

typedef struct
{
    const unsigned char *pSndData;
    uint32_t u32DataLen;
}tstSndListItem;

const tstSndListItem SndList[] PROGMEM = {
    {NULL,0},
	{DingDong,sizeof(DingDong)},
    {Alarm1,sizeof(Alarm1)},
    {Alarm2,sizeof(Alarm2)},
    {Ding,sizeof(Ding)},
    {Boo,sizeof(Boo)},	
};

bool boReqSound(tenSoundID SndID, uint16_t times)
{
    bool result = false;
    if(SndID < enSndID_TotalNum)
    {
        if(NULL != xMutex_ReqSnd)
        {
           if( xSemaphoreTake( xMutex_ReqSnd, ( TickType_t ) 100 ) == pdTRUE )
            {
                if(enSndID_None != RequestedSnd)
                {
                    result = false;
                }
                else
                {                
                        
                    RequestedSnd = SndID;
                    u16ReqTime = times;
                    
                    result = true;                                        
                }
                xSemaphoreGive( xMutex_ReqSnd );
            }
            else
            {
                Serial.printf("Can not get metux4!\n\r");
            }
        }
        else
        {
            Serial.printf("Request sound mutex not active!\n\r");
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
        digitalWrite(21,LOW);
        if(NULL != xMutex_ReqSnd)
        {
            if( xSemaphoreTake( xMutex_ReqSnd, ( TickType_t ) 100 ) == pdTRUE )
            {
                u16ReqTime = 0;
                RequestedSnd = enSndID_None;
                //digitalWrite(21,LOW);
                xSemaphoreGive( xMutex_ReqSnd );
            }
            else
            {
                Serial.printf("Can not get metux3!\n\r");
            }
        }
        else
        {
            Serial.printf("Request sound mutex not active!\n\r");
        } 
    } 
}

void vSoundLoop(void *param)
{
    for(;;)
    {
        if (wav->isRunning())
        {
            if (!wav->loop())
            {
                wav->stop();
                digitalWrite(21,LOW);
                if(NULL != xMutex_ReqSnd)
                {
                    if( xSemaphoreTake( xMutex_ReqSnd, ( TickType_t ) 100 ) == pdTRUE )
                    {
                        
                        if(0 < --u16ReqTime)
                        {
                            file->open(SndList[RequestedSnd].pSndData, SndList[RequestedSnd].u32DataLen);
                            digitalWrite(21,HIGH);
                            wav->begin(file, out);
                        }
                        else
                        {
                            RequestedSnd = enSndID_None;
                            //digitalWrite(21,LOW);
                        }
                        xSemaphoreGive( xMutex_ReqSnd );
                    }
                    else
                    {
                        Serial.printf("Can not get metux2!\n\r");
                    }
                }
                else
                {
                    Serial.printf("Request sound mutex not active!\n\r");
                } 
            } 
        } 
        else 
        {
            if(NULL != xMutex_ReqSnd)
            {
                if( xSemaphoreTake( xMutex_ReqSnd, ( TickType_t ) 100 ) == pdTRUE )
                {
                    if(enSndID_None != RequestedSnd)
                    {
                        //Serial.printf("Request Snd:%d!\n",RequestedSnd);
                        //digitalWrite(21,HIGH);
                        //Serial.printf("SoundData:%d len:%d!\n",SndList[RequestedSnd].pSndData[64],SndList[RequestedSnd].u32DataLen);
                        file->open(SndList[RequestedSnd].pSndData, SndList[RequestedSnd].u32DataLen);
                        digitalWrite(21,HIGH);
                        wav->begin(file, out);
                        //Serial.printf("begin!\n");
                    }
                    xSemaphoreGive( xMutex_ReqSnd );
                }
                else
                {
                    Serial.printf("Can not get metux1!\n\r");
                }
            }
            else
            {
                Serial.printf("Request sound mutex not active!\n\r");
            } 
        }
        vTaskDelay(10);
    }
}

void vSoundInit(void)
{
    pinMode(21, OUTPUT);//Sound SD_Mode
    //digitalWrite(21,HIGH);
    digitalWrite(21,LOW);
    audioLogger = &Serial;
    file = new AudioFileSourcePROGMEM();
    if(file == NULL)
    {
        Serial.printf("Create file failed!\n\r");
    }
    out = new AudioOutputI2S();
    if(out == NULL)
    {
        Serial.printf("Create out failed!\n\r");
    }
    wav = new AudioGeneratorWAV();
    if(wav == NULL)
    {
        Serial.printf("Create wav failed!\n\r");
    }
    out->SetPinout(18,5,19);
    xMutex_ReqSnd = xSemaphoreCreateMutex();
    if(NULL == xMutex_ReqSnd)
    {
        Serial.printf("Create request sound mutex failed!\n\r");
    }
    xTaskCreate(
        vSoundLoop,    // Function that should be called
        "Sound main task",   // Name of the task (for debugging)
        4000,            // Stack size (bytes)
        NULL,            // Parameter to pass
        3,               // Task priority
        NULL             // Task handle
    );
}

