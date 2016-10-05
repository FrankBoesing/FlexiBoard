
// This Demo requires https://github.com/FrankBoesing/Arduino-Teensy-Codec-lib

// (c) Frank Bösing, 2016, f.boesing@gmx.de

//#define SOUND  0 //No Sound
#define SOUND 1 // Output via DAC
//#define SOUND 2 // Output via PT811

#include <SPI.h>

#if (SOUND>0)
#include <SD.h>
#include <Audio.h>
#include <play_sd_aac.h>
#include <SerialFlash.h>
#include "tetris_mp3.h"
#endif

#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>
#include <FlexiBoard.h>

#include "blocks.h"
#include "font_BlackOpsOne-Regular.h"
#include "font_DroidSans.h"


ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);
XPT2046_Touchscreen ts(TFT_TOUCH_CS, TFT_TOUCH_INT);

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 100
#define TS_MINY 100
#define TS_MAXX 3800
#define TS_MAXY 4000

#if (SOUND>0)
AudioPlaySdAac      playSnd;
#if (SOUND==1)
AudioOutputAnalog   sndOut;
#elif (SOUND==2)
AudioOutputPT8211      sndOut;
#else
#error undefined Sound-Output
#endif

AudioConnection     patchCord1(playSnd, 0, sndOut, 0);
AudioConnection     patchCord2(playSnd, 1, sndOut, 1);
#endif

#define SCREEN_BG   ILI9341_NAVY
#define SPEED_START 500
#define SPEED_MAX   200

uint16_t color_gamma[3][NUMCOLORS];
uint8_t  field[FIELD_WIDTH][FIELD_HIGHT];
uint16_t aSpeed, score, highscore;
int8_t   nBlock, nColor, nRotation; //next Block
int8_t   aBlock, aColor, aX, aY, aRotation; //active Block


void initGame();
void initField() ;
bool checkMoveBlock(int deltaX, int deltaY, int deltaRotation);
bool game(bool demoMode);
char controls();
void setBlock();
void checkLines();
void nextBlock();
void effect1();
void playSound(bool onoff);
uint16_t colgamma(int16_t color, int16_t gamma);
void printColorText(const char * txt, unsigned colorOffset);
void printStartGame();
void printGameOver();
void printNum(unsigned num);
void printScore();
void printHighScore();
void drawBlockPix(int px, int py, int col);
void drawBlockPixSmall(int px, int py, int col);
void drawBlock(int blocknum, int px, int py, int rotation, int col);
void drawBlockSmall(bool draw);
void drawBlockEx(int blocknum, int px, int py, int rotation, int col, int oldx, int oldy, int oldrotation);
void drawField();

void setup() {

  //Switch off ESP8266
  pinMode(WLAN_ENABLE, OUTPUT);
  digitalWrite(WLAN_ENABLE, LOW);

  SPI.setSCK(SCK);
  SPI.setMISO(MISO);
  SPI.setMOSI(MOSI);

#if (SOUND>0)
  AudioMemory(20);
#endif

  //color[0] is background, no gamma
  for (unsigned i = 1; i < NUMCOLORS; i++) {
    color_gamma[0][i] = colgamma(color[i], 30);
    color_gamma[1][i] = colgamma(color[i], -70);
    color_gamma[2][i] = colgamma(color[i], -35);
  }
  delay(800);

  Serial.println("--T3TRIS--");
  tft.begin();
  ts.begin();
  tft.setRotation(2);

  highscore = 0;
  nextBlock();

  tft.fillScreen(SCREEN_BG);
  tft.setCursor(10, 10);
  tft.setFont(BlackOpsOne_40);
  printColorText("T3TRIS", 1);

  tft.setFont(DroidSans_10);
  tft.setCursor(210, 5);
  tft.setTextColor(ILI9341_YELLOW);
  tft.print("F.B.");

  tft.setCursor(0, 100);
  tft.setFont(DroidSans_10);
  tft.setTextColor(ILI9341_GREEN);
  tft.print("Score");

  tft.setCursor(0, 180);
  tft.setFont(DroidSans_10);
  tft.setTextColor(ILI9341_YELLOW);
  tft.print("Hi-");
  tft.setCursor(0, 191);
  tft.print("Score");

  initGame();
  printScore();
  printHighScore();
}

void initGame() {
  score = 0;
  aSpeed = SPEED_START;
  initField();
}

void printColorText(const char * txt, unsigned colorOffset) {
  unsigned col = colorOffset;
  while (*txt) {
    tft.setTextColor(color[col]);
    tft.print(*txt);
    if (++col > NUMCOLORS - 1) col = 1;
    txt++;
  }
}

