#include "Palette.h"
#include <Preferences.h>

#define MaxPaletteNum (sizeof(PaletteList) / sizeof(TProgmemRGBGradientPalette_bytes))
#define PrefKey_PaletteSpace "Palette"
#define PrefKey_PaletteIdx "PaletteIdx"
#define PrefKey_PaletteColIdx "PaletteColIdx"

// Gradient palette "GMT_sealand_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_sealand.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

static const char *TAG = "Palette";

DEFINE_GRADIENT_PALETTE(GMT_sealand_gp){
    0, 53, 33, 255,
    14, 23, 33, 255,
    28, 23, 68, 255,
    42, 23, 115, 255,
    56, 23, 178, 255,
    70, 23, 255, 255,
    84, 23, 255, 170,
    99, 23, 255, 103,
    113, 23, 255, 56,
    127, 23, 255, 25,
    141, 53, 255, 25,
    155, 100, 255, 25,
    170, 167, 255, 25,
    170, 255, 255, 87,
    184, 255, 193, 87,
    198, 255, 141, 87,
    212, 255, 99, 87,
    226, 255, 115, 135,
    240, 255, 135, 182,
    255, 255, 156, 223};

// Gradient palette "GMT_seafloor_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_seafloor.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 88 bytes of program space.

DEFINE_GRADIENT_PALETTE(GMT_seafloor_gp){
    0, 25, 0, 109,
    10, 28, 0, 119,
    21, 32, 0, 127,
    31, 35, 0, 140,
    42, 27, 1, 145,
    53, 20, 1, 151,
    74, 14, 4, 156,
    84, 9, 9, 164,
    95, 5, 15, 170,
    106, 2, 24, 176,
    116, 1, 35, 182,
    138, 1, 49, 188,
    148, 0, 66, 197,
    159, 1, 79, 203,
    170, 3, 93, 210,
    180, 10, 109, 216,
    191, 24, 128, 223,
    212, 43, 149, 230,
    223, 72, 173, 240,
    233, 112, 197, 247,
    244, 163, 225, 255,
    255, 220, 248, 255};

// Gradient palette "rainbow_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/oc/tn/rainbow.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 1016 bytes of program space.

DEFINE_GRADIENT_PALETTE(rainbow_gp){
    0, 57, 0, 31,
    1, 54, 0, 34,
    2, 51, 0, 36,
    3, 48, 0, 38,
    4, 46, 0, 41,
    5, 43, 0, 43,
    6, 40, 0, 46,
    7, 38, 0, 49,
    8, 35, 0, 52,
    9, 33, 0, 54,
    10, 31, 0, 57,
    11, 29, 0, 61,
    12, 27, 0, 64,
    13, 25, 0, 67,
    14, 23, 0, 71,
    15, 21, 0, 74,
    16, 20, 0, 78,
    17, 18, 0, 82,
    18, 17, 0, 85,
    19, 15, 0, 89,
    20, 14, 0, 93,
    21, 12, 0, 98,
    22, 11, 0, 102,
    23, 10, 0, 106,
    24, 9, 0, 111,
    25, 8, 0, 115,
    26, 7, 0, 120,
    27, 6, 0, 125,
    28, 5, 0, 130,
    29, 5, 0, 135,
    30, 4, 0, 140,
    31, 3, 0, 145,
    32, 3, 0, 151,
    33, 2, 0, 156,
    34, 2, 0, 162,
    35, 1, 0, 168,
    36, 1, 0, 174,
    05, 255, 17, 0,
    206, 255, 15, 0,
    207, 255, 13, 0,
    208, 255, 11, 0,
    209, 255, 10, 0,
    210, 255, 8, 0,
    211, 255, 7, 0,
    212, 255, 6, 0,
    213, 255, 5, 0,
    214, 255, 4, 0,
    215, 255, 3, 0,
    216, 255, 2, 0,
    217, 255, 1, 0,
    218, 255, 1, 0,
    219, 255, 1, 0,
    220, 255, 1, 0,
    221, 255, 1, 0,
    222, 255, 1, 0,
    223, 255, 1, 0,
    224, 255, 0, 0,
    225, 242, 0, 0,
    226, 229, 0, 0,
    227, 217, 0, 0,
    228, 206, 0, 0,
    229, 194, 0, 0,
    230, 184, 0, 0,
    231, 173, 0, 0,
    232, 163, 0, 0,
    233, 153, 0, 0,
    234, 144, 0, 0,
    235, 135, 0, 0,
    236, 126, 0, 0,
    237, 118, 0, 0,
    238, 110, 0, 0,
    239, 103, 0, 0,
    240, 95, 0, 0,
    241, 88, 0, 0,
    242, 82, 0, 0,
    243, 75, 0, 0,
    244, 69, 0, 0,
    245, 64, 0, 0,
    246, 58, 0, 0,
    247, 53, 0, 0,
    248, 48, 0, 0,
    249, 44, 0, 0,
    250, 39, 0, 0,
    251, 35, 0, 0,
    252, 32, 0, 0,
    253, 28, 0, 0,
    255, 25, 0, 0};

