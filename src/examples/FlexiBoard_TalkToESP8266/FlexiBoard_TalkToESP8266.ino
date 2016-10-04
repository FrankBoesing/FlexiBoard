//Flexiboard ESP8266 - Talk to ESP with AT-Firmware
//(c) Frank Bösing, 2016

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
  digitalWriteFast(WLAN_GPIO0, HIGH);
  delay(200);
  //After that, we don't need it anymore:
  pinMode(WLAN_GPIO0, INPUT);

}


void loop() {

  if (WLAN_SERIAL.available()) {
    Serial.write( WLAN_SERIAL.read() );
  }

  if (Serial.available()) {
    WLAN_SERIAL.write(Serial.read());
  }

}
