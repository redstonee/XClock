#ifndef __SD3078_H__
#define __SD3078_H__

#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>

typedef struct
{
    uint8_t u8Sec;
    uint8_t u8Min;
    uint8_t u8Hour;
    uint8_t u8Week;
    uint8_t u8Day;
    uint8_t u8Month;
    uint8_t u8Year;
} tst3078Time;

class SD3078
{

private:
    static constexpr uint8_t TIME_REG_ADDR = 0x00;
    static constexpr uint8_t CTR1_REG_ADDR = 0x0f;
    static constexpr uint8_t CTR2_REG_ADDR = 0x10;
    static constexpr uint8_t TEMP_REG_ADDR = 0x10;

    Adafruit_I2CDevice _i2cDev;
    Adafruit_BusIO_Register _timeReg; // time register
    Adafruit_BusIO_Register _tempReg; // temperature register
    Adafruit_BusIO_Register _ctr1Reg; // temperature register
    Adafruit_BusIO_Register _ctr2Reg; // temperature register

public:
    SD3078(uint32_t sda_pin, uint32_t scl_pin, uint8_t addr = 0x32)
        : _i2cDev(addr),
          _timeReg(&_i2cDev, TIME_REG_ADDR, 7),
          _tempReg(&_i2cDev, TEMP_REG_ADDR, 1),
          _ctr1Reg(&_i2cDev, CTR1_REG_ADDR, 1),
          _ctr2Reg(&_i2cDev, CTR2_REG_ADDR, 1)
    {
        Wire.setPins(sda_pin, scl_pin);
        if (!_i2cDev.begin())
        {
            Serial.print("Did not find device at 0x");
            Serial.println(_i2cDev.address(), HEX);
            while (1)
                ;
        }
        Serial.print("Device found on address 0x");
        Serial.println(_i2cDev.address(), HEX);
    }

    inline bool ReadTime(tst3078Time *time)
    {
        return _timeReg.read((uint8_t *)time, 7);
    }

    bool SetTime(tst3078Time *time)
    {
        if (_ctr2Reg.write(0x80)) // Write enable bit WRTC1
        {
            if (_ctr1Reg.write(0x84)) // Write enable bit WRTC2 WRTC3
            {
                if (_timeReg.write((uint8_t *)time, 7))
                {
                    _ctr1Reg.write(0x00);
                    _ctr2Reg.write(0x00);
                    return true;
                }
            }
        }

        return false;
    }

    inline bool ReadTemp(int8_t *temp)
    {
        return _tempReg.read((uint8_t *)temp);
    }
};

#endif