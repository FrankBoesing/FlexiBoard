/*
 GNU LESSER GENERAL PUBLIC LICENSE
 See attached license.md
 
 (c) Frank Bösing, 2016
 
*/
#include <Audio.h>
#include <play_mp3queue.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <HardwareSerial.h>
#include "audioutil.h"
#include <FlexiBoard.h>


#if !HAVE_RAM
//Use Teeensy-RAM (limited buffering)
#include "FIFO.h"
#define FIFO_BUFFER (FIFO_SIZE)
Fifo fifo;
 
//USE SPI-RAM
#else
#define FIFO_BUFFER (RAM_SIZE_TOTAL)
#include "SRAM_FIFO.h"
SRAMFifo fifo;
#endif

AudioPlayMP3Queue        playMP3_1;     //xy=111,349
#if HAVE_PT8211
AudioOutputPT8211        output;           //xy=452,356
#else
AudioOutputAnalogStereo  output;           //xy=452,356
#endif
AudioConnection          patchCord1(playMP3_1, 0, output, 0);
AudioConnection          patchCord2(playMP3_1, 1, output, 1);

#define SERVER "mp3.ffh.de"
#define STREAM "/radioffh/hqlivestream.mp3"
#define SERVERPORT   80

#define CONNECTION_TIMEOUT 4000
#define WLAN_BAUD (115200*20)
#define WLAN_FLOWCONTROL 3 //0:disable, 1:enable RTS, 2:enable CTS, 3:both

const size_t buffersize = 512;
uint8_t buffer[buffersize];

struct streamData {
  char description[128];
  char genre[128];
  char name[128];
  char url[128];
  int samplerate;
} streamData = {{0}, {0}, {0}, {0}, 0};

const char icy_description[] = "icy-description:";
const char icy_genre[] = "icy-genre:";
const char icy_name[] = "icy-name:";
const char icy_url[] = "icy-url:";

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void yield(void) {} //Empty yield to save some RAM(and ROM)

class Wlan : public HardwareSerial {
  public:
    virtual int read(void) {
      int ch = serial_getchar();
      if (ch >= 0 && echo) Serial.write(ch);
      return ch;
    }
    int echo;
};

