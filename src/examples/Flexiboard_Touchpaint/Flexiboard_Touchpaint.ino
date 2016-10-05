/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341 Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

/* Adapted to Flexiboard and added LEDs, 2016, F.Bösing */



#include <SPI.h>
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>
#include <FlexiBoard.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

/*
  // The XPT2046 uses hardware SPI on the shield, and #8
  #define CS_PIN  16
  XPT2046_Touchscreen ts(CS_PIN);
  // The display also uses hardware SPI, plus #9 & #10
  #define TFT_CS 10
  #define TFT_DC 15
  ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
*/
XPT2046_Touchscreen ts(TFT_TOUCH_CS, TFT_TOUCH_INT);
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);


// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;


void initLEDs(void) {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
#endif
}

void ledColor(int color) {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
  digitalWrite(LED0, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  switch (color) {
    case ILI9341_RED: digitalWrite(LED0, HIGH); break;
    case ILI9341_YELLOW: digitalWrite(LED1, HIGH); break;
    case ILI9341_GREEN: digitalWrite(LED2, HIGH); break;
    case ILI9341_BLUE: digitalWrite(LED3, HIGH); break;
  }
#endif
}

void setup(void) {
  int t = millis();

  //Switch off ESP8266
  pinMode(WLAN_ENABLE, OUTPUT);
  digitalWrite(WLAN_ENABLE, LOW);

  SPI.setSCK(SCK);
  SPI.setMISO(MISO);
  SPI.setMOSI(MOSI);

  setBacklight(0.8);
  initLEDs();

  tft.begin();
  ts.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  while (!Serial || t - millis() < 1000) {
    ;
  }

  Serial.println("Touch Paint!");

  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
  tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
  tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
  tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
  tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);

  // select the current color 'red'
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
  currentcolor = ILI9341_RED;
  ledColor(currentcolor);
}


void loop()
{
  /*
    // See if there's any  touch data for us
    if (ts.bufferEmpty()) {
    return;
    }
  */

  // You can also wait for a touch
  if (! ts.touched()) {
    return;
  }


  // Retrieve a point
  TS_Point p = ts.getPoint();
  /*
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);

  */
  // Scale from ~0->4000 to tft.width using the calibration #'s
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

  /*
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.println(")");
  */

  if (p.y < BOXSIZE) {
    oldcolor = currentcolor;

    if (p.x < BOXSIZE) {
      currentcolor = ILI9341_RED;
      tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    } else if (p.x < BOXSIZE * 2) {
      currentcolor = ILI9341_YELLOW;
      tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    } else if (p.x < BOXSIZE * 3) {
      currentcolor = ILI9341_GREEN;
      tft.drawRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    } else if (p.x < BOXSIZE * 4) {
      currentcolor = ILI9341_CYAN;
      tft.drawRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    } else if (p.x < BOXSIZE * 5) {
      currentcolor = ILI9341_BLUE;
      tft.drawRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    } else if (p.x < BOXSIZE * 6) {
      currentcolor = ILI9341_MAGENTA;
      tft.drawRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    }

    if (oldcolor != currentcolor) {
      if (oldcolor == ILI9341_RED)
        tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
      if (oldcolor == ILI9341_YELLOW)
        tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
      if (oldcolor == ILI9341_GREEN)
        tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
      if (oldcolor == ILI9341_CYAN)
        tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
      if (oldcolor == ILI9341_BLUE)
        tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
      if (oldcolor == ILI9341_MAGENTA)
        tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
    }

    ledColor(currentcolor);
  }
  if (((p.y - PENRADIUS) > BOXSIZE) && ((p.y + PENRADIUS) < tft.height())) {
    tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
  }
}
