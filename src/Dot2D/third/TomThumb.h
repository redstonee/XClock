/**
** The original 3x5 font is licensed under the 3-clause BSD license:
**
** Copyright 1999 Brian J. Swetland
** Copyright 1999 Vassilii Khachaturov
** Portions (of vt100.c/vt100.h) copyright Dan Marks
**
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions, and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions, and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the authors may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** Modifications to Tom Thumb for improved readability are from Robey Pointer,
** see:
** http://robey.lag.net/2010/01/23/tiny-monospace-font.html
**
** The original author does not have any objection to relicensing of Robey
** Pointer's modifications (in this file) in a more permissive license.  See
** the discussion at the above blog, and also here:
** http://opengameart.org/forumtopic/how-to-submit-art-using-the-3-clause-bsd-license
**
** Feb 21, 2016: Conversion from Linux BDF --> Adafruit GFX font,
** with the help of this Python script:
** https://gist.github.com/skelliam/322d421f028545f16f6d
** William Skellenger (williamj@skellenger.net)
** Twitter: @skelliam
**
** Jan 09, 2020: Bitmaps now compressed, to fix the bounding box problem,
** because non-compressed the calculated text width were wrong.
** Andreas Merkle (web@blue-andi.de)
*/

#ifdef __AVR__
 #include <avr/io.h>
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#else
 #define PROGMEM
#endif

const uint8_t TomThumbBitmaps[] PROGMEM = {
  0x00, 0xE8, 0xB4, 0xBE, 0xFA, 0x79, 0xE4, 0x85, 0x42, 0xDB, 0xD6, 0xC0, 
  0x6A, 0x40, 0x95, 0x80, 0xAA, 0x80, 0x5D, 0x00, 0x60, 0xE0, 0x80, 0x25, 
  0x48, 0x00, 0xF6, 0xDE, 0x00, 0x75, 0x40, 0xE7, 0xCE, 0x00, 0xE7, 0x9E, 
  0x00, 0xB7, 0x92, 0xF3, 0x9E, 0x00, 0xF3, 0xDE, 0x00, 0xE4, 0x92, 0x00, 
  0xF7, 0xDE, 0xF7, 0x9E, 0x00, 0xA0, 0x46, 0x2A, 0x22, 0xE3, 0x80, 0x88, 
  0xA8, 0xE5, 0x04, 0x57, 0xC6, 0x00, 0x57, 0xDA, 0xD7, 0x5C, 0x00, 0x72, 
  0x46, 0xD6, 0xDC, 0xF3, 0xCE, 0xF3, 0xC8, 0x73, 0xD6, 0x00, 0xB7, 0xDA, 
  0xE9, 0x2E, 0x24, 0xD4, 0xB7, 0x5A, 0x92, 0x4E, 0xBF, 0xDA, 0x00, 0xBF, 
  0xFA, 0x00, 0x56, 0xD4, 0xD7, 0x48, 0x56, 0xF6, 0xD7, 0xEA, 0x71, 0x1C, 
  0xE9, 0x24, 0xB6, 0xD6, 0xB6, 0xA4, 0xB7, 0xFA, 0xB5, 0x5A, 0xB5, 0x24, 
  0xE5, 0x4E, 0xF2, 0x4E, 0x88, 0x80, 0xE4, 0x9E, 0x54, 0xE0, 0x90, 0xCE, 
  0xF0, 0x9A, 0xDC, 0x72, 0x30, 0x2E, 0xD6, 0x77, 0x30, 0x2B, 0xA4, 0x00, 
  0x77, 0x9C, 0x00, 0x9A, 0xDA, 0xB8, 0x20, 0x9A, 0x80, 0x97, 0x6A, 0xC9, 
  0x2E, 0xFF, 0xD0, 0xD6, 0xD0, 0x56, 0xA0, 0xD6, 0xE8, 0x76, 0xB2, 0x72, 
  0x40, 0x79, 0xE0, 0x5D, 0x26, 0xB6, 0xB0, 0xB7, 0xA0, 0xBF, 0xF0, 0xA9, 
  0x50, 0xB5, 0x94, 0xEF, 0x70, 0x6A, 0x26, 0xD8, 0xC8, 0xAC, 0x78
};

