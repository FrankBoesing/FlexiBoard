
#ifndef _FlexiBoard_h_
#define _FlexiBoard_h_

#define SCK  14
#define MISO 12
#define MOSI  7

#define LEDT 13   //Teensy-LED
#define LED0 29   //red
#define LED1 30   //orange
#define LED2 31   //green
#define LED3 32   //blue

const uint8_t ledArr[4] = { LED0, LED1, LED2, LED3 };
#define SETLED(_led,_onoff) {digitalWriteFast(ledArr[_led],_onoff);}

#define TFT_BACKLIGHT   4
#define TFT_TOUCH_CS    8
#define TFT_TOUCH_INT   3
#define TFT_DC          21
#define TFT_CS          15
#define TFT_RST         255  // 255 = unused, connected to 3.3V
#define TFT_SCLK        SCK
#define TFT_MOSI        MOSI
#define TFT_MISO        MISO

#define MEM0_CS         6
#define MEM1_CS         25
#define MEM2_CS         26
#define MEM3_CS         27
#define MEM4_CS         28

#define IR_RCV          17
#define IR_SND          5  //Shared with as WLAN_GPIO0 !

#define WLAN_SERIAL     Serial1
#define WLAN_SERIAL_BAUD_INIT 74880
#define WLAN_SERIAL_BAUD 115200
#define WLAN_RESET      24
#define WLAN_ENABLE     16  //CH_PD
#define WLAN_CTS        20  //GPIO15
#define WLAN_RTS        2   //GPIO13
#define WLAN_GPIO0      5   //Shared with IR_SND !
#define WLAN_GPIO13     WLAN_RTS
#define WLAN_GPIO15     WLAN_CTS


void setBacklight(float brightness) { // ( range from 0 to 1.0 ) 
  int b;
  
  b = 256 - roundf(256.0f * brightness);
  
  if (b < 0) b = 0;
  if (b > 256) b = 256;
  
  analogWrite(TFT_BACKLIGHT, b); //(0=Max, 256=MIN)
}


#endif