// Gradient palette "bhw1_07_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_07.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 8 bytes of program space.

DEFINE_GRADIENT_PALETTE(bhw1_07_gp){
    0, 232, 65, 1,
    255, 229, 227, 1};

// Gradient palette "bhw1_01_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_01.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 12 bytes of program space.

DEFINE_GRADIENT_PALETTE(bhw1_01_gp){
    0, 227, 101, 3,
    117, 194, 18, 19,
    255, 92, 8, 192};

// Gradient palette "bath_112_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/esri/hypsometry/bath/tn/bath_112.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 508 bytes of program space.

DEFINE_GRADIENT_PALETTE(bath_112_gp){
    0, 0, 3, 34,
    2, 0, 3, 34,
    4, 0, 4, 36,
    6, 0, 4, 36,
    8, 0, 5, 40,
    10, 0, 5, 40,
    12, 0, 5, 45,
    14, 0, 5, 45,
    16, 0, 5, 47,
    18, 0, 5, 47,
    20, 0, 6, 52,
    22, 0, 6, 52,
    24, 0, 7, 56,
    26, 0, 7, 56,
    28, 0, 8, 62,
    30, 0, 8, 62,
    32, 0, 8, 65,
    34, 0, 8, 65,
    36, 0, 9, 71,
    38, 0, 9, 71,
    40, 0, 9, 77,
    42, 0, 9, 77,
    44, 0, 11, 83,
    46, 0, 11, 83,
    48, 0, 12, 89,
    50, 0, 12, 89,
    52, 0, 12, 93,
    54, 0, 12, 93,
    56, 0, 13, 100,
    58, 0, 13, 100,
    60, 0, 14, 108,
    62, 0, 14, 108,
    64, 0, 16, 115,
    66, 0, 16, 115,
    68, 0, 18, 123,
    70, 0, 18, 123,
    55, 2, 72, 255,
    157, 3, 81, 255,
    159, 3, 81, 255,
    161, 4, 86, 255,
    163, 4, 86, 255,
    165, 6, 92, 255,
    167, 6, 92, 255,
    170, 7, 96, 255,
    172, 7, 96, 255,
    174, 10, 100, 255,
    176, 10, 100, 255,
    178, 13, 109, 255,
    180, 13, 109, 255,
    182, 15, 112, 255,
    184, 15, 112, 255,
    186, 18, 121, 255,
    188, 18, 121, 255,
    190, 20, 124, 255,
    192, 20, 124, 255,
    194, 25, 133, 255,
    196, 25, 133, 255,
    198, 30, 138, 255,
    200, 30, 138, 255,
    202, 33, 141, 255,
    204, 33, 141, 255,
    206, 37, 149, 255,
    208, 37, 149, 255,
    210, 42, 152, 255,
    212, 42, 152, 255,
    214, 51, 162, 255,
    216, 51, 162, 255,
    218, 56, 166, 255,
    220, 56, 166, 255,
    222, 61, 169, 255,
    224, 61, 169, 255,
    226, 67, 176, 255,
    228, 67, 176, 255,
    230, 77, 182, 255,
    232, 77, 182, 255,
    234, 86, 189, 255,
    236, 86, 189, 255,
    238, 92, 193, 255,
    240, 92, 193, 255,
    242, 101, 199, 255,
    244, 101, 199, 255,
    246, 109, 203, 255,
    248, 109, 203, 255,
    250, 120, 209, 255,
    252, 120, 209, 255,
    255, 120, 209, 255};

// Gradient palette "bhw1_03_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_03.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE(bhw1_03_gp){
    0, 0, 0, 0,
    137, 11, 112, 153,
    191, 40, 219, 105,
    255, 255, 255, 255};

// Gradient palette "seismic_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gery/tn/seismic.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 216 bytes of program space.