const GFXglyph TomThumbGlyphs[] PROGMEM = {
  {     0,   1,   1,   2,    0,   -5 },   // 0x20 ' '
  {     1,   1,   5,   2,    0,   -5 },   // 0x21 '!'
  {     2,   3,   2,   4,    0,   -5 },   // 0x22 '"'
  {     3,   3,   5,   4,    0,   -5 },   // 0x23 '#'
  {     5,   3,   5,   4,    0,   -5 },   // 0x24 '$'
  {     7,   3,   5,   4,    0,   -5 },   // 0x25 '%'
  {     9,   3,   5,   4,    0,   -5 },   // 0x26 '&'
  {    11,   1,   2,   2,    0,   -5 },   // 0x27 '''
  {    12,   2,   5,   3,    0,   -5 },   // 0x28 '('
  {    14,   2,   5,   3,    0,   -5 },   // 0x29 ')'
  {    16,   3,   3,   4,    0,   -5 },   // 0x2A '*'
  {    18,   3,   3,   4,    0,   -4 },   // 0x2B '+'
  {    20,   2,   2,   3,    0,   -2 },   // 0x2C ','
  {    21,   3,   1,   4,    0,   -3 },   // 0x2D '-'
  {    22,   1,   1,   2,    0,   -1 },   // 0x2E '.'
  {    23,   3,   5,   4,    0,   -5 },   // 0x2F '/'
  {    26,   3,   5,   4,    0,   -5 },   // 0x30 '0'
  {    29,   2,   5,   3,    0,   -5 },   // 0x31 '1'
  {    31,   3,   5,   4,    0,   -5 },   // 0x32 '2'
  {    34,   3,   5,   4,    0,   -5 },   // 0x33 '3'
  {    37,   3,   5,   4,    0,   -5 },   // 0x34 '4'
  {    39,   3,   5,   4,    0,   -5 },   // 0x35 '5'
  {    42,   3,   5,   4,    0,   -5 },   // 0x36 '6'
  {    45,   3,   5,   4,    0,   -5 },   // 0x37 '7'
  {    48,   3,   5,   4,    0,   -5 },   // 0x38 '8'
  {    50,   3,   5,   4,    0,   -5 },   // 0x39 '9'
  {    53,   1,   3,   2,    0,   -4 },   // 0x3A ':'
  {    54,   2,   4,   3,    0,   -4 },   // 0x3B ';'
  {    55,   3,   5,   4,    0,   -5 },   // 0x3C '<'
  {    57,   3,   3,   4,    0,   -4 },   // 0x3D '='
  {    59,   3,   5,   4,    0,   -5 },   // 0x3E '>'
  {    61,   3,   5,   4,    0,   -5 },   // 0x3F '?'
  {    63,   3,   5,   4,    0,   -5 },   // 0x40 '@'
  {    66,   3,   5,   4,    0,   -5 },   // 0x41 'A'
  {    68,   3,   5,   4,    0,   -5 },   // 0x42 'B'
  {    71,   3,   5,   4,    0,   -5 },   // 0x43 'C'
  {    73,   3,   5,   4,    0,   -5 },   // 0x44 'D'
  {    75,   3,   5,   4,    0,   -5 },   // 0x45 'E'
  {    77,   3,   5,   4,    0,   -5 },   // 0x46 'F'
  {    79,   3,   5,   4,    0,   -5 },   // 0x47 'G'
  {    82,   3,   5,   4,    0,   -5 },   // 0x48 'H'
  {    84,   3,   5,   4,    0,   -5 },   // 0x49 'I'
  {    86,   3,   5,   4,    0,   -5 },   // 0x4A 'J'
  {    88,   3,   5,   4,    0,   -5 },   // 0x4B 'K'
  {    90,   3,   5,   4,    0,   -5 },   // 0x4C 'L'
  {    92,   3,   5,   4,    0,   -5 },   // 0x4D 'M'
  {    95,   3,   5,   4,    0,   -5 },   // 0x4E 'N'
  {    98,   3,   5,   4,    0,   -5 },   // 0x4F 'O'
  {   100,   3,   5,   4,    0,   -5 },   // 0x50 'P'
  {   102,   3,   5,   4,    0,   -5 },   // 0x51 'Q'
  {   104,   3,   5,   4,    0,   -5 },   // 0x52 'R'
  {   106,   3,   5,   4,    0,   -5 },   // 0x53 'S'
  {   108,   3,   5,   4,    0,   -5 },   // 0x54 'T'
  {   110,   3,   5,   4,    0,   -5 },   // 0x55 'U'
  {   112,   3,   5,   4,    0,   -5 },   // 0x56 'V'
  {   114,   3,   5,   4,    0,   -5 },   // 0x57 'W'
  {   116,   3,   5,   4,    0,   -5 },   // 0x58 'X'
  {   118,   3,   5,   4,    0,   -5 },   // 0x59 'Y'
  {   120,   3,   5,   4,    0,   -5 },   // 0x5A 'Z'
  {   122,   3,   5,   4,    0,   -5 },   // 0x5B '['
  {   124,   3,   3,   4,    0,   -4 },   // 0x5C '\'
  {   126,   3,   5,   4,    0,   -5 },   // 0x5D ']'
  {   128,   3,   2,   4,    0,   -5 },   // 0x5E '^'
  {   129,   3,   1,   4,    0,   -1 },   // 0x5F '_'
  {   130,   2,   2,   3,    0,   -5 },   // 0x60 '`'
  {   131,   3,   4,   4,    0,   -4 },   // 0x61 'a'
  {   133,   3,   5,   4,    0,   -5 },   // 0x62 'b'
  {   135,   3,   4,   4,    0,   -4 },   // 0x63 'c'
  {   137,   3,   5,   4,    0,   -5 },   // 0x64 'd'
  {   139,   3,   4,   4,    0,   -4 },   // 0x65 'e'
  {   141,   3,   5,   4,    0,   -5 },   // 0x66 'f'
  {   144,   3,   5,   4,    0,   -4 },   // 0x67 'g'
  {   147,   3,   5,   4,    0,   -5 },   // 0x68 'h'
  {   149,   1,   5,   2,    0,   -5 },   // 0x69 'i'
  {   150,   3,   6,   4,    0,   -5 },   // 0x6A 'j'
  {   153,   3,   5,   4,    0,   -5 },   // 0x6B 'k'
  {   155,   3,   5,   4,    0,   -5 },   // 0x6C 'l'
  {   157,   3,   4,   4,    0,   -4 },   // 0x6D 'm'
  {   159,   3,   4,   4,    0,   -4 },   // 0x6E 'n'
  {   161,   3,   4,   4,    0,   -4 },   // 0x6F 'o'
  {   163,   3,   5,   4,    0,   -4 },   // 0x70 'p'
  {   165,   3,   5,   4,    0,   -4 },   // 0x71 'q'
  {   167,   3,   4,   4,    0,   -4 },   // 0x72 'r'
  {   169,   3,   4,   4,    0,   -4 },   // 0x73 's'
  {   171,   3,   5,   4,    0,   -5 },   // 0x74 't'
  {   173,   3,   4,   4,    0,   -4 },   // 0x75 'u'
  {   175,   3,   4,   4,    0,   -4 },   // 0x76 'v'
  {   177,   3,   4,   4,    0,   -4 },   // 0x77 'w'
  {   179,   3,   4,   4,    0,   -4 },   // 0x78 'x'
  {   181,   3,   5,   4,    0,   -4 },   // 0x79 'y'
  {   183,   3,   4,   4,    0,   -4 },   // 0x7A 'z'
  {   185,   3,   5,   4,    0,   -5 },   // 0x7B '{'
  {   187,   1,   5,   2,    0,   -5 },   // 0x7C '|'
  {   188,   3,   5,   4,    0,   -5 },   // 0x7D '}'
  {   190,   3,   2,   4,    0,   -5 }    // 0x7E '~'
};

const GFXfont TomThumb PROGMEM = {(uint8_t *)TomThumbBitmaps, 
                                  (GFXglyph *)TomThumbGlyphs, 0x20, 0x7E,  6};