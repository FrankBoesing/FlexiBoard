
 /* Fire ILI9341, Teensy 3.1..3.6
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

#include <SPI.h>
#include <ILI9341_t3.h>


#include "ILI9341_fire.h"
//#include "font_ChanceryItalic.h"
#include "font_Arial.h"
#include "font_ArialBold.h"
#include <FlexiBoard.h>

fireILI9341_t3 tft = fireILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

int frames = 0;  
int m;



void setup() {

    //Switch off ESP8266
  pinMode(WLAN_ENABLE, OUTPUT);
  digitalWrite(WLAN_ENABLE, LOW);

  SPI.setSCK(SCK);
  SPI.setMISO(MISO);
  SPI.setMOSI(MOSI);

  tft.begin(); 
  tft.setRotation(3);
 
  tft.fillScreen(ILI9341_BLACK);
#if 0 
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_GREENYELLOW);
  tft.println("Waiting for Arduino Serial Monitor...");
  tft.fillScreen(ILI9341_BLACK); 
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.print("Start..");
#endif  

#if 0
  tft.setFont(Arial_10);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(0,0);
  tft.print(F_BUS / 1e6);
  tft.print(" MHz");
#endif
  
//  tft.print("  FPS:");

#if 1 
  tft.setTextColor(ILI9341_PINK);
  //tft.setFont(Chancery_40_Italic);
  tft.setFont(Arial_16);
  tft.setCursor(80,60);
  tft.println("Teensy + ILI9341");
  tft.setCursor(80,120);
  tft.setFont(Arial_20_Bold);
  tft.println("FlexiBoard");  
  //tft.setFont(Chancery_20_Italic);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(20,180);
  tft.println("Greetings to all members @");
  tft.setCursor(60,200);
  tft.println("forum.pjrc.com");    
#endif 

  //add some Stars
  int i,y;
  for (i=0;i<200;i++) {
    y =  random()*200;
    tft.drawPixel(random()*cols, y, tft.color565(200-y,200-y,200-y));
  }
  m = micros();
}


void loop(void) { 
 tft.fire();
 /*
 frames++;
 if ( micros() - m >= 1000000) {    
    tft.setFont(Arial_9);
    tft.setCursor(110,0);
    tft.print(frames);    
    Serial.println(frames);
    frames = 0;
    m=micros();
 }
 */
}

