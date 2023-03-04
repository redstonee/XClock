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

#include "dtSpriteFrame.h"
#include <stdlib.h>
#include <string>
#include "dtDirector.h"
#include "platform/dtFileUtils.h"

NS_DT_BEGIN

SpriteFrame* SpriteFrame::create()
{
    SpriteFrame *spriteFrame = new SpriteFrame();
    if(spriteFrame)
    {
        spriteFrame->autorelease();
        return spriteFrame;
    }
    DT_SAFE_DELETE(spriteFrame);
    return nullptr;
}

SpriteFrame* SpriteFrame::create(const std::string& path)
{
    uint32_t size = 0;
    uint8_t* bitmap = FileUtils::getInstance()->readDataFromPath(path.c_str(),&size);
    SpriteFrame *frame = SpriteFrame::create(bitmap,size);
    free(bitmap);
    return frame;
}

SpriteFrame* SpriteFrame::create(const uint8_t* bitmap,uint32_t size)
{
    SpriteFrame *spriteFrame = new SpriteFrame();
    if(spriteFrame && spriteFrame->init(bitmap,size))
    {
        spriteFrame->autorelease();
        return spriteFrame;
    }
    DT_SAFE_DELETE(spriteFrame);
    return nullptr;
}

SpriteFrame* SpriteFrame::create(const uint8_t* bitmap,uint32_t size,BMPType bmp_type)
{
    SpriteFrame *spriteFrame = new SpriteFrame();
    if(spriteFrame == nullptr)
    {
        return nullptr;
    }
    spriteFrame->_bmp_type = bmp_type;
    if(spriteFrame && spriteFrame->init(bitmap,size))
    {
        spriteFrame->autorelease();
        return spriteFrame;
    }
    DT_SAFE_DELETE(spriteFrame);
    return nullptr;
}

SpriteFrame::SpriteFrame()
{

}

SpriteFrame::~SpriteFrame()
{

}

DTRGB SpriteFrame::colorWithXY(uint16_t x,uint16_t y) const
{
    if(BMP_DGF == this->_bmp_type)
    {
        return DTRGB(DgfDecoder::getColor(x,y));
    }
    else if(BMP_GIF == this->_bmp_type)
    {
        return DTRGB(GifClass::getColor(x,y));
    }
    else if(BMP_BMP == this->_bmp_type)
    {
        return DTRGB(BmpClass::getColor(x,y));
    }
    else
    {
        return DTRGB(0,0,0);
    }
}

void SpriteFrame::setSaturationRate(float rate)
{
    if(BMP_DGF == this->_bmp_type)
    {
        for (uint8_t i = 0; i < this->colorCount(); i++)
        {
            DTRGB color = this->getColorByIndex(i);
            DTHSV chsv = rgb2hsv_approximate(color);
            chsv.s += (255 - chsv.s) * rate;
            DTRGB crgb = chsv;
            this->setColor(i,crgb.r,crgb.g,crgb.b);
        } 
    }
    
}

bool SpriteFrame::init(const uint8_t* bitmap,uint32_t size)
{
    bool res = false;
    switch(this->_bmp_type)
    {
        case BMP_DGF:
            res = DgfDecoder::decode(bitmap,size);
            break;
        case BMP_GIF:
            res = GifClass::decode(bitmap,size);
            break;
        case BMP_BMP:
            res = BmpClass::decode(bitmap,size);
            break;
        default:break;
    }
    return res;
}

bool SpriteFrame::decode_bmp(const uint8_t* bitmap,uint32_t size)
{
    bool res = false;
    switch(this->_bmp_type)
    {
        case BMP_DGF:
            res = DgfDecoder::decode(bitmap,size);
            break;
        case BMP_GIF:
            res = GifClass::decode(bitmap,size);
            break;
        case BMP_BMP:
            res = BmpClass::decode(bitmap,size);
            break;
        default:break;
    }
    return res;
}

uint16_t SpriteFrame::getDuration_bmp() const
{
    uint16_t dur = 0;
    switch(this->_bmp_type)
    {
        case BMP_DGF:
            dur = DgfDecoder::getDuration();
            break;
        case BMP_GIF:
            dur = GifClass::getDuration();
            break;
        default:break;
    }
    return dur;
}

bool SpriteFrame::renderNextFrame_bmp(bool loop)
{
    bool res = false;
    if(BMP_DGF == this->_bmp_type)
    {
        res = renderNextFrame(loop);
    }
    else if(BMP_GIF == this->_bmp_type)
    {
        res = GifClass::renderFrame(loop);
    }
    return res;
}

uint8_t SpriteFrame::width_bmp()
{
    if(BMP_DGF == this->_bmp_type)
    {
        return width();
    }
    else if(BMP_GIF == this->_bmp_type)
    {
        return gifwidth();
    }
    else if(BMP_BMP == this->_bmp_type)
    {
        return bmpwidth();
    }
    return 0;
}

uint8_t SpriteFrame::height_bmp()
{
    if(BMP_DGF == this->_bmp_type)
    {
        return height();
    }
    else if(BMP_GIF == this->_bmp_type)
    {
        return gifheight();
    }
    else if(BMP_BMP == this->_bmp_type)
    {
        return bmpheight();
    }
    return 0;
}

NS_DT_END
