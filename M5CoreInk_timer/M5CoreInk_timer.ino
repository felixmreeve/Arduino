/*
  POMODORO timer based on
  https://github.com/simonprickett/m5stack-pomodoro
//*/

#include "M5CoreInk.h"

#include "icon.h"

enum state {
  STATE_MENU,
  STATE_WORKING,
  STATE_BREAK
};

unsigned int POMODORO_MINS = 25;
unsigned int SHORT_BREAK_MINS = 5;
unsigned int LONG_BREAK_MINS = 15;
unsigned long MINUTE_MILLIS = 60000;
unsigned long startTime = 0;
unsigned long timePassed = 0;
unsigned short iteration = 0;
unsigned short minutesPassed = 0;
unsigned short minutesTarget = 0;
state currentState = STATE_MENU;

// create a global sprite object for screen functionality
Ink_Sprite InkPageSprite(&M5.M5Ink);

int strLen(const char *charData) {
  const char *charOffset = charData;
  while (*charOffset != '\0') {
    charOffset++;
  }
  return charOffset - charData;
}

void drawImage(int posX, int posY,
               int width, int height,
               uint8_t *imageDataptr) {
    int _posX = posX + height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t mark = 0x80 >> (y * width + x) % 8;
            if ((imageDataptr[(y * width + x) / 8]) & mark) {
                InkPageSprite.drawPix(_posX - y, posY + x, 1);
            } else {
                InkPageSprite.drawPix(_posX - y, posY + x, 0);
            }
        }
    }
}

void drawRect(uint16_t posX, uint16_t posY, 
              uint16_t width, uint16_t height, 
              uint8_t pixBit) {
  for (int x = posX; x < posX+height; x++) {
    InkPageSprite.drawPix(x, posY, pixBit);
    InkPageSprite.drawPix(x, posY+width-1, pixBit);
  }
  for (int y = posY; y < posY+width; y++) {
    InkPageSprite.drawPix(posX, y, pixBit);
    InkPageSprite.drawPix(posX+height-1, y, pixBit);
  }
}

void drawChar(uint16_t posX, uint16_t posY, char charData,
              Ink_eSPI_font_t *fontPtr) {
    charData -= 0x20;
    int _posX = posX + fontPtr->_height;
    for (int y = 0; y < fontPtr->_height; y++) {
        for (int x = 0; x < fontPtr->_width; x++) {
            uint8_t mark = 0x80 >> (y * fontPtr->_width + x) % 8;
            if ((fontPtr->_fontptr[charData * fontPtr->_fontSize +
                                   (y * fontPtr->_width + x) / 8]) &
                mark) {
                InkPageSprite.drawPix(_posX - y, posY + x, 0);
            } else {
                InkPageSprite.drawPix(_posX - y, posY + x, 1);
            }
        }
    }
}

void drawString(uint16_t posX, uint16_t posY, const char *charData,
                Ink_eSPI_font_t *fontPtr = &AsciiFont8x16,
                bool border = false) {
    if (posX == uint16_t(-1)) {  // draw centre of screen
      posX = (InkPageSprite.width() - fontPtr->_height) / 2;
    }
    if (posY == uint16_t(-1)) {  // draw centre of screen
      posY = (InkPageSprite.height() - strLen(charData) * fontPtr->_width) / 2;
    }
    int _posY = posY;
    while (*charData != '\0') {
        drawChar(posX, _posY, *charData, fontPtr);
        _posY += fontPtr->_width;
        charData++;
    }
    if (border) {
      drawRect(posX, posY, 
               strLen(charData)*fontPtr->_width,
               fontPtr->_height, 0);
    }
}

void drawTomato(int posX, int posY) {
  drawRect(posX, posY, 64, 64, 0);
}

void drawTime() {
  unsigned short minutesLeft = minutesTarget - minutesPassed;
  minutesLeft = 27;
  drawImage(10, 10, 18, 29, &num18x29[minutesLeft / 10]);
  drawImage(10, 30, 18, 29, &num18x29[minutesLeft % 10]);
}

void pushDrawMenu() {
  M5.M5Ink.clear();
  drawTomato(((200-64)*3)/4, (200-64)/2);
  drawString(-1, -1, "PoMoDoRo", &AsciiFont24x48);
  drawString(40, -1, "MID:start", &AsciiFont8x16);
  drawString(20, -1, "DOWN:mode", &AsciiFont8x16);
  InkPageSprite.pushSprite();
}

void pushDrawWorking() {
  InkPageSprite.FillRect(0,0,200,200,1);
  drawTomato(((200-64)*3)/4, (200-64)/2);
  drawString(-1, -1, "WORKING", &AsciiFont24x48);
  drawTime();
  InkPageSprite.pushSprite();
}

void pushDrawBreak() {
  InkPageSprite.FillRect(0, 0, (InkPageSprite.width()+AsciiFont24x48._height)/2, 200, 1);
  drawString(-1, -1, "BREAK", &AsciiFont24x48);
  InkPageSprite.pushSprite();
}

void updateMenu() {
  if (M5.BtnPWR.wasPressed()) {
    // shut down
    M5.M5Ink.clear();
    drawTomato((200-64)/2, (200-64)/2);
    InkPageSprite.pushSprite();
  }
  if (M5.BtnMID.wasPressed()) {
    // start work
    startTime = millis();
    drawWorking();
    currentState = STATE_WORKING;
  }
}

void updateTime() {
  
}

void setup() {
  // put your setup code here, to run once:
    M5.begin();                // Initialize CoreInk
    if (!M5.M5Ink.isInit()) {  // check if the initialization is successful.
        Serial.printf("Ink Init faild");
        delay(2000);
        M5.shutdown();  // Turn off the power
    }
    M5.M5Ink.clear();
    if (InkPageSprite.creatSprite(0, 0, 200, 200, true) != 0) {
        Serial.printf("Ink Sprite create failed");
    }
    pushDrawMenu();
}

void loop() {
  // put your main code here, to run repeatedly:
  //char msg[] = "ABCDEFG";
  switch (currentState) {
    case STATE_MENU:
      updateMenu();
      break;
    case STATE_WORKING:
    case STATE_BREAK:
      updateTime();
      break;
  }
  M5.update();
  delay(1000);
}