DEFINE_GRADIENT_PALETTE(seismic_gp){
    0, 255, 4, 2,
    40, 255, 4, 2,
    40, 255, 34, 26,
    61, 255, 34, 26,
    61, 255, 66, 54,
    75, 255, 66, 54,
    75, 255, 92, 80,
    85, 255, 92, 80,
    85, 255, 115, 103,
    92, 255, 115, 103,
    92, 255, 135, 123,
    98, 255, 135, 123,
    98, 255, 154, 144,
    103, 255, 154, 144,
    103, 255, 169, 160,
    108, 255, 169, 160,
    108, 255, 186, 178,
    112, 255, 186, 178,
    112, 255, 201, 194,
    116, 255, 201, 194,
    116, 255, 217, 212,
    120, 255, 217, 212,
    120, 255, 231, 228,
    123, 255, 231, 228,
    123, 255, 246, 245,
    127, 255, 246, 245,
    127, 255, 255, 255,
    127, 255, 255, 255,
    127, 247, 248, 255,
    130, 247, 248, 255,
    130, 229, 233, 255,
    134, 229, 233, 255,
    134, 210, 217, 255,
    138, 210, 217, 255,
    138, 194, 203, 255,
    142, 194, 203, 255,
    142, 177, 187, 255,
    146, 177, 187, 255,
    146, 159, 171, 255,
    150, 159, 171, 255,
    150, 140, 154, 255,
    156, 140, 154, 255,
    156, 121, 136, 255,
    162, 121, 136, 255,
    162, 100, 115, 255,
    169, 100, 115, 255,
    169, 77, 92, 255,
    179, 77, 92, 255,
    179, 50, 65, 255,
    193, 50, 65, 255,
    193, 23, 33, 255,
    214, 23, 33, 255,
    214, 2, 4, 255,
    255, 2, 4, 255};

// Gradient palette "textbook_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/fme/metres/tn/textbook.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 56 bytes of program space.

DEFINE_GRADIENT_PALETTE(textbook_gp){
    0, 5, 255, 6,
    8, 64, 255, 1,
    17, 135, 255, 0,
    25, 169, 193, 1,
    33, 210, 157, 2,
    42, 255, 125, 6,
    51, 255, 73, 21,
    59, 255, 53, 34,
    68, 184, 66, 67,
    76, 126, 81, 67,
    84, 82, 97, 106,
    93, 153, 166, 255,
    102, 199, 207, 255,
    255, 255, 255, 255};

// Gradient palette "autumnrose_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/rc/tn/autumnrose.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE(autumnrose_gp){
    0, 71, 3, 1,
    45, 128, 5, 2,
    84, 186, 11, 3,
    127, 215, 27, 8,
    153, 224, 69, 13,
    188, 229, 84, 6,
    226, 242, 135, 17,
    255, 247, 161, 79};

TProgmemRGBGradientPalette_bytes PaletteList[] = {
    GMT_sealand_gp,
    GMT_seafloor_gp,
    rainbow_gp,
    bhw1_07_gp,
    bhw1_01_gp,
    bhw1_03_gp,
    bath_112_gp,
    seismic_gp,
    textbook_gp,
    autumnrose_gp};

uint8_t u8GetGlobalPaltIdx(void)
{
  uint8_t PaletteIdx = 0;
  Preferences pref;
  pref.begin(PrefKey_PaletteSpace);
  PaletteIdx = pref.getUChar(PrefKey_PaletteIdx, 0);
  pref.end();
  return PaletteIdx;
}

bool boSetGlobalPaltIdx(uint8_t idx)
{
  bool res = true;
  Preferences pref;
  pref.begin(PrefKey_PaletteSpace);
  if (idx < MaxPaletteNum)
  {
    pref.putUChar(PrefKey_PaletteIdx, idx);
    ESP_LOGI(TAG, "MaxPaletteNum1 %d", MaxPaletteNum);
  }
  else
  {
    ESP_LOGI(TAG, "MaxPaletteNum2 %d", MaxPaletteNum);
    res = false;
  }
  pref.end();
  return res;
}

uint8_t u8GetGlobalColorIdx(void)
{
  uint8_t ColorIdx = 0;
  Preferences pref;
  pref.begin(PrefKey_PaletteSpace);
  ColorIdx = pref.getUChar(PrefKey_PaletteColIdx, 0);
  pref.end();
  return ColorIdx;
}

bool boSetGlobalColorIdx(uint8_t idx)
{
  bool res = true;
  Preferences pref;
  pref.begin(PrefKey_PaletteSpace);
  pref.putUChar(PrefKey_PaletteColIdx, idx);
  pref.end();
  return res;
}

CRGBPalette16 pGetPalette(uint8_t Index)
{
  CRGBPalette16 Palette(PaletteList[Index]);
  return Palette;
}