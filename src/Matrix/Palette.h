#ifndef __PALETTE_H__
#define __PALETTE_H__
#include <Arduino.h>
#include "FastLED.h"


uint8_t u8GetGlobalPaltIdx(void);

bool boSetGlobalPaltIdx(uint8_t);

uint8_t u8GetGlobalColorIdx(void);

bool boSetGlobalColorIdx(uint8_t);

CRGBPalette16 pGetPalette(uint8_t Index);
#endif