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

#ifndef __FFT_H__
#define __FFT_H__

#include "Dot2D/dot2d.h"
#include <arduinoFFT.h>

#define SAMPLING_FREQ 8000
#define SAMPLES       64
#define FREQUENCY_BANDS 24
#define CONTENT_HIGHT   8
#define CONTENT_WIDTH   32
NS_DT_BEGIN

class FFT : public Scene
{

protected:

    bool init() override;

public:

    STATIC_CREATE(FFT);

};

class FFTLayer : public Layer
{
protected:
    
    uint8_t animateCount = 0;

    double realComponent[64];
    double imagComponent[64];
    double coutoffFrequencies[FREQUENCY_BANDS];
    float reference;
    arduinoFFT* fft;

public:

    virtual ~FFTLayer();

protected:
    
    CanvasSprite* canvasSprite = nullptr;
    FrameSprite* music_icon = nullptr;
public:

    STATIC_CREATE(FFTLayer);

    virtual bool initLayer();
    
    void update(float dt) override;

};

/*Music icon, GIF*/
const unsigned char icon_music[] PROGMEM  = {
	0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x08, 0x00, 0x08, 0x00, 0xA1, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0xFF, 0x00, 0x00, 0x0D, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x21, 0xFF, 0x0B, 0x4E, 0x45, 0x54, 0x53, 
	0x43, 0x41, 0x50, 0x45, 0x32, 0x2E, 0x30, 0x03, 0x01, 0x00, 0x00, 0x00, 0x21, 0xF9, 0x04, 0x05, 
	0x32, 0x00, 0x03, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x02, 0x0F, 
	0x84, 0x8F, 0x10, 0x1B, 0xD8, 0xEA, 0xCC, 0x53, 0xAF, 0xC2, 0x75, 0x9A, 0x4E, 0x10, 0x14, 0x00, 
	0x21, 0xF9, 0x04, 0x05, 0x32, 0x00, 0x03, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 
	0x00, 0x00, 0x02, 0x0F, 0x04, 0x84, 0x62, 0xA6, 0xB7, 0x0E, 0x4F, 0x9B, 0x0E, 0x2D, 0x85, 0x1F, 
	0xD3, 0xBC, 0x00, 0x00, 0x3B
};

NS_DT_END

#endif //__FFT_H__