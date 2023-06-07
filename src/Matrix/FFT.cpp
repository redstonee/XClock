/****************************************************************************
Copyright (c) 2021      Yinbaiyuan

https://www.yinbaiyuan.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "FFT.h"
#include "FastLED.h"
#include <Arduino.h>

NS_DT_BEGIN

CRGBPalette16 FFTPalette( RainbowColors_p );

bool FFT::init()
{
    FFTLayer *rootLayer = FFTLayer::create();
    rootLayer->setContentSize(Size(CONTENT_WIDTH,CONTENT_HIGHT));
    this->addChild(rootLayer);
    rootLayer->initLayer();
    return true;
}

FFTLayer::~FFTLayer()
{
    digitalWrite(ADC_MIC_EN_PIN,LOW);
    DT_SAFE_FREE(fft);
}

bool FFTLayer::initLayer()
{
    Size size = this->getContentSize();
    fft = new arduinoFFT(realComponent,imagComponent,SAMPLES,SAMPLING_FREQ);
    reference = log10(60.0);
    // 计算截止频率，以对数标度为基数 POt
    double basePot = pow(SAMPLING_FREQ, 1.0 / FREQUENCY_BANDS);
    coutoffFrequencies[0] = basePot;
    for (int i = 1 ; i < FREQUENCY_BANDS; i++ ) 
    {
        coutoffFrequencies[i] = basePot * coutoffFrequencies[i - 1];
        maxinband[i] = 0;
        median_pre[i] = 0;
    }
    music_icon = FrameSprite::create(SpriteFrame::create(icon_music,sizeof(icon_music),BMP_GIF));
    music_icon->setPosition(0,0);
    music_icon->setAutoSwitch(true);
    canvasSprite = CanvasSprite::create(FREQUENCY_BANDS,size.height);
    canvasSprite->setPosition(8,0);
    this->addChild(music_icon);
    this->addChild(canvasSprite);
    this->scheduleUpdate();
    pinMode(ADC_MIC_EN_PIN, OUTPUT);//MIC EN
    digitalWrite(ADC_MIC_EN_PIN,HIGH);
    return true;
}

void FFTLayer::update(float dt)
{
    static uint8_t color_index_offset = 0;
    Size size = this->getContentSize();
    SpriteCanvas* canvas = canvasSprite->getSpriteCanvas();
    canvas->canvasReset();
    for (int i=0; i<SAMPLES; i++)
    {
        realComponent[i] = (analogReadMilliVolts(ADC_PIN))/6;
        imagComponent[i] = 0;
    }
    fft->DCRemoval();
    fft->Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    fft->Compute(FFT_FORWARD);
    fft->ComplexToMagnitude();

    double median[FREQUENCY_BANDS];
    int index = 0;    
    double hz = 0;
    double sum = 0;
    int count = 0;
    for (int i = 0; i < SAMPLES ; i++) /*计算每个频率段的中值和最大值*/
    {
        count++;
        sum += realComponent[i];
        if (hz >= coutoffFrequencies[index]) //达到此段的截止频率，清零开始下个段的计算
        {
            if ( sum > 0.0) 
            {
                median[index] =  sum / count;
                median[index] =  (median[index] + median_pre[index])/2;
                if (median[index] > maxinband[index]) 
                {
                    maxinband[index]  = median[index];
                }                
            }
            sum = 0.0;
            count = 0;
            index++;
            median[index]  = 0;
        }
        hz += hzPerSample;
    }
    if ( sum > 0.0) //The last one
    {
        median[index] =  sum / count;
        median[index] =  (median[index] + median_pre[index])/2;
        if (median[index] > maxinband[index]) 
        {
            maxinband[index]  = median[index];
        }                
    }
    for (int i; i < FREQUENCY_BANDS; i++) 
    {
        int Height = 0;
        int Max = 0;
        // 计算实际分贝
        if (median[i] > 0 ) 
        {
            Height = 10.0 * (log10(median[i] ) - reference);            
        }
        if(maxinband[i] > 0)
        {
            Max = 10.0 * (log10(maxinband[i] ) - reference);
            maxinband[i] = maxinband[i] -15;
        }
        // 调整最小和最大级别
        if (Height < 0) 
        {
            Height = 0;
        }
        if (Height >= CONTENT_HIGHT) 
        {
            Height = CONTENT_HIGHT;
        }
        if (Max < 1) 
        {
            Max = 1;
        }
        if (Max >= CONTENT_HIGHT) 
        {
            Max = CONTENT_HIGHT;
        }
        if(Height)
        {
            for(uint8_t k = 0; k < Height; k++)
            {
                CRGB ColorFromPat = ColorFromPalette( FFTPalette, i*8+k + color_index_offset);
                DTRGB DotColor;
                DotColor.r = ColorFromPat.r;
                DotColor.g = ColorFromPat.g;
                DotColor.b = ColorFromPat.b;
                canvas->drawPixel(i, 8 - k, DotColor);
            }
        }
        canvas->drawPixel(i, 8 - Max, DTRGB(255,255,255));
        median_pre[i] = median[i];
    }
    color_index_offset++;
    
}


NS_DT_END