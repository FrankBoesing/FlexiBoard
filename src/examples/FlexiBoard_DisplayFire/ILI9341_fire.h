/* Fire ILI9341, Teensy 3.1
 * Copyright (c) 2015, Frank Bösing, f.boesing(at)gmx.de,
 * 29.8.2015
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "SPI.h"
#include "ILI9341_t3.h"

#ifndef fireILI9341_t3_h_
#define fireILI9341_t3_h_

static const int rows = 80;
static const int cols = 320 ;
static const int rowoffset = 239-rows*2;

//un-protect some functions:
class fireILI9341_t3: public ILI9341_t3
{
public:
  fireILI9341_t3(uint8_t _CS, uint8_t _DC, uint8_t _RST = 255, uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12): ILI9341_t3(_CS,_DC,_RST,_MOSI,_SCLK,_MISO) {init();}
void fire(void) {
     int x,y,i,r;
     for (y=1;y<rows-1;y++) {
      SPI.beginTransaction(SPISettings(72000000, MSBFIRST, SPI_MODE0));
    //setAddr(0, y+rowoffset, cols-1, y+rowoffset);
      setAddr(0, y*2+rowoffset, cols-1, y*2+rowoffset);
      writecommand_cont(ILI9341_RAMWR);
      for (x=0; x<cols-1; x++) {
          writedata16_cont(colorpal[screen[y][x]]);
          screen[y-1][x] =max((( (uint16_t)screen[y-1][x-1] + (uint16_t)screen[y-1][x] + (uint16_t)screen[y-1][x+1] +
                             (uint16_t)screen[y  ][x-1] +                          + (uint16_t)screen[y][x+1] +
                             (uint16_t)screen[y+1][x-1] + (uint16_t)screen[y+1][x] + (uint16_t)screen[y+1][x+1]) >> 3) - 1,0);
       }
        SPI.endTransaction();
      }

      //add some noise to last line
      for (x=0; x<cols-1; x++)
        screen[rows-1][x] = random(255);

      //add big hotspots
     
      r = random(40);
      for (i=0; i<r; i++) {
        x = random(cols-3)+1;
        screen[rows-4][x-1]=255;
        screen[rows-4][x]=255;
        screen[rows-4][x+1]=255;
        screen[rows-3][x-1]=255;
        screen[rows-3][x]=255;
        screen[rows-3][x+1]=255;
      }
     
  }
private:
  uint16_t colorpal[256];
  uint8_t screen[rows][cols];
  void init(){
      //clear screenbuf
      memset(screen, 0, sizeof(screen));
       //setup colorpalette
      int i;
      for (i=0;i<8;i++)
        colorpal[i] = color565( 0, 0, i * 8);
      for (i=8;i<16;i++)
        colorpal[i] = color565( (i-8)*11, 0, 128-i*8);
      for (i=16;i<32;i++)
        colorpal[i] = color565( (i-8)*11, 0, 0);
      for (i=32;i<56;i++)
        colorpal[i] = color565( 253, (i-32) * 11, 0);
      for (i=56;i<80;i++)
        colorpal[i] = color565( 253, 253, (i-56) * 11);
      for (i=80;i<256;i++)
        colorpal[i] = color565( 253, 253, 253);
    }
};
#endif
