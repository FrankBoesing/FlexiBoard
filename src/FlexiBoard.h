
#ifndef _FlexiBoard_h_
#define _FlexiBoard_h_


#define HAVE_ILI9341	1	// 0= not available / 1=available

#define HAVE_ESP12E		1	// 0= not available / 1=available
#define HAVE_RTL8710	0	// 0= not available / 1=available

#define HAVE_IR_SND	1	// 0= not available / 1=available
#define HAVE_IR_RCV	1	// 0= not available / 1=available

#define HAVE_MEM0	1	// 0= not available / 1=available
#define HAVE_MEM1	1	// 0= not available / 1=available
#define HAVE_MEM2	1	// 0= not available / 1=available
#define HAVE_MEM3	1	// 0= not available / 1=available
#define HAVE_MEM4	1	// 0= not available / 1=available

#define HAVE_LED0	1	// 0= not available / 1=available
#define HAVE_LED1	1	// 0= not available / 1=available
#define HAVE_LED2	1	// 0= not available / 1=available
#define HAVE_LED3	1	// 0= not available / 1=available


#define SCK  14
#define MISO 12
#define MOSI  7

#define LEDT LED_BUILTIN   //Teensy-LED

#if HAVE_LED0
#define LED0 29   //red
#else 
#define LED0 255
#endif

#if HAVE_LED1
#define LED1 30   //orange
#else
#define LED1 255
#endif

#if HAVE_LED2
#define LED2 31   //green
#else
#define LED2 255
#endif

#if HAVE_LED3
#define LED3 32   //blue
#else
#define LED3 255
#endif

const uint8_t ledArr[] = { LED0, LED1, LED2, LED3 };
#define SETLED(_led,_onoff) { if ( (_led < sizeof(ledArr) ) && ledArr[_led]<255) digitalWriteFast(ledArr[_led],_onoff); }

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
#if HAVE_ILI9341
  int b;
  
  b = 256 - roundf(256.0f * brightness);
  
  if (b < 0) b = 0;
  if (b > 256) b = 256;
  pinMode(TFT_BACKLIGHT, OUTPUT);
  analogWrite(TFT_BACKLIGHT, b); //(0=Max, 256=MIN)
#endif  
}

bool resetWLAN() {
#if HAVE_ESP12E		
  WLAN_SERIAL.begin(WLAN_SERIAL_BAUD);

  pinMode(WLAN_RESET, OUTPUT);
  pinMode(WLAN_ENABLE, OUTPUT);
  pinMode(WLAN_CTS, OUTPUT);
  pinMode(WLAN_GPIO0, OUTPUT);

  digitalWrite(WLAN_CTS, LOW);
  digitalWrite(WLAN_ENABLE, LOW);


  //Reset ESP
  digitalWrite(WLAN_RESET, LOW);
  delay(15);
  digitalWrite(WLAN_ENABLE, HIGH);
  digitalWrite(WLAN_RESET, HIGH);

  //The ESP - Bootloader waits for  HIGH on WLAN_GPIO0
  digitalWrite(WLAN_GPIO0, HIGH);
  
  bool ret = WLAN_SERIAL.findUntil((char*)"ready",(char*)"\n\r");
  
  //GPIO0 ist not needed anymore:
  pinMode(WLAN_GPIO0, INPUT);
  
  //Disable all used pins if no response from ESP
  if (!ret) {
    pinMode(WLAN_RESET, INPUT);
    pinMode(WLAN_ENABLE, INPUT);
    pinMode(WLAN_CTS, INPUT);
  }
  
  return ret;
#endif  
}



void initLEDs(void) {  
  if (LED0<255) pinMode(LED0, OUTPUT);
  if (LED1<255) pinMode(LED1, OUTPUT);
  if (LED2<255) pinMode(LED2, OUTPUT);
  if (LED3<255) pinMode(LED3, OUTPUT);
}

#endif