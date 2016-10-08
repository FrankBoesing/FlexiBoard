//Flexiboard ESP8266 - Flashing Tool
//(c) Frank Bösing, 2016

#include <FlexiBoard.h>

void setup(void) {

  //Switch off TFT-Backlight:
  setBacklight(0);

  pinMode(LED_BUILTIN, OUTPUT);
  initLEDs();
  SETLED(2, HIGH);

  if ( !resetWLAN() ) {
    Serial.println("ESP-12e not found!!!!");
    while (1);
  }

  while (!Serial) {
    ;
  }

  digitalWrite(LEDT, HIGH);
}


void loop() {

  int dtr = Serial.dtr();
  int rts = Serial.rts();

  SETLED(3, dtr);
  SETLED(0, rts);

#if 1
  //Implementation of "nodemcu" reset method:
  {
    if (dtr == rts) {
      digitalWriteFast(WLAN_GPIO0, HIGH);
      digitalWriteFast(WLAN_RESET, HIGH);
    }
    else if (rts) {
      digitalWriteFast(WLAN_GPIO0, HIGH);
      digitalWriteFast(WLAN_RESET, LOW);
    } else {
      digitalWriteFast(WLAN_GPIO0, LOW);
      digitalWriteFast(WLAN_RESET, HIGH);
    }
  }
#else
  //Implementation of "ck" reset method:
  {
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
