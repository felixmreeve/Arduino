#include "M5CoreInk.h"

#include "icon.h"

// create a global sprite object for screen functionality
Ink_Sprite InkPageSprite(&M5.M5Ink);

int strLen(const char *charData) {
  int n = 0;
  while (charData[n] != '\0') {
    n++;
  }
  return n;
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
                Ink_eSPI_font_t *fontPtr = &AsciiFont8x16) {
    int _posY = posY;
    while (*charData != '\0') {
        drawChar(posX, _posY, *charData, fontPtr);
        _posY += fontPtr->_width;

        charData++;
    }
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
}

void loop() {
  // put your main code here, to run repeatedly:
    char msg[] = "abcdefg";
    //drawString(10, 10, msg, &AsciiFont8x16); //or AsciiFont24x48
    //drawRect(10, 10, strLen(msg)*AsciiFont8x16._width, AsciiFont8x16._height, 0);
    drawImage(10, 10, 18, 29, image_num_29_05);
    drawRect(10, 10, 18, 29, 0);
    InkPageSprite.pushSprite();
    delay(15000);
}
