#ifndef __BMPCLASS_H__
#define __BMPCLASS_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

struct BmpRGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    inline BmpRGB() __attribute__((always_inline))
    :r(0),g(0),b(0)
    {

    }

    inline BmpRGB( uint32_t colorcode)  __attribute__((always_inline))
    : r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b((colorcode >> 0) & 0xFF)
    {

    }
    
    inline uint32_t getColor () const
    {
        uint32_t color = 0;
        color = (color + r)<<8;
        color = (color + g)<<8;
        color = (color + b);
        return color;
    }

    inline BmpRGB& operator= (const uint32_t colorcode) __attribute__((always_inline))
    {
        r = (colorcode >> 16) & 0xFF;
        g = (colorcode >>  8) & 0xFF;
        b = (colorcode >>  0) & 0xFF;
        return *this;
    }
};

class BmpClass
{
public:
    bool decode(const uint8_t *fd,uint32_t size);

    uint32_t getColor(uint8_t x,uint8_t y)const;

    uint8_t bmpwidth();

    uint8_t bmpheight();
private:
    const uint8_t *fd_addr;
    uint16_t bmtype, bmdataptr;                              //from header
    uint32_t bmhdrsize, bmwidth, bmheight, bmbpp, bmpltsize; //from DIB Header
    uint16_t *bmplt;                                        //palette- stored encoded for LCD
    uint16_t *bmpRow;
    uint16_t bmbpl;                                          //bytes per line- derived
    BmpRGB getcol_plt(uint8_t x,uint8_t y) const;
    BmpRGB getcol_Rgb565(uint8_t x,uint8_t y) const;
    BmpRGB getcol_true(uint8_t x,uint8_t y) const;
};

#endif