void initField() {
  memset(field, 0, sizeof(field));
  tft.fillRect( FIELD_X, FIELD_Y, FIELD_WIDTH * PIX, FIELD_HIGHT * PIX, color[0]);
}

void printNum(unsigned num) {
  if (num < 10000) tft.print("0");
  if (num < 1000) tft.print("0");
  if (num < 100) tft.print("0");
  if (num < 10) tft.print("0");
  tft.print(num);
}

void printScore() {
  tft.setFont(DroidSans_10);
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(3, 116);
#if (SOUND>0)
  AudioNoInterrupts();
#endif
  tft.fillRect( 3, 116, FIELD_X - 3, 10, SCREEN_BG);
  printNum(score);
#if (SOUND>0)
  AudioInterrupts();
#endif
}

void printHighScore() {
  tft.setFont(DroidSans_10);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(3, 208);
  tft.fillRect( 3, 208, FIELD_X - 3, 10, SCREEN_BG);
  printNum(highscore);
}

void printGameOver() {
  tft.setFont(BlackOpsOne_40);
  tft.fillRect( FIELD_X , 120, FIELD_WIDTH * PIX, 40, color[0]);
  tft.fillRect( FIELD_X , 170, FIELD_WIDTH * PIX, 40, color[0]);

  int t = millis();
  unsigned cofs = 1;
  do {
    tft.setCursor( FIELD_X + 10, 120);
    printColorText("GAME", cofs);
    tft.setCursor( FIELD_X + 20, 170);
    printColorText("OVER", cofs);
    if (++cofs > NUMCOLORS - 1) cofs = 1;
    delay(30);
  } while (millis() - t < 2000);
}

void printStartGame() {
  tft.setFont(BlackOpsOne_72);
  for (int i = 3; i > 0; i--) {
    for (int y = FIELD_Y + 20; y < FIELD_HIGHT * PIX - 32; y += 72) {
      tft.setCursor(FIELD_X + FIELD_WIDTH * PIX / 2 - 36, y);
      tft.setTextColor(color[i + 2]);
      tft.print(i);
      delay(150);
      tft.fillRect( FIELD_X + FIELD_WIDTH * PIX / 2 - 36, y, 72, 72, color[0]);
    }
  }
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor( FIELD_X + 30, 120);
  tft.print("GO");
  delay(400);
  initField();
}

void playSound(bool onoff) {
#if (SOUND>0)
  if (!playSnd.isPlaying() && onoff) playSnd.play(tetris_aac, tetris_aac_len);
  else if (playSnd.isPlaying() && !onoff) playSnd.stop();
#endif
}

void loop(void) {
  bool r = false;
  int c = 0;
  int c1 = 0;

  playSound(true);

  while (!r) {
    if (++c == 8) {
      effect1();
      c = 0;
    }
    if (++c1 == 50) {
      playSound(true);
      c1 = 0;
    }
    r = game(true);
  }
  game(false);
}


bool game(bool demoMode) {
  bool gameOver = false;
  int tk = 0;

  initGame();
  if (!demoMode) printStartGame();

  nextBlock();
  drawBlockSmall(true);
  drawBlock(aBlock, aX, aY, aRotation, aColor);

  do {
    //yield();
    if (!demoMode) playSound(true);

    int t = millis();

    if (!demoMode) do {  // process controls
        if (millis() - tk > aSpeed / 3) {
          char ch = controls();
          if (ch != '\0') tk = millis();
          switch (ch) {
            case 's' : //down
              t = 0;
              break;
            case '+' :  //rotate
              if (checkMoveBlock(0, 0, 1)) {
                int oldaRotation = aRotation;
                aRotation = (aRotation + 1) & 0x03;
                drawBlockEx(aBlock, aX, aY, aRotation, aColor, aX, aY, oldaRotation);
              }
              break;
            case 'a' : //left
            case 'd' : //right
              int dX = (ch == 'd') ? 1 : -1;
              if (checkMoveBlock(dX, 0, 0)) {
                drawBlockEx(aBlock, aX + dX, aY, aRotation, aColor, aX, aY, aRotation);
                aX += dX;
              }
              break;
          }
        }
        yield();
      } while ( millis() - t < aSpeed);   // process controls end

    else { //demoMode
      delay(5);
      char ch = controls();
      if (ch != '\0')  return true;
    }

    //move the block down
    bool movePossible = checkMoveBlock(0, 1, 0);
    if ( movePossible ) {
      drawBlockEx(aBlock, aX, aY + 1, aRotation, aColor, aX, aY, aRotation);
      aY++;
      score++;
    }

    else {
      //block stopped moving down
      //store location
      setBlock();
      checkLines();
      //get new block and draw it
      score += 10;
      nextBlock();
      drawBlockSmall(true);
      drawBlock(aBlock, aX, aY, aRotation, aColor);
      if (!checkMoveBlock(0, 0, 0)) {
        //no, game over !
        initField();
        gameOver = true;
      }
    }

    if (!demoMode) {
      printScore();
    }

  } while (!gameOver);

  if (!demoMode) {
    if (score > highscore) {
      highscore = score;
      printHighScore();
    }
    Serial.println();
    Serial.print("Score: ");
    Serial.println(score);
    playSound(false);
    printGameOver();
  }
  drawBlockSmall(false);
  return false;
}

