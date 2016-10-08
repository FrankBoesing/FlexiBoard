//Flexiboard ESP8266 - Talk To ESP8266 (AT Firmware)
//(c) Frank Bösing, 2016

#include <FlexiBoard.h>

void proxy() {
  int dtr = Serial.dtr();
  int rts = Serial.rts();

  SETLED(3, dtr);
  SETLED(0, rts);
  
  while (WLAN_SERIAL.available()) {
    Serial.write( WLAN_SERIAL.read() );
  }

  while (Serial.available()) {
    WLAN_SERIAL.write(Serial.read());
  }

}

void setup(void) {
  
  setBacklight(0);//Switch off TFT-Backlight
  pinMode(LED_BUILTIN, OUTPUT);    
  initLEDs();

  //Enable green LED as "PowerOn signal"
  SETLED(2, HIGH);

  //Wait for serial console
  int t = millis();
  while (!Serial && (millis() - t < 3000) ) {
    ;
  }
  //Enable LED on Teensy
  digitalWrite(LEDT, HIGH);
  
  if ( !resetWLAN() ) {
    Serial.println("ESP-12e not found!!!!");
    while (1);
  }

  WLAN_SERIAL.print("ATE1\r\n");
  delay(100);
  proxy();
  
  WLAN_SERIAL.print("AT+GMR\r\n");
}


void loop() {
  proxy();
}
