#include "M5CoreInk.h"

// create a global sprite object for screen functionality
Ink_Sprite InkPageSprite(&M5.M5Ink);

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
    InkPageSprite.drawString(10, 100, "success!");
    InkPageSprite.pushSprite();
    delay(15000);
}
