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

#ifndef __DT_SPRITE_FRAME_H__
#define __DT_SPRITE_FRAME_H__

#include <string>
#include "base/dtMacros.h"
#include "base/dtRef.h"
#include "base/dtTypes.h"
#include "extended/DgfParser.h"
#include "extended/GifClass.h"
#include "extended/BmpClass.h"

NS_DT_BEGIN

class SpriteFrame : public Ref , public DgfDecoder ,public GifClass,public BmpClass
{
private:

    

public:
    BMPType _bmp_type;


public:
    static SpriteFrame* create();

    static SpriteFrame* create(const std::string& path);

    static SpriteFrame* create(const uint8_t* bitmap,uint32_t size);

    static SpriteFrame* create(const uint8_t* bitmap,uint32_t size,BMPType bmp_type);

    DTRGB colorWithXY(uint16_t x,uint16_t y) const;

    void setSaturationRate(float rate);
    
    bool init(const uint8_t* bitmap,uint32_t size);

    bool decode_bmp(const uint8_t* bitmap,uint32_t size);

    uint16_t getDuration_bmp() const;

    bool renderNextFrame_bmp(bool loop);

    uint8_t width_bmp();

    uint8_t height_bmp();
protected:

    SpriteFrame();

    virtual ~SpriteFrame();

    

};

NS_DT_END



#endif //__DT_SPRITE_FRAME_H__
