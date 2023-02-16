#include <Arduino.h>
#include <Wire.h>
#include "SD3078.h"

Adafruit_I2CDevice i2c_dev = Adafruit_I2CDevice(SD3078Addr);
Adafruit_BusIO_Register timereg = Adafruit_BusIO_Register(&i2c_dev,SD3078TimeRegAddr,7);
Adafruit_BusIO_Register tempreg = Adafruit_BusIO_Register(&i2c_dev,SD3078TempRegAddr,1);//temperature register
Adafruit_BusIO_Register ctr1reg = Adafruit_BusIO_Register(&i2c_dev,SD3078CTR1RegAddr,1);//control 1
Adafruit_BusIO_Register ctr2reg = Adafruit_BusIO_Register(&i2c_dev,SD3078CTR2RegAddr,1);//control 2

SD3078::SD3078(void)
{
    Wire.setPins(SD3078SDA_PIN,SD3078SCL_PIN);
    _i2cDev = &i2c_dev;
    _timereg = &timereg;// time register
    _tempreg = &tempreg;//temperature register
    _ctr1reg = &ctr1reg;//temperature register
    _ctr2reg = &ctr2reg;//temperature register
    if (!_i2cDev->begin()) {
        Serial.print("Did not find device at 0x");
        Serial.println(_i2cDev->address(), HEX);
        while (1);
    }
    Serial.print("Device found on address 0x");
    Serial.println(_i2cDev->address(), HEX);
}

bool SD3078::ReadTime(tst3078Time* time)
{
    return _timereg->read((uint8_t*)time,7);
}

bool SD3078::SetTime(tst3078Time* time)
{
    if(_ctr2reg->write(0x80))  //Write enable bit WRTC1
    {
        if(_ctr1reg->write(0x84)) //Write enable bit WRTC2 WRTC3
        {
            if(_timereg->write((uint8_t*)time,7))
            {
                _ctr1reg->write(0x00);
                _ctr2reg->write(0x00);
                return true;
            }
        }
    }

    return false;
}

bool SD3078::ReadTemp(int8_t* temp)
{
    return _tempreg->read((uint8_t*)temp);
}