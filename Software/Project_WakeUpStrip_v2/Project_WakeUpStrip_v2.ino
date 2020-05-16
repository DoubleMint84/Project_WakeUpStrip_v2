//----------------------НАСТРОЙКИ-----------------------
#define CLK 9
#define DT 10
#define CLK_tm 7
#define DIO 6
#define SW 11
#define maxArrSize 30
#define kolArr 10
#define oled_adr 0x78
#define clock_adr 0x68
//---------------------БИБЛИОТЕКИ
#include <OLED_I2C.h>
#include "DS3231.h"
#include "GyverEncoder.h"
#include <Wire.h>
#include "GyverTM1637.h"

Encoder enc1(CLK, DT, SW);  // для работы c кнопкой
OLED  myOLED(SDA, SCL);
RTClib rtc;
GyverTM1637 disp(CLK_tm, DIO);

extern uint8_t SmallFont[];

int value = 0, change = 0;
bool inMenu = false, dots = true;
DateTime t_now, t_prev;

char menu[][maxArrSize] = {"Settings", "Alarm1(--:--, OFF)", "Alarm2(--:--, OFF)", "Light", "BACK", "Sms", "Connection", "Reset", "Somebody", "once told me"};

void setup() {
  Wire.begin();
  t_now = rtc.now();
  t_prev = t_now;
  disp.clear();
  disp.brightness(7);
  disp.displayClock(byte(t_now.hour()), byte(t_now.minute()));
  if (!myOLED.begin(SSD1306_128X64))
    while (1);  // In case the library failed to allocate enough RAM for the display buffer...

  myOLED.setFont(SmallFont);
  enc1.setType(TYPE2);    // тип энкодера TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
  myOLED.clrScr();
  /* myOLED.print(F("Main menu"), LEFT, 0);
    for (int i = 0; i < ((kolArr - change > 6) ? 6 : kolArr - change); i++) {
     if (i == 0) {
       myOLED.print(String("->") + String(menu[change + i]), LEFT, 8 * (i + 1));
     } else {
       myOLED.print(String("  ") + String(menu[change + i]), LEFT, 8 * (i + 1));
     }
    }*/
  myOLED.update();
  disp.point(dots);
}

void loop() {
  inputTick();
  setupTick();
  t_now = rtc.now();
  /*if (t_now.second() != t_prev.second()){
    disp.point(!dots);
    dots = !dots;
    }*/
  if (t_now.minute() != t_prev.minute()) {
    disp.displayClock(byte(t_now.hour()), byte(t_now.minute()));
    t_prev = t_now;
  }
}

void setupTick() {
  if (enc1.isTurn() and inMenu) {       // если был совершён поворот (индикатор поворота в любую сторону)


    myOLED.clrScr();
    myOLED.print(F("Main menu"), CENTER, 0);
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

void inputTick() {
  enc1.tick();
  if (enc1.isClick()) {
    if (inMenu) {
      inMenu = false;
      myOLED.clrScr();
      myOLED.update();
    } else {
      inMenu = true;
      change = 0;
      myOLED.clrScr();
      myOLED.print(F("Main menu"), CENTER, 0);
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
  if (inMenu) {
    if (enc1.isLeft() and (change > 0)) change--;
    if (enc1.isRight() and (change < kolArr - 1)) change++;
  }
}
