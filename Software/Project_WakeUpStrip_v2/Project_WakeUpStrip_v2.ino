#define CLK 9
#define DT 10
#define SW 11
#define maxArrSize 30
#define kolArr 10

#include <OLED_I2C.h>

#include "GyverEncoder.h"
//Encoder enc1(CLK, DT);      // для работы без кнопки
Encoder enc1(CLK, DT, SW);  // для работы c кнопкой
OLED  myOLED(SDA, SCL);
extern uint8_t SmallFont[];
//Encoder enc1(CLK, DT, SW, TYPE2);  // для работы c кнопкой и сразу выбираем тип
//Encoder enc1(CLK, DT, ENC_NO_BUTTON, TYPE2);  // для работы без кнопки и сразу выбираем тип

int value = 0, change = 0;

char menu[][maxArrSize] = {"Settings", "Alarm1(--:--, OFF)", "Alarm2(--:--, OFF)", "Light", "BACK", "Sms", "Connection", "Reset", "Somebody", "once told me"};

void setup() {
  if (!myOLED.begin(SSD1306_128X64))
    while (1);  // In case the library failed to allocate enough RAM for the display buffer...

  myOLED.setFont(SmallFont);
  enc1.setType(TYPE2);    // тип энкодера TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
  myOLED.clrScr();
  myOLED.print(F("Main menu"), LEFT, 0);
  for (int i = 0; i < ((kolArr - change > 6) ? 6 : kolArr - change); i++) {
    if (i == 0) {
      myOLED.print(String("->") + String(menu[change + i]), LEFT, 8 * (i + 1));
    } else {
      myOLED.print(String("  ") + String(menu[change + i]), LEFT, 8 * (i + 1));
    }
  }
  myOLED.update();

}

void loop() {
  // обязательная функция отработки. Должна постоянно опрашиваться
  enc1.tick();



  if (enc1.isTurn()) {       // если был совершён поворот (индикатор поворота в любую сторону)
    if (enc1.isLeft() and (change > 0)) change--;
    if (enc1.isRight() and (change < kolArr - 1)) change++;

    myOLED.clrScr();
    myOLED.print(F("Main menu"), LEFT, 0);
    for (int i = 0; i < ((kolArr - change > 6) ? 6 : kolArr - change); i++) {
      if (i == 0) {
        myOLED.print(String("->") + String(menu[change + i]), LEFT, 8 * (i + 1));
      } else {
        myOLED.print(String("  ") + String(menu[change + i]), LEFT, 8 * (i + 1));
      }
    }
    myOLED.update();
  }
}
