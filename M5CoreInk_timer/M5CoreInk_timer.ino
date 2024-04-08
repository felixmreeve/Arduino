/*
  POMODORO timer based on
  https://github.com/simonprickett/m5stack-pomodoro
//*/

#include "M5CoreInk.h"
#include "icon.h"

enum state {
  STATE_MENU,
  STATE_WORKING,
  STATE_BREAK_SHORT,
  STATE_BREAK_LONG,
  STATE_INPUT,
  STATE_ERROR
};

char *stateStrings[] = {
  "MENU",
  "WORKING",
  "break",
  "BREAK",
  "INPUT",
  "ERROR"
};

const unsigned long stateMins[] = {
  0, // MENU, not used
  25, // WORKING
  5, // BREAK_SHORT
  20, // BREAK_LONG
  0, // INPUT
  0 // ERROR
};

// can adjust number of seconds in a "minute" for testing
const unsigned long MINUTE_SECONDS = 60;
const unsigned long MINUTE_MILLIS = 1000 * MINUTE_SECONDS;
const unsigned long SHUTDOWN_SECONDS = MINUTE_MILLIS - 1000;
// number of working sessions before long break
const unsigned int BREAK_LOOP = 4;

const unsigned long MENU_TIMEOUT_SECONDS = 60;

unsigned long timeStart = 0;
unsigned long minutesPassed = 0;
unsigned long minutesTarget = 0;
unsigned int breakIteration = 0;
state currentState = STATE_ERROR;

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
    if (posX == -1) {  // draw centre of screen
      posX = (InkPageSprite.width() - width) / 2;
    }
    if (posY == -1) {  // draw centre of screen
      posY = (InkPageSprite.height() - height) / 2;
    }
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
              Ink_eSPI_font_t *fontPtr, 
              uint8_t pixBit = 0, bool alpha = false) {
    charData -= 0x20;
    int _posX = posX + fontPtr->_height;
    for (int y = 0; y < fontPtr->_height; y++) {
        for (int x = 0; x < fontPtr->_width; x++) {
            uint8_t mark = 0x80 >> (y * fontPtr->_width + x) % 8;
            if ((fontPtr->_fontptr[charData * fontPtr->_fontSize +
                                   (y * fontPtr->_width + x) / 8]) &
                mark) {
                InkPageSprite.drawPix(_posX - y, posY + x, pixBit);
            } else if (!alpha) {
                InkPageSprite.drawPix(_posX - y, posY + x, !pixBit);
            }
        }
    }
}

void drawString(uint16_t posX, uint16_t posY, const char *charData,
                Ink_eSPI_font_t *fontPtr = &AsciiFont8x16,
                uint8_t pixBit = 0,
                bool boxBorder = false, bool textBorder = false) {
  int _posY;
  if (posX == uint16_t(-1)) {  // draw centre of screen
    posX = (InkPageSprite.width() - fontPtr->_height) / 2;
  }
  if (posY == uint16_t(-1)) {  // draw centre of screen
    posY = (InkPageSprite.height() - strLen(charData) * fontPtr->_width) / 2;
  }
  _posY = posY;
  while (*charData != '\0') {
      if (textBorder) {
        drawChar(posX+1, _posY, *charData, fontPtr, !pixBit, true);
        drawChar(posX-1, _posY, *charData, fontPtr, !pixBit, true);
        drawChar(posX, _posY+1, *charData, fontPtr, !pixBit, true);
        drawChar(posX, _posY-1, *charData, fontPtr, !pixBit, true);
      }
      // draw char with alpha only if there's a textBorder
      drawChar(posX, _posY, *charData, fontPtr, pixBit, textBorder);
      _posY += fontPtr->_width;
      charData++;
  }
  if (boxBorder) {
    drawRect(posX, posY, 
             strLen(charData)*fontPtr->_width,
             fontPtr->_height, 0);
  }
}

void drawTomato(int posX, int posY) {
  drawRect(posX, posY, 64, 64, 0);
}

void drawTime() {
  unsigned long minutesLeft = minutesTarget - minutesPassed;
  int percent = (minutesLeft * 100) / minutesTarget;
  // pixel images
  //drawImage(InkPageSprite.width()/4 - 29/2, InkPageSprite.height()/2 - 18, 18, 29, num18x29[minutesLeft / 10]);
  //drawImage(InkPageSprite.width()/4 - 29/2, InkPageSprite.height()/2, 18, 29, num18x29[minutesLeft % 10]);
  // font text
  char num[3] = "00";
  num[0] = '0' + minutesLeft / 10;
  num[1] = '0' + minutesLeft % 10;
  drawString(34, -1, num, &AsciiFont24x48, 0);
  // draw line for loading bar
  for (int i=100-percent; i<100+percent; i++) {
    InkPageSprite.drawPix(32, i, 0);
  }
}

