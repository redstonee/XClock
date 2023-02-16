#ifndef __SD3078_H__
#define __SD3078_H__

#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>

#define SD3078SDA_PIN     23
#define SD3078SCL_PIN     22
#define SD3078Addr        0x32
#define SD3078TimeRegAddr  0x00
#define SD3078CTR1RegAddr  0x0F
#define SD3078CTR2RegAddr  0x10
#define SD3078TempRegAddr  0x16

typedef struct
{
    uint8_t u8Sec;
    uint8_t u8Min;
    uint8_t u8Hour;
    uint8_t u8Week;
    uint8_t u8Day;
    uint8_t u8Month;
    uint8_t u8Year;
}tst3078Time;


class SD3078
{
public:
    SD3078();
    bool ReadTime(tst3078Time* time);
    bool SetTime(tst3078Time* time);
    bool ReadTemp(int8_t* temp);
private:
    Adafruit_I2CDevice *_i2cDev;
    Adafruit_BusIO_Register *_timereg;// time register
    Adafruit_BusIO_Register *_tempreg;//temperature register
    Adafruit_BusIO_Register *_ctr1reg;//temperature register
    Adafruit_BusIO_Register *_ctr2reg;//temperature register
};

#endif