Wlan wlan;
int streamStarted;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void openStream(char* server, int port, char *stream ) {
  streamStarted = 0;
  fifo.reset();
  playMP3_1.reset();
  memset((void*)&streamData, 0, sizeof(streamData));
  wlan.setTimeout(3000);
  wlan.printf("AT+CIPSTART=1,\"TCP\",\"%s\",%d,%d\r\n", SERVER, SERVERPORT, CONNECTION_TIMEOUT);
  wlan.findUntil((char*)"OK", (char*)"");

  char request[512] = {0};
  strcat(request, "GET ");
  strcat(request, stream);
  strcat(request, " HTTP/1.0\r\n HOST:");
  strcat(request, server); 
  strcat(request, "\r\nUser-Agent:Teensyradio\r\nicy-metadata:0\r\nicy-reset:1\r\nicy-br:128\r\n\r\n");

  wlan.printf("AT+CIPSEND=1,%d\r\n", strlen(request));
  wlan.findUntil((char*)">", (char*)"");
  wlan.print(request);
  wlan.echo = 0;  
  unsigned k = 0;
  while (fifo.used() < FIFO_BUFFER - 2048) {
    if (k / 32768 != fifo.used() / 32768) {
      k = fifo.used();
      Serial.printf("%dKB..", k / 1024);
    }
    readStream(); //fill Buffer before start playing    
  }  
  Serial.println();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define SMETA(icytag,sdata) if (strncmp(icytag, linebuf, strlen(icytag)) == 0) \
                                  { strncpy(sdata, linebuf + strlen(icytag), sizeof(sdata)-1); }

void readStream(void) {
  const size_t mp3bufsize = 256;
  static char linebuf[256] = {0};
  static unsigned lbi = 0;
  static int state = 0;
  static unsigned cntData = 0;  
  static unsigned mp3bufidx = 0;
  static uint8_t mp3buf[mp3bufsize];
  
  char ch;

  //Parse Response Header
  if (state == 900) {
    while ((cntData > 0) && (wlan.available() > 0) ) {
      cntData--;
      ch =  wlan.read();
      if (ch != '\r' && ch != '\n' ) linebuf[lbi++] = ch;
      if (ch == '\n') {
        linebuf[lbi] = 0;
        Serial.println(linebuf);
        
        SMETA(icy_description, streamData.description)
        else SMETA(icy_genre, streamData.genre)
        else SMETA(icy_name, streamData.name)
        else SMETA(icy_url, streamData.url)
//        else if (strncmp(icy_metaint, linebuf, icy_metaint_len) == 0) streamData.metaint = strtol(linebuf + icy_metaint_len, NULL, 10);
        else if (lbi < 2) {
          lbi = 0;          
          streamStarted = 1;
          state = 901;
          Serial.println("Buffering...");
          break;
        }
        lbi = 0;
      }
      else if (lbi >= (sizeof(linebuf) - 2) ) lbi = sizeof(linebuf) - 2;
    }
    if (cntData == 0) state = 0;
    return;
  }

  //Fill FIFO with a new packet
  if ( state == 901 ) {
    SETLED(0, 1);
    while ((cntData > 0) && (wlan.available() ) && (fifo.available() >= mp3bufsize) ) {      
      cntData--;
      mp3buf[mp3bufidx++] = wlan.read();
      if (mp3bufidx >= mp3bufsize) {
        mp3bufidx = 0;    
        fifo.write(mp3buf, mp3bufsize);
      }
    }  
    if (cntData == 0) state = 0;
    SETLED(0, 0);
    return;
  }

  //Parse response to AT Command
  if ( wlan.available() ) {
    ch = wlan.read();
    switch (state) {
      //+IPD,1,1420:
      case 0: if (ch == '+') state++; break;
      case 1: if (ch == 'I') state++; break;
      case 2: if (ch == 'P') state++; break;
      case 3: if (ch == 'D') state++; break;
      case 4: if (ch == ',') state++; break;
      case 5: if (ch == '1') state++; break;
      case 6: if (ch == ',') state++; cntData = 0; break;
      case 7: if (ch == ':') state = 900 | streamStarted;
        else cntData = cntData * 10 + (ch - '0');
        break;
      default:  state = 0;
    }
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void setup() {
  AudioMemory(10);
  Serial.begin(115200); 
  wlan.echo = 1;

  initLEDs();
  setBacklight(0);

  //Wait for serial console
  int t = millis();
  while (!Serial && (millis() - t < 3000) ) {}

  for (int i = 0; i < 3; i++) {
    SETLED(2, LOW);
    delay(40);
    SETLED(2, HIGH);
    delay(40);
  }
  delay(100);

  if ( !resetWLAN() ) while (1) {
      SETLED(0, HIGH);
      Serial.println("ESP-12e not found!!!!");
      delay(500);
    }

  Serial.println();
  Serial.println("ESP-12e initialized.");
  wlan.setTimeout(8000);
  int __attribute__((unused)) gotIP = wlan.findUntil((char*)"WIFI GOT IP", (char*)"") ;

  wlan.printf("AT+UART_CUR=%d,8,1,0,%d\r\n", WLAN_BAUD, WLAN_FLOWCONTROL);
  if ( wlan.findUntil((char*)"OK", (char*)"") ) {
    delay(30);
    wlan.begin(WLAN_BAUD);
    if (WLAN_FLOWCONTROL & 1) wlan.attachRts(WLAN_RTS);
    if (WLAN_FLOWCONTROL & 2) wlan.attachCts(WLAN_CTS);
    NVIC_SET_PRIORITY(IRQ_UART0_STATUS, 16);
  } else {
    Serial.println("Switching Baudrate not successful");
    while (1);
  }

  wlan.setTimeout(1000);
  wlan.println("AT+CIPMUX=1");
  wlan.findUntil((char*)"OK", (char*)"");

  openStream((char*)SERVER, SERVERPORT, (char*)STREAM );
  Serial.printf("Name:%s\r\n", streamData.name);
  Serial.printf("URL:%s\r\n", streamData.url);  
     
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void loop() {
  static unsigned long time = millis();
  static int len = 0;

  readStream();

  if (len > 0) {
    int ret = playMP3_1.pushData(buffer, &len);
    if (ret < -1) { /* Serial.printf("TEMPERROR %d\r\n", ret); */ } 
    else if (streamData.samplerate==0 && playMP3_1.samplerate()>0) {
      streamData.samplerate = playMP3_1.samplerate();
#if HAVE_PT8211     
      setI2SFreq(streamData.samplerate);
#else
      setDACFreq(streamData.samplerate);     
#endif      
      Serial.printf("Samplerate:%d\r\n",streamData.samplerate);
    }
  }
  else {
    len = fifo.read(buffer, buffersize);
    if (len <= 0) {
      SETLED(0, 1);
      Serial.println("EOF");
      while (1); // End Of File
    }
  }

  if (millis() - time > 1000) {
    time = millis();
    Serial.printf("Buffer:%d KB\r\n", fifo.used() / 1024);
  }
}

