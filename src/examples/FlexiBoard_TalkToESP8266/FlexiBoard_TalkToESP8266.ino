//Flexiboard ESP8266 - Talk To ESP8266 (AT Firmware)
//(c) Frank Bösing, 2016

#include <SPI.h>
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>
#include <FlexiBoard.h>


void proxy() {
  
  while (WLAN_SERIAL.available()) {
    Serial.write( WLAN_SERIAL.read() );
  }

  while (Serial.available()) {
    WLAN_SERIAL.write(Serial.read());
  }
  
}

void setup(void) {
  WLAN_SERIAL.begin(WLAN_SERIAL_BAUD);
  pinMode(WLAN_RESET, OUTPUT);
  pinMode(WLAN_ENABLE, OUTPUT);
  pinMode(WLAN_CTS, OUTPUT);
  pinMode(WLAN_GPIO0, OUTPUT);

  digitalWrite(WLAN_CTS, LOW);
  digitalWrite(WLAN_ENABLE, LOW);

  int t = millis();
  while (!Serial || t - millis() < 1000) {
    ;
  }

  //Reset ESP
  digitalWrite(WLAN_RESET, LOW);
  delay(20);
  digitalWrite(WLAN_ENABLE, HIGH);
  digitalWrite(WLAN_RESET, HIGH);

  //The ESP - Bootloader waits for  HIGH on WLAN_GPIO0
  digitalWrite(WLAN_GPIO0, HIGH);
  delay(200);
  //After that, we don't need it anymore:
  pinMode(WLAN_GPIO0, INPUT);


  delay(250);
  WLAN_SERIAL.printf("ATE1\r\n");
  delay(300);
  WLAN_SERIAL.printf("AT+GMR\r\n");

  t=millis();
  while (t - millis() < 1500) {
    proxy();
  }

  
  WLAN_SERIAL.printf("AT+CWLAP\r\n");  
  
}


void loop() {
  proxy();
}
