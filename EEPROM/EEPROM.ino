/*
Edited from:
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with CoreInk sample source code
*                          配套  CoreInk 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/coreink
* 获取更多资料请访问：https://docs.m5stack.com/zh_CN/core/coreink
*
* Describe: EEPROM Write.
* Date: 2021/11/28
*******************************************************************************
*/

#include <M5CoreInk.h>
#include <EEPROM.h>

Ink_Sprite InkPageSprite(&M5.M5Ink);

int addr = 0;  // EEPROM Start number of an ADDRESS
#define SIZE 32  // define the size of EEPROM(Byte)

void setup() {
    M5.begin();                      // Init CoreInk
    digitalWrite(LED_EXT_PIN, LOW);  // LED_EXT_PIN?
    if (!M5.M5Ink.isInit()) {
        Serial.printf("Ink Init faild");
        while (1) delay(100);
    }
    M5.M5Ink.clear();  // clear the screen
    delay(1000);
    // creat ink Sprite
    if (InkPageSprite.creatSprite(0, 0, 200, 200, true) != 0) {
        Serial.printf("Ink Sprite creat faild");
    }
    InkPageSprite.drawString(10, 10, "EEPROM");
    InkPageSprite.pushSprite()  // write to screen
    if (!EEPROM.begin(SIZE)) {  // Request storage of SIZE size(success return
                                // 1).  申请SIZE大小的存储(成功返回1)
        Serial.println(
            "\nFailed to initialise EEPROM!");  //串口输出格式化字符串.  Serial
                                                // output format string
        delay(1000000);
    }
    Serial.println("\nRead data from EEPROM. Values are:");
    for (int i = 0; i < SIZE; i++) {
        Serial.printf("%d ",
                      EEPROM.read(i));  // Reads data from 0 to SIZE in EEPROM.
                                        // 读取EEPROM中从0到SIZE中的数据
    }
    Serial.println("\n\nPress BtnA to Write EEPROM");
}

void loop() {
    M5.update();  // get button states
    if (M5.BtnEXT.isPressed()) {  // if the button.A is Pressed
        Serial.printf("\n%d Bytes datas written on EEPROM.\nValues are:\n",
                      SIZE);
        for (int i = 0; i < SIZE; i++) {
            int val = random(
                256);  // Integer values to be stored in the EEPROM (EEPROM can
                       // store one byte per memory address, and can only store
                       // numbers from 0 to 255. Therefore, if you want to use
                       // EEPROM to store the numeric value read by the analog
                       // input pin, divide the numeric value by 4.
            EEPROM.write(addr,
                         val);  // Writes the specified data to the specified
                                // address
            Serial.printf("%d ", val);
            addr += 1;  // Go to the next storage address
        }
        // When the storage address sequence number reaches the end of the
        // storage space of the EEPROM, return to.
        addr = 0;
        Serial.println("\n\nRead form EEPROM. Values are:");
        for (int i = 0; i < SIZE; i++) {
            Serial.printf("%d ", EEPROM.read(i));
        }
        Serial.println("\n-------------------------------------\n");
        // end EEPROM will write to flash (can use EEPROM.commit() instead)
        //EEPROM.commit()
        EEPROM.end();
        M5.shutdown()
    }
    delay(150);
}