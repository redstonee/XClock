#ifndef __SD3078_H__
#define __SD3078_H__

#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>

class SD3078
{

private:
    static constexpr uint8_t TIME_REG_ADDR = 0x00;
    static constexpr uint8_t CTR1_REG_ADDR = 0x0f;
    static constexpr uint8_t CTR2_REG_ADDR = 0x10;
    static constexpr uint8_t TEMP_REG_ADDR = 0x10;
    const char *TAG = "Main";

    Adafruit_I2CDevice _i2cDev;
    Adafruit_BusIO_Register _timeReg; // time register
    Adafruit_BusIO_Register _tempReg; // temperature register
    Adafruit_BusIO_Register _ctr1Reg; // temperature register
    Adafruit_BusIO_Register _ctr2Reg; // temperature register

    struct __attribute__((packed)) TimeReg
    {
        uint8_t second; // 0-59
        uint8_t minute; // 0-59
        uint8_t hour;   // MSB = 0, 1-12; MSB = 1, 0-23
        uint8_t wday;   // 0-6
        uint8_t day;    // 1-31
        uint8_t month;  // 1-12
        uint8_t year;   // year since 2000
    };

public:
    SD3078(uint32_t sda_pin, uint32_t scl_pin, uint8_t addr = 0x32)
        : _i2cDev(addr),
          _timeReg(&_i2cDev, TIME_REG_ADDR, 7),
          _tempReg(&_i2cDev, TEMP_REG_ADDR, 1),
          _ctr1Reg(&_i2cDev, CTR1_REG_ADDR, 1),
          _ctr2Reg(&_i2cDev, CTR2_REG_ADDR, 1)
    {
        Wire.setPins(sda_pin, scl_pin);
    }

    inline bool begin()
    {
        return _i2cDev.begin();
    }

    inline bool getTime(tm *time)
    {
        TimeReg timeReg;
        if (!_timeReg.read(reinterpret_cast<uint8_t *>(&timeReg), 7))
            return false;

        time->tm_sec = ((timeReg.second >> 4) * 10) | (timeReg.second & 0x0f);
        time->tm_min = ((timeReg.minute >> 4) * 10) | (timeReg.minute & 0x0f);

        if (timeReg.hour & 0x80) // MSB = 1 for 24h format
        {
            time->tm_hour = (((timeReg.hour & 0x7f) >> 4) * 10) | (timeReg.hour & 0x0f); // clear MSB
        }
        else
        {
            time->tm_hour = (((timeReg.hour & 0x1f) >> 4) * 10) | (timeReg.hour & 0x0f); // clear AM/PM bit
            if (timeReg.hour & 0x20)                                                     // AM/PM bit = 1 for PM
                time->tm_hour += 12;                                                     // convert to 24h format
        }

        time->tm_mday = ((timeReg.day >> 4) * 10) | (timeReg.day & 0x0f);                  // tm_mday is 1-31
        time->tm_mon = ((timeReg.month >> 4) * 10) | (timeReg.month & 0x0f) - 1;           // convert to 0-11
        time->tm_year = ((timeReg.year >> 4) * 10) | (timeReg.month & 0x0f) + 2000 - 1900; // tm_year is years since 1900
        time->tm_wday = ((timeReg.wday >> 4) * 10) | (timeReg.wday & 0x0f);                // tm_wday is 0-6
        time->tm_yday = 0;                                                                 // not used
        time->tm_isdst = 0;                                                                // not used
        return true;
    }

    bool setTime(const tm &time)
    {
        TimeReg timeReg;
        auto month = time.tm_mon + 1;           // tm_mon is 0-11
        auto year = time.tm_year + 1900 - 2000; // tm_year is years since 1900

        timeReg.second = ((time.tm_sec / 10) << 4) | (time.tm_sec % 10);
        timeReg.minute = ((time.tm_min / 10) << 4) | (time.tm_min % 10);
        timeReg.hour = ((time.tm_hour / 10) << 4) | (time.tm_hour % 10) | 0x80; // MSB = 1 for 24h format
        timeReg.day = ((time.tm_mday / 10) << 4) | (time.tm_mday % 10);
        timeReg.month = ((month / 10) << 4) | (month % 10);
        timeReg.year = ((year / 10) << 4) | (year % 10);
        timeReg.wday = ((time.tm_wday / 10) << 4) | (time.tm_wday % 10);

        if (_ctr2Reg.write(0x80)) // Write enable bit WRTC1
        {
            if (_ctr1Reg.write(0x84)) // Write enable bit WRTC2 WRTC3
            {
                if (_timeReg.write(reinterpret_cast<uint8_t *>(&timeReg), 7))
                {
                    _ctr1Reg.write(0x00);
                    _ctr2Reg.write(0x00);
                    return true;
                }
            }
        }
        return false;
    }

    inline bool getTemp(int8_t *temp)
    {
        return _tempReg.read((uint8_t *)temp);
    }
};

#endif