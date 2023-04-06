#include "BmpClass.h"
#include <Arduino.h>
bool BmpClass::decode(const uint8_t *fd,uint32_t size)
{
    int16_t i;
    bmtype = fd[0] + (fd[1] << 8);      //offset 0 'BM'
    if(0x4d42 != bmtype)
    {
        return false;
    }
    fd_addr = fd;
    bmdataptr = fd[10] + (fd[11] << 8); //offset 0xA pointer to image data
    bmhdrsize = fd[14] + (fd[15] << 8); //dib header size (0x28 is usual)
    //files may vary here, if !=28, unsupported type, put default values
    bmwidth = 0;
    bmheight = 0;
    bmbpp = 0;
    bmpltsize = 0;
    
    if ((bmhdrsize == 0x28) || (bmhdrsize == 0x38))
    {
        bmwidth = fd[18] + (fd[19] << 8);   //width
        bmheight = fd[22] + (fd[23] << 8);  //height
        bmbpp = fd[28] + (fd[29] << 8);     //bits per pixel
        bmpltsize = fd[46] + (fd[47] << 8); //palette size
    }
    bmbpl = ((bmbpp * bmwidth + 31) / 32) * 4; //bytes per line
    return true;
    // Serial.printf("bmtype: %d, bmhdrsize: %d, bmwidth: %d, bmheight: %d, bmbpp: %d\n", bmtype, bmhdrsize, bmwidth, bmheight, bmbpp);
}

BmpClass::BmpClass()
{
}

BmpClass::~BmpClass()
{
    fd_addr = nullptr;
}

uint32_t BmpClass::getColor(uint8_t x,uint8_t y) const
{
    BmpRGB color;
    if (bmbpp < 9)
    {
        color = getcol_plt(x,y);
        //Serial.printf("bmp plt:%x\n",color.getColor());
    }
    else if (bmbpp == 16)
    {
        // TODO: bpp 16 should have 3 pixel types
        color = getcol_Rgb565(x,y);
        //Serial.printf("bmp 565:%x\n",color.getColor());
    }
    else
    {
        color = getcol_true(x,y);
        //Serial.printf("bmp true:%x\n",color.getColor());
    }

    return color.getColor();
}

uint8_t BmpClass::bmpwidth()
{
    //Serial.printf("bmp w:%d\n",bmwidth);
    return (uint8_t)bmwidth;
}

uint8_t BmpClass::bmpheight()
{
    //Serial.printf("bmp h:%d\n",bmheight);
    return (uint8_t)bmheight;
}

BmpRGB BmpClass::getcol_plt(uint8_t x,uint8_t y) const
{
    uint8_t bmbitmask;
    int16_t bmppb , pla_index;
    BmpRGB color;
    bmppb = 8 / bmbpp;                         //pixels/byte
    bmbitmask = ((1 << bmbpp) - 1);            //mask for each pixel
    pla_index = fd_addr[bmdataptr + y * bmbpl + x/bmppb];
    pla_index = pla_index<<(bmbpp*(1+x%bmppb));
    pla_index = (bmbitmask & (pla_index >> 8));
    color.b = fd_addr[54+(pla_index*4)];
    color.g = fd_addr[54+(pla_index*4) + 1];
    color.r = fd_addr[54+(pla_index*4) + 2];
    return color;
}
BmpRGB BmpClass::getcol_Rgb565(uint8_t x,uint8_t y) const
{
    int16_t c;
    BmpRGB color;
    c = fd_addr[bmdataptr + y * bmbpl + x*2]|fd_addr[bmdataptr + y * bmbpl + x*2 + 1]<<8;
    color.r = (uint8_t)((c & 0xf800) >> 11);
    color.g = (uint8_t)((c & 0x7e0) >> 5);
    color.b = (uint8_t)((c & 0x1f));
    return color;
}

BmpRGB BmpClass::getcol_true(uint8_t x,uint8_t y) const
{
    BmpRGB color;
    uint8_t index;
    index = bmdataptr + (7-y) * bmbpl + x*3;
    color.b = fd_addr[index];
    color.g = fd_addr[index + 1];
    color.r = fd_addr[index + 2];
    //Serial.printf("xy:%d %d\n",x,y);
    return color;
}