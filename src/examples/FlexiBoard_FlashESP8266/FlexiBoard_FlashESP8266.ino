//Flexiboard ESP8266 - Flashing Tool
//(c) Frank Bösing, 2016

#include <FlexiBoard.h>

void setup(void) {

  //Switch off TFT-Backlight:
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWriteFast(TFT_BACKLIGHT, HIGH);
  
  pinMode(LEDT, OUTPUT);
  pinMode(LED0, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWriteFast(LED2, HIGH);

  WLAN_SERIAL.begin(115200 * 8);
  pinMode(WLAN_RESET, OUTPUT);
  pinMode(WLAN_ENABLE, OUTPUT);
  pinMode(WLAN_GPIO0, OUTPUT);
  pinMode(WLAN_GPIO15, OUTPUT);

  digitalWriteFast(WLAN_GPIO15, LOW);
  digitalWriteFast(WLAN_ENABLE, HIGH);
  digitalWriteFast(WLAN_GPIO0, HIGH);
  
  //Start in Run Mode
  digitalWriteFast(WLAN_RESET,  LOW);
  delay(1);
  digitalWriteFast(WLAN_RESET, HIGH);
    
  while (!Serial) {
    ;
  }

  digitalWrite(LEDT, HIGH);
}


void loop() {

#if 1
  //Implementation of "nodemcu" reset method:
  {
    int dtr = Serial.dtr();
    int rts = Serial.rts();

    digitalWriteFast(LED3, dtr);
    digitalWriteFast(LED0, rts);

    if (dtr == rts) {
      digitalWriteFast(WLAN_GPIO0, HIGH);
      digitalWriteFast(WLAN_RESET, HIGH);
    }
    else if (rts) {
      digitalWriteFast(WLAN_GPIO0,HIGH);
      digitalWriteFast(WLAN_RESET, LOW);
    } else {
      digitalWriteFast(WLAN_GPIO0,LOW);
      digitalWriteFast(WLAN_RESET, HIGH);
    }
  }
#else
  //Implementation of "ck" reset method:
  {
    int dtr = Serial.dtr();
    int rts = Serial.rts();

    digitalWriteFast(LED3, dtr);
    digitalWriteFast(LED0, rts);

    //Connect DTR to GPIO0
    digitalWriteFast(WLAN_GPIO0, dtr);
    //Connect RTS to RESET:
    digitalWriteFast(WLAN_RESET, !rts );
  }
#endif

  //Data Transfer
  while (Serial.available())
    WLAN_SERIAL.write(Serial.read());
  
  while (WLAN_SERIAL.available())
    Serial.write( WLAN_SERIAL.read() );
  
}