void drawBreakIterations() {
  for (int i=0; i<BREAK_LOOP; i++) {
    // draw representation of breaks
    // and signify which break we're on

  }
}

void pushDrawMenu() {
  Serial.println("drawing menu");
  InkPageSprite.FillRect(0, 0, 200, 200, 1);
  drawTomato(((200-64)*3)/4, (200-64)/2);
  drawString(-1, -1, "PoMoDoRo", &AsciiFont24x48);
  switch (getNextState()) {
      case STATE_WORKING:
        drawString(40, -1, "MID:start task", &AsciiFont8x16);
        break;
      case STATE_BREAK_SHORT:
        drawString(40, -1, "MID:short break", &AsciiFont8x16);
        break;
      case STATE_BREAK_LONG:
        drawString(40, -1, "MID:long break", &AsciiFont8x16);
        break;
      default:
        drawString(40, -1, "UNKNOWN STATE", &AsciiFont8x16);
  }
  drawString(20, -1, "DOWN:mode", &AsciiFont8x16);
  InkPageSprite.pushSprite();
}

void pushDrawIteration() {
  char *currentStateString = stateStrings[currentState];
  Serial.printf("drawing state: %s\n", currentStateString);
  InkPageSprite.FillRect(0,0,200,200,1);
  drawTomato(((200-64)*3)/4, (200-64)/2);
  drawString(-1, -1, currentStateString, &AsciiFont24x48);
  drawTime();
  drawBreakIterations();
  InkPageSprite.pushSprite();
}

void quit() {
    // shut down
    M5.M5Ink.clear();
    delay(2000);
    InkPageSprite.FillRect(0, 0, 200, 200, 1);
    drawTomato((200-64)/2, (200-64)/2);
    InkPageSprite.pushSprite();
    delay(2000);
    M5.shutdown();
}

state getNextState() {
  state nextState;
  if (breakIteration % 2 == 0) {
    nextState = STATE_WORKING;
  }
  else if ((breakIteration/2 + 1) % BREAK_LOOP == 0) {
    nextState = STATE_BREAK_LONG;
  }
  else {
    nextState = STATE_BREAK_SHORT;
  }
  return nextState;
}

void setNextState() {
  Serial.printf("next state after iteration: %d\n", breakIteration);
  state nextState = getNextState();
  setState(nextState);
  breakIteration++;
}

void updateMenu() {
  unsigned long timeNow = millis();
  if (M5.BtnMID.wasPressed()) {
    Serial.println("MID pressed");
    setNextState();
    pushDrawIteration();
  }
  if (M5.BtnDOWN.wasPressed()) {
    setNextState();  // skip this state
    setState(STATE_MENU);
    pushDrawMenu();
  }
  if (M5.BtnPWR.wasPressed() || timeNow/1000 > MENU_TIMEOUT_SECONDS) {
    quit();
  }
}

void updateTime() {
  unsigned long timeNow;
  unsigned long origMinutesPassed = minutesPassed;
  while (minutesPassed == origMinutesPassed) {
    timeNow = millis();
    // purposely only record minute to avoid
    // unnecessary precision distraction
    minutesPassed = (timeNow - timeStart) / MINUTE_MILLIS;
    delay(500);
  }
  if (minutesPassed == minutesTarget) {
    setState(STATE_MENU);
    pushDrawMenu();
  }
  else {
    pushDrawIteration();
  }
}

void setState(state newState) {
  Serial.printf("setting state to: %s\n", stateStrings[newState]);
  currentState = newState;
  minutesTarget = stateMins[newState];
  minutesPassed = 0;
  timeStart = millis();
}

void setup() {
  // put your setup code here, to run once:
  M5.begin();                // Initialize CoreInk
  //Serial.begin(9600);
  if (!M5.M5Ink.isInit()) {  // check if the initialization is successful.
      Serial.printf("Ink Init faild");
      delay(2000);
      M5.shutdown();  // Turn off the power
  }
  if (InkPageSprite.creatSprite(0, 0, 200, 200, true) != 0) {
      Serial.printf("Ink Sprite create failed");
      delay(2000);
      M5.shutdown();  // Turn off the power
  }
  Serial.println("setup success");
  Serial.flush();
  M5.M5Ink.clear();
  delay(1000);
  setState(STATE_MENU);
  pushDrawMenu();
}

void loop() {
  Serial.println("loop");
  Serial.println(stateStrings[currentState]);
  // put your main code here, to run repeatedly:
  switch (currentState) {
    case STATE_MENU:
      updateMenu();
      delay(1000);
      break;
    case STATE_WORKING:
    case STATE_BREAK_SHORT:
    case STATE_BREAK_LONG:
      delay(SHUTDOWN_SECONDS);
      updateTime();
      break;
    default:
      Serial.println("UNKNOWN STATE");
  }
  // check for buttons
  M5.update();
}