char controls() {
  if (Serial.available()) {
    return (Serial.read());
  }
  if ( ts.bufferEmpty() ) return ('\0');

  TS_Point p = ts.getPoint();

#if 0
  tft.setFont(DroidSans_10);
  tft.setTextColor(ILI9341_GREEN);
  tft.fillRect( 0, 0, 40, 40, color[0]);
  tft.setCursor(3, 3);
  tft.print(p.x);
  tft.setCursor(3, 16);
  tft.print(p.y);
  tft.setCursor(3, 26);
  tft.print(p.z);
#endif

  if (p.z < 400) return ('\0');

  p.y = TS_MAXY - p.y;
  p.x = TS_MAXX - p.x;
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 3);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 3);


  if ((p.y < 1)  && (p.x > 1)) return ('+');
  if ((p.y < 1)  && (p.x < 1)) return ('d');
  if ((p.y >= 2) && (p.x < 1)) return ('a');
  if ((p.y >= 2) && (p.x > 1)) return ('s');

  return ('\0' );
}

void setBlock() {
  int bH = BLOCKHIGHT(aBlock, aRotation);
  int bW = BLOCKWIDTH(aBlock, aRotation);

  for (int y = 0; y < bH; y++) {
    for (int x = 0; x < bW; x++) {
      if ( (block[aRotation][aBlock][y * 4 + x + 2] > 0) ) {
        field[x + aX][y + aY] = aColor;
      }
    }
  }

}

void checkLines() {
  int x, y, c, i;
  for (y = 0; y < FIELD_HIGHT; y++) {
    c = 0;
    for (x = 0; x < FIELD_WIDTH; x++) {
      if (field[x][y] > 0) c++;
    }

    if ( c >= FIELD_WIDTH ) {//complete line !
      //line-effect:
      for (i = NUMCOLORS - 1; i >= 0; i--) {
        for (x = 0; x < FIELD_WIDTH; x++) {
          drawBlockPix(FIELD_X + x * PIX, FIELD_Y + y * PIX, i);
        }
        delay(60);
      }

      //move entire field above complete line down and redraw
      for (i = y; i > 0; i--)
        for (x = 0; x < FIELD_WIDTH; x++)
          field[x][i] = field[x][i - 1];
      for (x = 0; x < FIELD_WIDTH; x++)
        field[x][0] = 0;

      drawField();
      if (aSpeed > SPEED_MAX) aSpeed -= 5;
    }
  }
}

bool checkMoveBlock(int deltaX, int deltaY, int deltaRotation) {
  int rot = (aRotation + deltaRotation) & 0x03;
  int bH = BLOCKHIGHT(aBlock, rot);
  int dY = aY + deltaY;

  if (dY + bH > FIELD_HIGHT)  //lower border
    return false;

  int bW = BLOCKWIDTH(aBlock, rot);
  int dX = aX + deltaX;

  if (dX < 0 || dX + bW > FIELD_WIDTH) { //left/right border
    return false;
  }

  for (int y = bH - 1; y >= 0; y--) {
    for (int x = 0; x < bW; x++) {
      if ( (field[x + dX][y + dY] > 0) && (block[rot][aBlock][y * 4 + x + 2] > 0) ) {
        return false;
      }
    }
  }

  return true;
}

void nextBlock() {
  aColor = nColor;
  aBlock = nBlock;
  aRotation = nRotation;
  aY = 0;
  aX = random(FIELD_WIDTH - BLOCKWIDTH(aBlock, aRotation) + 1);

  nColor = random(1, NUMCOLORS);
  nBlock = random(NUMBLOCKS);
  nRotation = random(4);
}

void effect1() {
  int t = millis();
  do {
    nextBlock();
    drawBlock(aBlock, aX, random(FIELD_HIGHT), aRotation, aColor);
  } while (millis() - t < 1000);
}

uint16_t colgamma(int16_t color, int16_t gamma) {
  return  tft.color565(
            constrain(((color >> 8) & 0xf8) + gamma, 0, 255), //r
            constrain(((color >> 3) & 0xfc) + gamma, 0, 255), //g
            constrain(((color << 3) & 0xf8) + gamma, 0, 255)); //b
}

