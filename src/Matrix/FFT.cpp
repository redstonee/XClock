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

#include <Arduino.h>

NS_DT_BEGIN

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
    DT_SAFE_FREE(fft);
}

bool FFTLayer::initLayer()
{
    Size size = this->getContentSize();
    fft = new arduinoFFT(realComponent,imagComponent,SAMPLES,SAMPLING_FREQ);
    reference = log10(60.0);
    // 计算截止频率，以对数标度为基数 POt
    double basePot = pow(SAMPLING_FREQ / 2.0, 1.0 / FREQUENCY_BANDS);
    coutoffFrequencies[0] = basePot;
    for (int i = 1 ; i < FREQUENCY_BANDS; i++ ) 
    {
        coutoffFrequencies[i] = basePot * coutoffFrequencies[i - 1];
    }
    music_icon = FrameSprite::create(SpriteFrame::create(icon_music,sizeof(icon_music),BMP_GIF));
    music_icon->setPosition(0,0);
    music_icon->setAutoSwitch(true);
    canvasSprite = CanvasSprite::create(FREQUENCY_BANDS,size.height);
    canvasSprite->setPosition(8,0);
    this->addChild(music_icon);
    this->addChild(canvasSprite);
    this->scheduleUpdate();
    return true;
}

void FFTLayer::update(float dt)
{
    Size size = this->getContentSize();
    SpriteCanvas* canvas = canvasSprite->getSpriteCanvas();
    canvas->canvasReset();
    for (int i=0; i<SAMPLES; i++)
    {
        realComponent[i] = (analogReadMilliVolts(4))/16;
        imagComponent[i] = 0;
    }
    fft->DCRemoval();
    fft->Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    fft->Compute(FFT_FORWARD);
    fft->ComplexToMagnitude();

    double median[FREQUENCY_BANDS];
    double max[FREQUENCY_BANDS];
    int oldHeight[FREQUENCY_BANDS]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    int oldMax[FREQUENCY_BANDS]= {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    int index = 0;
    double hzPerSample = (1.0 * SAMPLING_FREQ) / SAMPLES; //
    double hz = 0;
    double maxinband = 0;
    double sum = 0;
    int count = 0;
    for (int i = 0; i < (SAMPLES / 2) ; i++) 
    {
        count++;
        sum += realComponent[i];
        if (realComponent[i] >  max[index] ) 
        {
            max[index] = realComponent[i];
        }
        if (hz > coutoffFrequencies[index]) 
        {
            median[index] = sum / count;
            sum = 0.0;
            count = 0;
            index++;
            max[index] = 0;
            median[index]  = 0;
        }
        hz += hzPerSample;
    }
    // 计算每个频段的中值和最大值
    if ( sum > 0.0) 
    {
        median[index] =  sum / count;
        if (median[index] > maxinband) {
        maxinband = median[index];
        }
    }

    for (int i; i < FREQUENCY_BANDS; i++) 
    {
        int newHeight = 0;
        int newMax = 0;
        // 计算实际分贝
        if (median[i] > 0 && max[i] > 0 ) 
        {
            newHeight = 15.0 * (log10(median[i] ) - reference);
            newMax = 15.0 * (log10(max[i] ) - reference);
        }

        // 调整最小和最大级别
        if (newHeight < 0 ||  newMax < 0) 
        {
            newHeight = 1;
            newMax = 1;
        }
        if (newHeight >= CONTENT_HIGHT) 
        {
            newHeight = CONTENT_HIGHT;
        }
        if (newMax >= CONTENT_HIGHT) 
        {
            newMax = CONTENT_HIGHT;
        }
        if(oldMax[i]!=0 || newMax != 0)
        {
            if(newMax > oldMax[i])
            {
                newMax = (oldMax[i]*2 + newMax*8)/10;
            }
            else
            {
                newMax = (oldMax[i]*6 + newMax*4)/10;
            }
        }
        if(oldHeight[i]!=0 || newHeight != 0)
        {
            if(newHeight > oldHeight[i])
            {
                newHeight = (oldHeight[i]*2 + newHeight*8)/10;
            }
            else
            {
                newHeight = (oldHeight[i]*8 + newHeight*2)/10;
            }
        }
        
        canvas->writeLine(i, 8, i, 9 - newHeight, DTRGB(255,255,255));
        canvas->drawPixel(i, 8 - newMax, DTRGB(255,0,0));
        oldMax[i] = newMax;
        oldHeight[i] = newHeight;

    }
}


NS_DT_END