void drawBlockPix(int px, int py, int col) {

  if (px >= FIELD_XW) return;
  if (px < FIELD_X) return;
  if (py >= FIELD_YW) return;
  if (py < FIELD_Y) return;

  if (col == 0) {
    //remove Pix, draw backgroundcolor
    tft.fillRect(px, py, PIX, PIX, color[col]);
    return;
  }

  const int w = 4;

  tft.fillRect(px + w, py + w, PIX - w * 2 + 1, PIX - w * 2 + 1, color[col]);
  for (int i = 0; i < w; i++) {
    tft.drawFastHLine(px + i, py + i, PIX - 2 * i , color_gamma[0][col]);
    tft.drawFastHLine(px + i, PIX + py - i - 1 , PIX - 2 * i , color_gamma[1][col]);
    tft.drawFastVLine(px + i, py + i , PIX - 2 * i , color_gamma[2][col]);
    tft.drawFastVLine(px + PIX - i - 1, py + i , PIX - 2 * i , color_gamma[2][col]);
  }
}

inline void drawBlockPixSmall(int px, int py, int col) {

  const int w = 2;

  tft.fillRect(px + w, py + w, PIXSMALL - w * 2 + 1, PIXSMALL - w * 2 + 1, color[col]);
  for (int i = 0; i < w; i++) {
    tft.drawFastHLine(px + i, py + i, PIXSMALL - 2 * i , color_gamma[0][col]);
    tft.drawFastHLine(px + i, PIXSMALL + py - i - 1 , PIXSMALL - 2 * i , color_gamma[1][col]);
    tft.drawFastVLine(px + i, py + i , PIXSMALL - 2 * i , color_gamma[2][col]);
    tft.drawFastVLine(px + PIXSMALL - i - 1, py + i , PIXSMALL - 2 * i , color_gamma[2][col]);
  }
}


void drawBlock(int blocknum, int px, int py, int rotation, int col) {
  int w = BLOCKWIDTH(blocknum, rotation);
  int h = BLOCKHIGHT(blocknum, rotation);

  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      if (block[rotation][blocknum][y * 4 + x + 2])
        drawBlockPix(FIELD_X + px * PIX + x * PIX, FIELD_Y + py * PIX + y * PIX, col);
    }
  }
}

void drawBlockSmall(bool draw) {
  const int px = 3;
  const int py = 260;


  tft.fillRect(px, py, PIXSMALL * 4, PIXSMALL * 4, SCREEN_BG);

  if (draw) {
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        if (block[nRotation][nBlock][y * 4 + x + 2])
          drawBlockPixSmall(px + x * PIXSMALL, py + y * PIXSMALL, nColor);
      }
    }
  }
}

static uint8_t dbuf[FIELD_WIDTH][FIELD_HIGHT] = {0};
void drawBlockEx(int blocknum, int px, int py, int rotation, int col, int oldx, int oldy, int oldrotation) {

  int x, y;
  int w = BLOCKWIDTH(blocknum, oldrotation);
  int h = BLOCKHIGHT(blocknum, oldrotation);

  for (x = 0; x < w; x++)
    for (y = 0; y < h; y++)
      if (block[oldrotation][blocknum][y * 4 + x + 2] > 0) dbuf[x + oldx][y + oldy] = 2;

  w = BLOCKWIDTH(blocknum, rotation);
  h = BLOCKHIGHT(blocknum, rotation);
  for (x = 0; x < w; x++)
    for (y = 0; y < h; y++)
      if (block[rotation][blocknum][y * 4 + x + 2] > 0) dbuf[x + px][y + py] = 1;

#if (SOUND>0)
  AudioNoInterrupts();
#endif
  for (y = FIELD_HIGHT - 1; y >= oldy; y--)
    for (x = 0; x < FIELD_WIDTH; x++)
      switch (dbuf[x][y]) {
        case 1:  drawBlockPix(FIELD_X + x * PIX, FIELD_Y + y * PIX, col); dbuf[x][y] = 0; break;
        case 2:  tft.fillRect(FIELD_X + x * PIX, FIELD_Y + y * PIX, PIX, PIX, color[0]); dbuf[x][y] = 0; break;
      }
#if (SOUND>0)
  AudioInterrupts();
#endif
}

void drawField() {
  int x, y;
  for (y = FIELD_HIGHT - 1; y >= 0; y--)
    for (x = 0; x < FIELD_WIDTH; x++)
      drawBlockPix(FIELD_X + x * PIX, FIELD_Y + y * PIX, field[x][y]);
}

